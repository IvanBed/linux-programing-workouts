// You can edit this code!
// Click here and start typing.
package main

import (
	"fmt"
	"sync"
)

func Merge(a <-chan string, b <-chan string) <-chan string {
	var wg sync.WaitGroup
	wg.Add(2)
	out := make(chan string, len(a)+len(b))

	go func() {
		for v := range a {
			out <- v
		}
		wg.Done()
	}()

	go func() {
		for v := range b {
			out <- v
		}
		wg.Done()
	}()

	go func() {
		wg.Wait()
		close(out)
	}()

	return out
}

func main() {
	var wg sync.WaitGroup
	a := make(chan string, 3)
	a <- "foo"
	a <- "bar"
	a <- "baz"
	close(a)

	// channel b contains 2 messages
	b := make(chan string, 2)
	b <- "hello"
	b <- "world"
	close(b)
	wg.Wait()
	// your implementation
	c := Merge(a, b)

	for el := range c {
		fmt.Println(el)
	}

}
