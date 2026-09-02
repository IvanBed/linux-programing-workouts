package producer

import (
	"context"
	"log"

	"github.com/segmentio/kafka-go"
)

func main() {
	ctx := context.Background()

	writer := kafka.NewWriter(kafka.WriterConfig{
		Brokers: []string{"localhost:9092"},
		Topic:   "my-topic",
	})
	defer writer.Close()

	err := writer.WriteMessages(ctx, kafka.Message{
		Value: []byte("Hello, Kafka!"),
	})
	if err != nil {
		log.Fatal("Ошибка при отправке:", err)
	}
}
