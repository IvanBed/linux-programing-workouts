// You can edit this code!
// Click here and start typing.
package main

import (
	"fmt"
	"math/rand/v2"
	"strconv"
	"sync"
	"time"
)

type Url struct {
	name     string
	priority uint8
}

type ResponseNew struct {
	code      string
	url       Url
	timestamp int64
}

type Response string

const (
	High = iota
	Middle
	Low
)

const letterBytes = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

func RandStringBytes(n int) string {
	b := make([]byte, n)
	for i := range b {
		b[i] = letterBytes[rand.IntN(len(letterBytes))]
	}
	return string(b)
}

func concatSlices(l []Response, r []Response) []Response {
	res := make([]Response, 0)

	for i := range l {
		res = append(res, l[i])
	}

	for i := range r {
		res = append(res, r[i])
	}
	return res
}

func fillOutputChannel(ch chan Response, slices ...[]Response) {

	for slice := range slices {
		for _, resp := range slices[slice] {
			ch <- resp
		}
	}
}

func getResp(url Url) Response {

	var resp Response = strconv.FormatInt(url.priority, 10)
	randInt := rand.IntN(5)
	randTime := rand.IntN(1000)
	time.Sleep(time.Duration(randTime) * time.Millisecond)
	switch randInt {
	case 0:
		resp += " 200 OK"
	case 1:
		resp += " 301 Moved Permanently"
	case 2:
		resp += " 404 Not Found"
	case 3:
		resp += " 400 Bad Request"
	case 4:
		resp += " 500 Internal Server Error"
	}
	return resp
}

func checkUrl(chIn chan Url, chOut chan Response, wg *sync.WaitGroup) {
	cnt := 0
	defer wg.Done()
	for url := range chIn {
		resp := getResp(url)
		fmt.Println(resp)
		chOut <- resp
		cnt++
	}
}

func makeChans[T any](bufSize int) []chan T {
	chans := make([]chan T, 3)
	chans[High] = make(chan T, bufSize)
	chans[Middle] = make(chan T, bufSize)
	chans[Low] = make(chan T, bufSize)
	return chans
}

func requestWorkerPool(urls []Url, workersCnt int) []chan Response {
	if workersCnt < 3 {
		workersCnt = 3
	}
	var wg sync.WaitGroup
	wg.Add(workersCnt + 1)
	chansInput := makeChans[Url](workersCnt)
	chansOutput := makeChans[Response](0)

	for i := 0; i < workersCnt; i++ {
		go checkUrl(chansInput[i%3], chansOutput[i%3], &wg)
	}

	go func() {
		wg.Wait()
		for _, out := range chansOutput {
			close(out)
		}
	}()

	go func() {
		defer wg.Done()
		for _, url := range urls {
			chansInput[url.priority] <- url
		}
		for _, in := range chansInput {
			close(in)
		}
	}()

	return chansOutput
}

func merge(chans []chan Response, out chan Response) {

	var wg sync.WaitGroup
	wg.Add(len(chans))

	responses := make([][]Response, 3)

	responses[High] = make([]Response, 0)
	responses[Middle] = make([]Response, 0)
	responses[Low] = make([]Response, 0)

	for index, ch := range chans {
		go func(in chan Response, out []Response) {
			defer wg.Done()
			for resp := range in {
				out = append(out, resp)
			}
		}(ch, responses[index])
	}

	go func() {
		wg.Wait()
		fillOutputChannel(out, responses[High], responses[Middle], responses[Low])
		close(out)
	}()
}

func makeUrls(size int) []Url {
	urls := make([]Url, size)
	for i := range urls {
		urls[i] = Url{name: RandStringBytes(10), priority: uint8(rand.IntN(3))}
	}
	return urls
}

func printUrls(urls []Url) {
	for _, url := range urls {
		fmt.Println(url)
	}
}

func main() {

	size := 10

	urls := makeUrls(size)
	responses := make(chan Response, len(urls))

	chansOutput := requestWorkerPool(urls, 6)

	go merge(chansOutput, responses)

	for resp := range responses {
		fmt.Println(resp)
	}
	fmt.Println("Ready")
}
