package main

import (
	"fmt"
	"net"
	"os"
	"strings"
)

func makeConnection(ip string, port string) (net.Conn, error) {
	var address string

	address = ip + ":" + port

	conn, err := net.Dial("tcp", address)
	if err != nil {
		return nil, err
	}

	return conn, nil
}

func getFileInternal(conn net.Conn, srcPath string, destPath string) error {
	return nil
}

func postFile() error {
	return nil
}

func main() {

	var ip string
	var port string
	var methodType string

	var srcPath string
	var destPath string

	var conn net.Conn

	if len(os.Args) < 4 {
		fmt.Println("Specify the address, port and operation type")
		return
	}

	ip = os.Args[2]
	port = os.Args[3]

	conn, err := makeConnection(ip, port)

	if err != nil {

		return
	}

	if strings.ToUpper(methodType) == "GET" {
		if len(os.Args) < 5 {
			fmt.Println("Specify the path to the desired file")
		} else {
			srcPath = os.Args[4]
			if len(os.Args) == 5 {
				destPath = ""
			} else {
				destPath = os.Args[5]
			}
		}
		getFileInternal(conn, srcPath, destPath)

	} else if strings.ToUpper(methodType) == "PUT" {

	}

}
