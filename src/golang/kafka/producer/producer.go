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
	buttonId   int
	buttonName string
	clicksCnt  int
}

type ProduceState struct {
	AsynProducer sarama.AsyncProducer
	InputCh      chan ButtonsEvents
	EndCh        chan bool
}

func NewProduceState(host string, port string) (*ProduceState, error) {

	var ps *ProduceState
	asyncProducer, err := sarama.NewAsyncProducer([]string{host + ":" + port}, nil)
	if err != nil {
		return nil, err
	}
	inputCh := make(chan ButtonsEvents, 0)
	endCh := make(chan bool, 0)
	ps = &ProduceState{AsynProducer: asyncProducer, InputCh: inputCh, EndCh: endCh}
	return ps, nil
}

func (ps *ProduceState) Close() {
	ps.AsynProducer.Close()
}

func (this *ProduceState) writeAsync(be ButtonsEvents) {
	select {
	case this.AsynProducer.Input() <- &sarama.ProducerMessage{Topic: "buttons_events", Key: sarama.StringEncoder(be.buttonName), Value: sarama.StringEncoder(be.clicksCnt)}:

	case err := <-this.AsynProducer.Errors():
		log.Println("Failed to produce message", err)
	}
}

func (this *ProduceState) mainLoop(wg *sync.WaitGroup) {
	var be ButtonsEvents
	defer wg.Done()

	for {
		select {
		case be = <-this.InputCh:
			this.writeAsync(be)
		case <-this.EndCh:
			return
		}
	}
}

func buttonsEventsHandler(ch chan ButtonsEvents) http.HandlerFunc {

	return func(w http.ResponseWriter, r *http.Request) {

		var be ButtonsEvents

		if r.Method != http.MethodPost {
			http.Error(w, "Only POST requests are allowed!", http.StatusMethodNotAllowed)
			return
		}

		err := r.ParseForm()
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		decoder := json.NewDecoder(r.Body)
		err = decoder.Decode(&be)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
		fmt.Println("Send data to topic buttons_events")
		ch <- be
		w.WriteHeader(http.StatusOK)

	}
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
	var wg sync.WaitGroup
	var producerState *ProduceState
	args := os.Args

	if len(args) < 3 {
		fmt.Println("")
	}

	kafkaHost := args[1]
	kafkaPort := args[2]
	//APIHost := args[3]
	APIPort := args[3]

	producerState, err := NewProduceState(kafkaHost, kafkaPort)
	if err != nil {
		log.Printf("")
	}
	defer producerState.Close()

	wg.Add(1)
	go producerState.mainLoop(&wg)

	mux := http.NewServeMux()
	mux.HandleFunc("/post_buttons_events", buttonsEventsHandler(producerState.InputCh))
	loggedMux := loggingMiddleware(mux)

	fmt.Println(":" + APIPort)

	err = http.ListenAndServe(":"+APIPort, loggedMux)
	if err != nil {
		log.Printf("")
	}

	producerState.EndCh <- true
	wg.Wait()
	return
}

//curl -d '{"buttonId":"1", "buttonName":"test", "clicksCnt":"1"}' -H "Content-Type: application/json" -X POST http://localhost:22007/post_buttons_events
