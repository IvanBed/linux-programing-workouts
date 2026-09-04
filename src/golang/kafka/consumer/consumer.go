package main

import (
	"context"
	"fmt"
	"log"

	kf "github.com/segmentio/kafka-go"
)

func mainLoop(reader *kf.Reader, ctx context.Context) {

	for {

		msg, err := reader.ReadMessage(ctx)
		if err != nil {
			log.Fatal("Ошибка при получении:", err)
		}

		fmt.Println(string(msg.Value))
	}
}

func main() {

	ctx := context.Background()

	reader := kafka.NewReader(kafka.ReaderConfig{
		Brokers: []string{"localhost:9092"},
		Topic:   "test-topic",
		GroupID: "1",
	})
	defer reader.Close()

	mainLoop(reader, ctx)
}
