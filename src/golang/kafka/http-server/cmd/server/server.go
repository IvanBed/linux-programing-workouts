package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"time"

	"github.com/IBM/sarama"
)

type ButtonsEvents struct {
	buttonId    int
	buttoanName string
	clicksCnt   int
}

type ProducePostReq struct {
	producer sarama.AsyncProducer
}

func (this *ProducePostReq) write(be ButtonsEvents) {

	select {
	case this.producer.Input() <- &sarama.ProducerMessage{Topic: "my_topic", Key: sarama.StringEncoder(be.buttoanName), Value: sarama.StringEncoder(be.clicksCnt)}:

	case err := <-this.producer.Errors():
		log.Println("Failed to produce message", err)
	}
}

func (this *ProducePostReq) buttonsEventsHandler(w http.ResponseWriter, r *http.Request) {

	var be ButtonsEvents

	if r.Method != http.MethodPost {
		http.Error(w, "Only POST requests are allowed!", http.StatusMethodNotAllowed)
		return
	}

	err := r.ParseForm()
	if err != nil {
		fmt.Fprintf(w, "Error parsing form: %v", err)
		return
	}

	name := r.FormValue("name")
	fmt.Fprintf(w, "Hello, %s!", name)
	decoder := json.NewDecoder(r.Body)

	err = decoder.Decode(&be)
	if err != nil {
		panic(err)
	}

	this.write(be)

	log.Println(be.buttoanName)
	log.Println(be.clicksCnt)
}

func loggingMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		start := time.Now()
		log.Printf("🚀 Старт обработки %s %s", r.Method, r.URL.Path)
		next.ServeHTTP(w, r)
		log.Printf("🏁 Завершено за %v", time.Since(start))
	})
}

func main() {

	args := os.Args

	if len(args) < 3 {
		fmt.Println("")
	}

	host := args[1]
	port := args[2]

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
	signal.Notify(signals, os.Interrupt)
	defer close(signals)

	// Оборачиваем mux в middleware

	mux := http.NewServeMux()
	mux.HandleFunc("/post_buttons_events", buttonsEventsHandler)

	loggedMux := loggingMiddleware(mux)
	err = http.ListenAndServe(`:8080`, loggedMux)
	if err != nil {
		panic(err)
	}
}
