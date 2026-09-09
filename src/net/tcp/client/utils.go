package utils

import "fmt"

const Reset = "\033[0m"
const Red = "\033[31m"
const Green = "\033[32m"

func downloadVisualization(segmentSize int, currentSize int, linesCnt *int) {

	if *linesCnt == 0 || *linesCnt*segmentSize <= currentSize {
		fmt.Print(Green, "#")
		*linesCnt++
	}
}
