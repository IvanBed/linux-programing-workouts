package main

import (
	"fmt"
	"net"
	"os"
	"strings"
)

func main() {

	var ip string
	var port string
	var opType string
	var srcPath string
	var destPath string
	var conn net.Conn

	if len(os.Args) < 5 {
		fmt.Println("Specify the address, port, operation type and the file path")
		fmt.Println("Example: 127.0.0.1 80 GET /path/to/file /path/to")
		return
	}

	ip = os.Args[1]
	port = os.Args[2]
	opType = os.Args[3]
	srcPath = os.Args[4]

	if len(os.Args) == 5 {
		destPath = "/tmp"
	} else {
		destPath = os.Args[5]
	}

	/*validateIp();
	  validatePort();
	  validateType();
	  validatePath();*/

	conn, err := makeConnection(ip, port)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer conn.Close()

	if strings.ToUpper(opType) == "GET" {

		err := getFile(conn, srcPath, destPath)
		if err != nil {

		}

	} else if strings.ToUpper(opType) == "POST" {

		err := postFile(conn, srcPath, destPath)
		if err != nil {

		}
	}
}
