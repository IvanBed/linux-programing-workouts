package main

import (
	"fmt"
	"os"

	"github.com/confluentinc/confluent-kafka-go/kafka"
)

func mainLoop(consumer *kafka.Consumer) {
	msg_count := 0
	run := true
	MIN_COMMIT_COUNT := 1000
	for run == true {
		ev := consumer.Poll(100)
		switch e := ev.(type) {
		case *kafka.Message:
			msg_count += 1
			if msg_count%MIN_COMMIT_COUNT == 0 {
				consumer.Commit()
			}
			fmt.Printf("%% Message on %s:\n%s\n", e.TopicPartition, string(e.Value))
		case kafka.Error:
			fmt.Fprintf(os.Stderr, "%% Error: %v\n", e)
			run = false
		default:
			fmt.Printf("Ignored %v\n", e)
		}
	}
}

func main() {
	// Настройка конфигурации консьюмера
	config := &kafka.ConfigMap{
		"bootstrap.servers": "localhost:9092",
		"group.id":          "myGroup",
		"auto.offset.reset": "smallest",
	}

	consumer, err := kafka.NewConsumer(config)
	if err != nil {
		panic(fmt.Sprintf("Failed to create consumer: %v", err))
	}

	if err != nil {
		panic(err)
	}

	err = consumer.SubscribeTopics([]string{"async-topic", "sync-topic"}, nil)

	if err != nil {
		panic(err)
	}

	fmt.Println("Consumer initialized")

	mainLoop(consumer)

	consumer.Close()
}
