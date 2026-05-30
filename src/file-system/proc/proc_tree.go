package main

import (
	"bufio"
	"fmt"
	"os"
	"path/filepath"
	"strconv"
	"strings"
)

func tryGetPPid(line string) (bool, int) {
	tokens := strings.Split(line, ":")

	if tokens[0] == "PPid" {
		ppid, err := strconv.Atoi(strings.TrimSpace(tokens[1]))
		if err != nil {
			fmt.Println(err)
			return false, -1
		}
		return true, ppid
	} else {
		return false, -1
	}
}

func getPPidFromStatusFile(statusFile *os.File) (bool, int) {
	scanner := bufio.NewScanner(statusFile)
	for scanner.Scan() {
		found, ppid := tryGetPPid(scanner.Text())
		if found {
			return true, ppid
		}
	}
	return false, -1
}

// pid передавать как строку
func getPidTree(pid int, pids *[]int) {

	if pid == 0 {
		return
	}
	// переделать через sprintf

	procFilePath := filepath.Join("/proc", strconv.Itoa(pid))
	statusFilePath := filepath.Join(procFilePath, "status")

	procStatusFile, err := os.Open(statusFilePath)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer procStatusFile.Close()

	found, ppid := getPPidFromStatusFile(procStatusFile)

	if found {
		*pids = append(*pids, ppid)
		getPidTree(ppid, pids)
	}
}

func main() {
	var target_pid int
	var argsCount int
	var argError error
	argsCount = len(os.Args)

	if argsCount < 2 {
		target_pid = os.Getpid()
	} else {
		target_pid, argError = strconv.Atoi(os.Args[1])
		if argError != nil {
			fmt.Println(argError)
			return
		}
	}

	ppids := make([]int, 1)
	ppids[0] = target_pid

	getPidTree(target_pid, &ppids)

	for _, pid := range ppids {
		fmt.Println(pid)
	}

}
