package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"os/signal"
	"strings"

	"github.com/IBM/sarama"
)

type ProducerResults struct {
	enqueued       int
	producerErrors int
}

func write(producer sarama.AsyncProducer, signals chan os.Signal, resultsCh chan ProducerResults) {

	var results ProducerResults

	reader := bufio.NewReader(os.Stdin)

ProducerLoop:
	for {
		text, _ := reader.ReadString('\n')
		text = strings.TrimSpace(text)
		if text == "STOP" {
			break
		}
		select {
		case producer.Input() <- &sarama.ProducerMessage{Topic: "my_topic", Key: nil, Value: sarama.StringEncoder(text)}:
			results.enqueued++
		case err := <-producer.Errors():
			log.Println("Failed to produce message", err)
			results.producerErrors++
		case <-signals:
			break ProducerLoop
		}
	}
	resultsCh <- results
	close(resultsCh)
}

func main() {
	//var wg sync.WaitGroup

	args := os.Args

	if len(args) < 3 {
		fmt.Println("")
	}

	host := args[1]
	port := args[2]
	//localhost:29092
	producer, err := sarama.NewAsyncProducer([]string{host + ":" + port}, nil)
	if err != nil {
		panic(err)
	}
	defer producer.Close()
	defer func() {
		if err := producer.Close(); err != nil {
			log.Fatalln(err)
		}
	}()

	// Trap SIGINT to trigger a shutdown.
	signals := make(chan os.Signal, 1)
	resultsCh := make(chan ProducerResults, 0)
	signal.Notify(signals, os.Interrupt)

	var totalEnqueued, totalProducerErrors int

	for i := 1; i <= 1; i++ {
		//wg.Add(1)
		go write(producer, signals, resultsCh)

	}

	for curResults := range resultsCh {
		totalEnqueued += curResults.enqueued
		totalProducerErrors += curResults.producerErrors
	}
	close(signals)
	log.Printf("Enqueued: %d; errors: %d\n", totalEnqueued, totalProducerErrors)
	//wg.Wait()
}
