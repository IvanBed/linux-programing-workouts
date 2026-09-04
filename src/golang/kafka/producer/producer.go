package main

import (
	"context"
	"log"

	kf "github.com/segmentio/kafka-go"
)

func mainLoop(writer *kf.Writer, ctx context.Context) {

	for {
		err := writer.WriteMessages(ctx, kf.kafka.Message{
			Value: []byte("Hello, Kafka!"),
		})
		if err != nil {
			log.Fatal("Ошибка при отправке:", err)
		}
	}
}

func main() {
	ctx := context.Background()

	writer := kf.kafka.NewWriter(kf.kafka.WriterConfig{
		Brokers: []string{"localhost:9092"},
		Topic:   "test-topic",
	})
	defer writer.Close()

	mainLoop(writer, ctx)
}
