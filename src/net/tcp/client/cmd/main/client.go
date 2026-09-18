package main

import (
	"client/internal/network"
	"client/internal/visualization"
	"fmt"
	"os"
	"strings"
	"sync"
	_ "sync"
)

func startRoutins(opType string, ip string, port string, srcPathes string, destPath string) {
	var pathTokens []string
	var wg sync.WaitGroup
	var visualizationChannel chan network.VisualizationArgs

	pathTokens = strings.Split(srcPathes, ":")
	visualizationChannel = make(chan network.VisualizationArgs, 0)

	go visualization.DownloadVisualization(visualizationChannel, len(pathTokens))

	for routineId, path := range pathTokens {
		if strings.ToUpper(opType) == "GET" {
			wg.Add(1)
			fmt.Printf("routineId %d, path%s\n", routineId, path)
			go network.GetFile(routineId, ip, port, strings.TrimSpace(path), destPath, visualizationChannel, &wg)

		} else if strings.ToUpper(opType) == "POST" {

		}
	}
	wg.Wait()
	close(visualizationChannel)
	fmt.Print(fmt.Sprintf("\033[%dG", 1))
	for i := 0; i < len(pathTokens); i++ {
		fmt.Print("\033[B")
	}
}

func main() {

	var ip string
	var port string
	var opType string
	var srcPathes string
	var destPath string

	if len(os.Args) < 5 {
		fmt.Println("Specify the address, port, operation type and the file path")
		fmt.Println("Example: 127.0.0.1 80 GET /path/to/files /path/to")
		return
	}

	ip = os.Args[1]
	port = os.Args[2]
	opType = os.Args[3]
	srcPathes = os.Args[4]

	if len(os.Args) == 5 {
		destPath = "/tmp"
	} else {
		destPath = os.Args[5]
	}
	fmt.Println(srcPathes)
	/*validateIp();
	  validatePort();
	  validateType();
	  validatePath();*/

	startRoutins(opType, ip, port, srcPathes, destPath)
	/*conn, err := network.MakeConnection(ip, port)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer conn.Close()

	go visualization.Test()

	if strings.ToUpper(opType) == "GET" {

		err := network.GetFile(conn, srcPath, destPath)
		if err != nil {

		}

	} else if strings.ToUpper(opType) == "POST" {

		err := network.PostFile(conn, srcPath, destPath)
		if err != nil {

		}
	}*/
}
