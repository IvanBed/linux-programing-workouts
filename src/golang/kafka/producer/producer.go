package main

import (
	"log"
	"os"
	"os/signal"

	"github.com/IBM/sarama"
)

func asyncConversation(producer sarama.AsyncProducer, signals chan os.Signal) (enqueued int, producerErrors int) {

ProducerLoop:
	for {
		select {
		case producer.Input() <- &sarama.ProducerMessage{Topic: "my_topic", Key: nil, Value: sarama.StringEncoder("testing 123")}:
			enqueued++
		case err := <-producer.Errors():
			log.Println("Failed to produce message", err)
			producerErrors++
		case <-signals:
			break ProducerLoop
		}
	}
	return
}

func main() {

	producer, err := sarama.NewAsyncProducer([]string{"localhost:29092"}, nil)
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
	signal.Notify(signals, os.Interrupt)

	var enqueued, producerErrors int

	enqueued, producerErrors = asyncConversation(producer, signals)

	log.Printf("Enqueued: %d; errors: %d\n", enqueued, producerErrors)
}
