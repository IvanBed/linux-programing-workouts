package main

import (
	"bufio"
	"fmt"
	"os"

	//"path/filepath"
	"strconv"
	"strings"
)

func tryGetPPid(line string) (bool, string) {
	var ppid string = ""
	tokens := strings.Split(line, ":")
	if tokens[0] == "PPid" {
		ppid = strings.TrimSpace(tokens[1])
		return true, ppid
	} else {
		return false, ppid
	}
}

func getPPidFromStatusFile(statusFile *os.File) (bool, string) {
	scanner := bufio.NewScanner(statusFile)
	for scanner.Scan() {
		found, ppid := tryGetPPid(scanner.Text())
		if found {
			return true, ppid
		}
	}
	return false, ""
}

// pid передавать как строку
func getPidTree(pid string, pids *[]string) {

	if pid == "0" {
		return
	}
	// переделать через sprintf

	statusFilePath := "/proc/" + pid + "/status"
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

	ppids := make([]string, 1)
	ppids[0] = strconv.Itoa(target_pid)

	getPidTree(ppids[0], &ppids)

	for _, pid := range ppids {
		fmt.Println(pid)
	}

}
