package main

import (
	"fmt"
	"os"
	"sync"

	"github.com/confluentinc/confluent-kafka-go/kafka"
)

func syncConversation(producer *kafka.Producer) {
	// Создание канала доставки
	deliveryChan := make(chan kafka.Event)

	// Отправка сообщений в синхронном режиме
	topic := "sync-topic"
	for _, word := range []string{"this", "is", "synchronous", "message", "delivery", "in", "kafka", "with", "Go", "Client"} {
		producer.Produce(&kafka.Message{
			TopicPartition: kafka.TopicPartition{Topic: &topic, Partition: kafka.PartitionAny},
			Value:          []byte(word),
		}, deliveryChan)

		// Ожидание события доставки
		event := <-deliveryChan
		m := event.(*kafka.Message)

		if m.TopicPartition.Error != nil {
			fmt.Printf("Delivery failed: %v\n", m.TopicPartition.Error)
		} else {
			fmt.Printf("Delivered message to topic %s [%d] at offset %v\n",
				*m.TopicPartition.Topic, m.TopicPartition.Partition, m.TopicPartition.Offset)
		}
	}

	// Закрытие канала подтверждений
	close(deliveryChan)
}

func asyncConversation(producer *kafka.Producer) {
	var wg sync.WaitGroup

	topic := "async-topic"
	for _, word := range []string{"this", "is", "asynchronous", "message", "delivery", "in", "kafka", "with", "Go", "Client"} {
		producer.Produce(&kafka.Message{
			TopicPartition: kafka.TopicPartition{Topic: &topic, Partition: kafka.PartitionAny},
			Value:          []byte(word),
		}, nil)
	}
	// Асинхронная обработка событий продюсера
	wg.Add(1)
	go func() {
		for e := range producer.Events() {
			switch ev := e.(type) {
			case *kafka.Message:
				if ev.TopicPartition.Error != nil {
					fmt.Printf("Failed to deliver message: %v\n", ev.TopicPartition.Error)
				} else {
					fmt.Printf("Successfully produced record to topic %s partition [%d] @ offset %v\n",
						*ev.TopicPartition.Topic, ev.TopicPartition.Partition, ev.TopicPartition.Offset)
				}
			}
		}
	}()
	wg.Wait()
}

func main() {

	config := &kafka.ConfigMap{
		"bootstrap.servers": "localhost:9092",
		"acks":              "all",
		"client.id":         "myProducer",
	}

	producer, err := kafka.NewProducer(config)
	if err != nil {
		fmt.Printf("Failed to create producer: %s\n", err)
		os.Exit(1)
	}

	defer producer.Close()
	fmt.Println("Producer initialized")

}
