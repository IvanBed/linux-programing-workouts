package visualization

import (
	"client/internal/network"
	"fmt"
)

func allRoutineDone(routinesStatus []bool) bool {
	for _, val := range routinesStatus {
		if val == false {
			return false
		}
	}
	return true
}

func DownloadVisualization(ch chan network.VisualizationArgs, routinesCnt int) {

	var args network.VisualizationArgs
	var routinesStatus []bool
	var prevRoutineId int = -1
	var offset int
	routinesStatus = make([]bool, routinesCnt)

	for !allRoutineDone(routinesStatus) {
		select {
		case args = <-ch:
			if !args.EndFlag {
				prevRoutineId, offset = downloadVisualizationInternal(args.SegmentSize, args.CurrentSize, args.LinesCnt, args.RoutineId, routinesCnt, prevRoutineId, offset)
			} else {
				routinesStatus[args.RoutineId] = true
			}
		}
	}
}

func downloadVisualizationInternal(segmentSize int, currentSize int, linesCnt *int, routineId int, routinesCnt int, prevRoutineId int, offset int) (int, int) {

	if *linesCnt == 0 || *linesCnt*segmentSize <= currentSize {
		if prevRoutineId != -1 {
			if prevRoutineId != routineId {
				for i := 0; i < offset; i++ {
					fmt.Print("\033[D")
				}
				offset = 0
			} else {
				offset++
			}
		}
		switch routineId {
		case 0:
			fmt.Print(network.Green, "#")
		case 1:
			fmt.Print(network.Green, "\033[B", "#")
			fmt.Print("\033[A")
		case 2:
			fmt.Print(network.Green, "\033[B", "\033[B", "#")
			fmt.Print("\033[A", "\033[A")
		}
		*linesCnt++
	}

	for i := 0; i < routinesCnt; i++ {
		fmt.Print("\033[B")
	}
	return routineId, offset
}

func Test() {

}
