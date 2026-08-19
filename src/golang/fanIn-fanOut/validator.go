// You can edit this code!
// Click here and start typing.
package main

import (
	"fmt"
	"math/rand/v2"
	"sync"
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

func getResp(url string) Response {

	var resp Response
	randInt := rand.IntN(5)

	switch randInt {
	case 0:
		resp = "200 OK"
	case 1:
		resp = "301 Moved Permanently"
	case 2:
		resp = "404 Not Found"
	case 3:
		resp = "400 Bad Request"
	case 4:
		resp = "500 Internal Server Error"
	}
	return resp
}

func checkUrl(chIn chan string, chOut chan Response, wg *sync.WaitGroup) {
	cnt := 0

	defer func() {
		close(chOut)
		wg.Done()
	}()

	for {
		select {
		case url, ok := <-chIn:
			if !ok {
				fmt.Println("cnt: ", cnt)
				fmt.Println("close")

				return
			}
			resp := getResp(url)
			//fmt.Println(resp)
			cnt++
			chOut <- resp
		}
	}
}

func manage(urls []Url, chansOutput []chan Response, wg *sync.WaitGroup) {
	highInput := make(chan string)
	midInput := make(chan string)
	lowInput := make(chan string)

	go checkUrl(highInput, chansOutput[0], wg)
	go checkUrl(midInput, chansOutput[1], wg)
	go checkUrl(lowInput, chansOutput[2], wg)

	for _, url := range urls {
		//fmt.Println(url)
		switch url.priority {
		case 0:
			highInput <- url.name
		case 1:
			midInput <- url.name
		case 2:
			lowInput <- url.name
		}
	}

	close(highInput)
	close(midInput)
	close(lowInput)
	wg.Done()
}

func requestWorkerPool(urls []Url, chansOutput []chan Response, workersCnt int, wg *sync.WaitGroup) {

	var chansIn []chan string
	chansIn[0] = make(chan string)
	chansIn[1] = make(chan string)
	chansIn[2] = make(chan string)

	wg.Add(workersCnt)

	for i := 0; i < workersCnt; i++ {

		go checkUrl(chansIn[i%3], chansOutput[i%3], wg)
	}

	go func() {
		wg.Wait()
		for _, out := range chansOutput {
			close(out)
		}
	}()

	go func() {
		for _, url := range urls {
			chansIn[url.priority] <- url.name
		}

		//for
	}()

}

func merge(chans []chan Response, out chan Response, wg *sync.WaitGroup) {
	var resp Response
	var highStatus bool = true
	var midStatus bool = true
	var lowStatus bool = true

	high := make([]Response, 0)
	mid := make([]Response, 0)
	low := make([]Response, 0)
	cnt := 0

	defer func() {
		fmt.Println("Total cnt: ", cnt)
		fillOutputChannel(out, high, mid, low)
		wg.Done()
	}()

	for {
		if !highStatus && !midStatus && !lowStatus {
			return
		}

		select {
		case resp, highStatus = <-chans[0]:
			if resp != "" {
				cnt++
				high = append(high, resp)
			}
		case resp, midStatus = <-chans[1]:
			if resp != "" {
				cnt++
				mid = append(mid, resp)
			}
		case resp, lowStatus = <-chans[2]:
			if resp != "" {
				cnt++
				low = append(low, resp)
			}
		default:
			//fmt.Println(highStatus, midStatus, lowStatus)
		}

	}
}

func mergeNew(chans []chan Response) chan Response {
	var resp Response
	var highStatus bool = true
	var midStatus bool = true
	var lowStatus bool = true

	out := make(chan Response)

	high := make([]Response, 0)
	mid := make([]Response, 0)
	low := make([]Response, 0)
	cnt := 0

	defer func() {
		fmt.Println("Total cnt: ", cnt)
		fillOutputChannel(out, high, mid, low)
	}()

	for {
		if !highStatus && !midStatus && !lowStatus {
			break
		}

		select {
		case resp, highStatus = <-chans[0]:
			if resp != "" {
				cnt++
				high = append(high, resp)
			}
		case resp, midStatus = <-chans[1]:
			if resp != "" {
				cnt++
				mid = append(mid, resp)
			}
		case resp, lowStatus = <-chans[2]:
			if resp != "" {
				cnt++
				low = append(low, resp)
			}
		default:
			//fmt.Println(highStatus, midStatus, lowStatus)
		}

	}
	return out
}

func makeUrls(size int) []Url {
	urls := make([]Url, size)
	for i := range urls {
		urls[i] = Url{name: RandStringBytes(10), priority: uint8(rand.IntN(3))}
	}
	return urls
}

func makeChans(bufSize int) []chan Response {
	chans := make([]chan Response, 3)
	chans[0] = make(chan Response, bufSize)
	chans[1] = make(chan Response, bufSize)
	chans[2] = make(chan Response, bufSize)
	return chans
}

func printUrls(urls []Url) {
	for _, url := range urls {
		fmt.Println(url)
	}
}

func main() {
	var wg sync.WaitGroup
	wg.Add(5)

	size := 1000

	urls := makeUrls(size)
	chans := makeChans(0)
	responses := make(chan Response, len(urls))

	go manage(urls, chans, &wg)
	go merge(chans, responses, &wg)
	wg.Wait()

	for i := 0; i < len(urls); i++ {
		fmt.Println(<-responses)
	}

	close(responses)
	fmt.Println("Ready")
}
