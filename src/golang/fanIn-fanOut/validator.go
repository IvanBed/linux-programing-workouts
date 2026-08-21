// You can edit this code!
// Click here and start typing.
package main

import (
	"context"
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

func getRespAsync(url Url, chOut chan Response) {

	var resp Response = Response(strconv.FormatInt(int64(url.priority), 10))
	randInt := rand.IntN(4)
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
	}

	chOut <- resp
	//close(chOut)
}

func checkUrl(chIn chan Url, chOut chan Response, wg *sync.WaitGroup, ctx context.Context) {
	defer wg.Done()
	respChan := make(chan Response)
	var resp Response
	for url := range chIn {
		//fmt.Println(url)
		go getRespAsync(url, respChan)
		select {
		case <-ctx.Done():
			resp = Response(strconv.FormatInt(int64(url.priority), 10)) + " 500 Internal Server Error"
		case resp = <-respChan:

		}
		chOut <- resp
	}

}

func makeChans[T any](bufSize int) []chan T {
	chans := make([]chan T, 3)
	chans[High] = make(chan T, bufSize)
	chans[Middle] = make(chan T, bufSize)
	chans[Low] = make(chan T, bufSize)
	return chans
}

func requestWorkerPool(urls []Url, workersCnt int, ctx context.Context) []chan Response {
	if workersCnt < 3 {
		workersCnt = 3
	}

	ctxWithTimeout, cancelFunction := context.WithTimeout(ctx, time.Duration(900)*time.Millisecond)

	var wg sync.WaitGroup

	chansInput := makeChans[Url](workersCnt)
	chansOutput := makeChans[Response](0)

	wg.Add(workersCnt + 1)

	for i := 0; i < workersCnt; i++ {
		go checkUrl(chansInput[i%3], chansOutput[i%3], &wg, ctxWithTimeout)
	}

	go func() {
		wg.Wait()
		for _, out := range chansOutput {
			close(out)
		}
		cancelFunction()
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
		go func(in chan Response, out *[]Response) {
			defer wg.Done()
			for resp := range in {
				*out = append(*out, resp)
			}
		}(ch, &responses[index])
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
	ctx := context.Background()
	ctxWithCancel, cancelFunction := context.WithCancel(ctx)

	defer func() {
		fmt.Println("Main Defer: canceling context")
		cancelFunction()
	}()

	urls := makeUrls(size)
	responses := make(chan Response, len(urls))

	chansOutput := requestWorkerPool(urls, 6, ctxWithCancel)

	go merge(chansOutput, responses)

	for resp := range responses {
		fmt.Println(resp)
	}
	fmt.Println("Ready")
}
