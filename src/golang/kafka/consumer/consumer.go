package main

import (
	"context"
	"fmt"
	"log"
	"os"
	"os/signal"
	"slices"
	"strconv"
	"strings"
	"sync"
	"syscall"
	"time"

	"github.com/IBM/sarama"

	"db/database"
)

type ButtonEvents struct {
	buttonId   int
	buttonName string
	clicksCnt  int
}

type ConsumerState struct {
	Consumer     sarama.Consumer
	PartConsumer sarama.PartitionConsumer
	EndCh        chan bool
}

func NewConsumerState(host string, port string, targetTopic string) (*ConsumerState, error) {
	var cs *ConsumerState
	consumer, err := sarama.NewConsumer([]string{host + ":" + port}, nil)
	if err != nil {
		//log.Fatalf("Failed to create consumer: %v", err)
		return nil, err
	}

	partConsumer, err := consumer.ConsumePartition(targetTopic, 0, sarama.OffsetOldest)
	if err != nil {
		//log.Fatalf("Failed to consume partition: %v", err)
		return nil, err
	}
	endCh := make(chan bool, 0)
	cs = &ConsumerState{Consumer: consumer, PartConsumer: partConsumer, EndCh: endCh}
	return cs, nil
}

func (cs *ConsumerState) Close() {
	cs.PartConsumer.Close()
	cs.Consumer.Close()
}

func parseButtonEvents(key, value []byte) (ButtonEvents, error) {
	var be ButtonEvents
	var err error
	keyStr := string(key)
	keyStrTokens := strings.Split(keyStr, ":")

	be.buttonId, err = strconv.Atoi(keyStrTokens[0])
	if err != nil {
		return be, err
	}
	be.buttonName = keyStrTokens[1]
	be.clicksCnt, err = strconv.Atoi(string(value))
	if err != nil {
		return be, err
	}
	return be, nil
}

func mainLoop(consumerState *ConsumerState, batchSize int, errCh chan error, wg *sync.WaitGroup) {
	defer func() {
		close(errCh)
		wg.Done()
	}()

	batch := make([]ButtonEvents, batchSize)
	timer := time.NewTimer(16 * time.Millisecond)

	for {
		select {
		case endSignal, ok := <-consumerState.EndCh:
			if !ok || endSignal == true {
				return
			}
		case msg, ok := <-consumerState.PartConsumer.Messages():
			if !ok {
				fmt.Println("Channel closed, exiting goroutine")
				return
			}

			if len(batch) == batchSize {
				go database.StoreButtonEvents(context.Background(), slices.Clone(batch), errCh)
				clear(batch)
			}

			be, err := parseButtonEvents(msg.Key, msg.Value)
			if err != nil {
				fmt.Println("Could not parse the message from Kafka, check the next")
				continue
			}
			batch = append(batch, be)

		case <-timer.C:
			go database.StoreButtonEvents(context.Background(), slices.Clone(batch), errCh)
			timer.Reset()
			clear(batch)
		}
	}
}

func errorHandle(errCh chan error, errorsStore []error, wg *sync.WaitGroup) {
	defer wg.Done()
	for err := range errCh {
		errorsStore = append(errorsStore, err)
	}
}

func printErrors(errorsStore []error) {

}

func main() {

	var batchSize int
	var errStore []error
	var errCh chan error
	var wg sync.WaitGroup
	var consumerState *ConsumerState

	args := os.Args

	if len(args) < 3 {
		fmt.Println("Please specify the Kafka IP address and port")
		return
	}
	signalContex, stop := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer stop()

	kafkaHost := args[1]
	kafkaPort := args[2]

	consumerState, err := NewConsumerState(kafkaHost, kafkaPort, "my_topic")
	if err != nil {
		fmt.Println("Could not create Consumer State")
		log.Printf("")
		return
	}
	defer consumerState.Close()

	err = database.InitConnsPoolFromEnvs()
	if err != nil {
		fmt.Println("Could not create a connections pool")
		log.Printf("")
		return
	}
	defer database.ClosePool()

	errStore = make([]error, 0)
	errCh = make(chan error, 0)
	batchSize = 16

	wg.Add(2)
	fmt.Println("Start error handling routine")
	go errorHandle(errCh, errStore, &wg)
	fmt.Println("Start consumer main loop")
	go mainLoop(consumerState, batchSize, errCh, &wg)

	fmt.Println("Programm is running...")
	<-signalContex.Done()
	fmt.Println("Signal has been handeled, stop the programm")
	consumerState.EndCh <- true
	wg.Wait()
	fmt.Println("Errors")
	printErrors(errStore)
}
