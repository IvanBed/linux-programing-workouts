package main

import (
	"fmt"
	"log"
	"sync"

	"github.com/IBM/sarama"
)

func mainLoop(partConsumer sarama.PartitionConsumer, mtx *sync.Mutex, responseChannels map[string]chan *sarama.ConsumerMessage) {

	//MIN_COMMIT_COUNT := 1000
	for {
		select {
		// Чтение сообщения из Kafka
		case msg, ok := <-partConsumer.Messages():
			if !ok {
				fmt.Println("Channel closed, exiting goroutine")
				return
			}
			responseID := string(msg.Key)
			msgVal := string(msg.Value)
			fmt.Printf("сonsumed message values %d offset %d\n", msgVal, msg.Offset)

			mtx.Lock()
			ch, exists := responseChannels[responseID]
			if exists {
				ch <- msg
				delete(responseChannels, responseID)
			}
			mtx.Unlock()
		}
	}
}

func main() {
	// Настройка конфигурации консьюмера
	var responseChannels map[string]chan *sarama.ConsumerMessage
	var mtx sync.Mutex

	consumer, err := sarama.NewConsumer([]string{"localhost:29092"}, nil)
	if err != nil {
		log.Fatalf("Failed to create consumer: %v", err)
	}
	defer consumer.Close()

	partConsumer, err := consumer.ConsumePartition("my_topic", 0, sarama.OffsetOldest)
	if err != nil {
		log.Fatalf("Failed to consume partition: %v", err)
	}
	defer partConsumer.Close()

	fmt.Println("Consumer initialized")

	mainLoop(partConsumer, &mtx, responseChannels)
}
