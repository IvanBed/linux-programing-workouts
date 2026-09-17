package visualization

import (
	"client/internal/network"
	"fmt"
)

type VisualizationArgs struct {
	RoutineId     int
	CurrentOffset int
	TotalOffset   *int
	EndFlag       bool
}

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

	routinesStatus = make([]bool, routinesCnt)

	for !allRoutineDone(routinesStatus) {
		select {
		case args = <-ch:
			if !args.EndFlag {
				downloadVisualizationInternalNew(args.CurrentOffset, args.TotalOffset, args.RoutineId, routinesCnt)
			} else {
				routinesStatus[args.RoutineId] = true
			}
		}
	}
	for i := 0; i < routinesCnt; i++ {
		fmt.Println("")
	}
}

func findCursorPos(offset int) {
	var pos string
	pos = fmt.Sprintf("\033[%dG", offset)
	fmt.Printf(pos)
}

func downloadVisualizationInternal(CurrentOffset int, TotalOffset *int, routineId int, routinesCnt int) {
	var offset int = CurrentOffset + *TotalOffset
	var tempalte string
	var tempLen int = 0

	switch routineId {
	case 0:
		if offset == 1 {
			tempalte = fmt.Sprintf("File №%d ", routineId)
			findCursorPos(1)
			fmt.Print(network.Reset, tempalte)
			tempLen = len(tempalte)
			offset += tempLen
		}
		findCursorPos(offset)
		fmt.Print(network.Green, "#")
		if offset == 51 {
			fmt.Print(network.Reset, " Ok!")
		}
	case 1:
		fmt.Print("\033[B")
		if offset == 1 {
			tempalte = fmt.Sprintf("File №%d ", routineId)
			findCursorPos(1)
			fmt.Print(network.Reset, tempalte)
			offset += len(tempalte)
			tempLen = len(tempalte)
		}
		findCursorPos(offset)
		fmt.Print(network.Green, "#")

		if offset == 51 {
			fmt.Print(network.Reset, " Ok!")
		}
		fmt.Print("\033[A")
	case 2:
		fmt.Print("\033[B", "\033[B")
		if offset == 1 {
			tempalte = fmt.Sprintf("File №%d ", routineId)
			findCursorPos(1)
			fmt.Print(network.Reset, tempalte)
			offset += len(tempalte)
			tempLen = len(tempalte)
		}
		findCursorPos(offset)
		fmt.Print(network.Green, "#")

		if offset == 51 {
			fmt.Print(network.Reset, " Ok!")
		}
		fmt.Print("\033[A", "\033[A")
	}

	//findCursorPosNew(routineId+10, CurrentOffset+*TotalOffset)
	//fmt.Print(network.Green, "#")
	*TotalOffset += CurrentOffset + tempLen
}

func downloadVisualizationInternalNew(CurrentOffset int, TotalOffset *int, routineId int, routinesCnt int) {

	var offset int = CurrentOffset + *TotalOffset
	var tempalte string
	var tempLen int = 0

	for i := 0; i < routineId; i++ {
		fmt.Print("\033[B")
	}

	if offset == 1 {
		tempalte = fmt.Sprintf("File №%d ", routineId)
		findCursorPos(1)
		fmt.Print(network.Reset, tempalte)
		tempLen = len(tempalte)
		offset += tempLen
	}
	findCursorPos(offset)
	fmt.Print(network.Green, "#")

	if offset == 51 {
		fmt.Print(network.Reset, " Ok!")
	}

	for i := 0; i < routineId; i++ {
		fmt.Print("\033[A")
	}

	*TotalOffset += CurrentOffset + tempLen
}
