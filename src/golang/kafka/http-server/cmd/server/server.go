package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"sync"
	"time"

	"github.com/IBM/sarama"
)

type ButtonsEvents struct {
	buttonId    int
	buttoanName string
	clicksCnt   int
}

type ProduceState struct {
	AsynProducer sarama.AsyncProducer
	Mtx          sync.Mutex
}

func NewProduceState(host string, port string) (*ProduceState, error) {

	var ps *ProduceState
	asyncProducer, err := sarama.NewAsyncProducer([]string{host + ":" + port}, nil)
	if err != nil {
		return nil, err
	}
	ps = &ProduceState{AsynProducer: asyncProducer}
	return ps, nil
}

func (ps *ProduceState) Close() {
	ps.AsynProducer.Close()
}

func (this *ProduceState) writeAsync(be ButtonsEvents) {
	this.Mtx.Lock()
	select {
	case this.AsynProducer.Input() <- &sarama.ProducerMessage{Topic: "my_topic", Key: sarama.StringEncoder(be.buttoanName), Value: sarama.StringEncoder(be.clicksCnt)}:

	case err := <-this.AsynProducer.Errors():
		log.Println("Failed to produce message", err)
	}
	this.Mtx.Unlock()
}

func (this *ProduceState) buttonsEventsHandler(w http.ResponseWriter, r *http.Request) {

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

	decoder := json.NewDecoder(r.Body)
	err = decoder.Decode(&be)
	if err != nil {
		panic(err)
	}

	this.writeAsync(be)
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

	var producerState *ProduceState
	args := os.Args

	if len(args) < 3 {
		fmt.Println("")
	}

	kafkaHost := args[1]
	kafkaPort := args[2]
	APIHost := args[3]
	APIPort := args[4]

	producerState, err := NewProduceState(kafkaHost, kafkaPort)
	if err != nil {
		panic(err)
	}
	defer producerState.Close()

	mux := http.NewServeMux()
	mux.HandleFunc("/post_buttons_events", producerState.buttonsEventsHandler)
	loggedMux := loggingMiddleware(mux)

	err = http.ListenAndServe(APIHost+":"+APIPort, loggedMux)
	if err != nil {
		panic(err)
	}
}
