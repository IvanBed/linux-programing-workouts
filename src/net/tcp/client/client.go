package main

import (
	"bufio"
	"errors"
	"fmt"
	"net"
	"os"
	"path/filepath"
	"strconv"
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

func getFileSize(filePath string) (int64, error) {

	fi, err := os.Stat(filePath)
	if err != nil {
		return -1, err
	}
	// get the size
	return fi.Size(), nil
}

func downloadFile(reader *bufio.Reader, filePath string) error {

	var writer *bufio.Writer
	var perms os.FileMode = 0666

	destFile, err := os.OpenFile(filePath, os.O_RDWR|os.O_CREATE, perms)
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer destFile.Close()

	writer = bufio.NewWriter(destFile)

	for {
		bytes, err := reader.ReadBytes('\n')
		if err != nil {
			if err.Error() != "EOF" {
				fmt.Printf("Ошибка чтения файла: %v\n", err)
			}
			break
		}
		for i := range bytes {
			writer.WriteByte(bytes[i])
		}
	}
	return nil
}

func getFile(conn net.Conn, srcPath string, destPath string) error {

	var fileName string
	var fileSizeStr string
	var fileSize int64
	var downloadedFileSize int64

	var request string = "GET:" + srcPath + "\n"

	var response string

	var reader *bufio.Reader

	reader = bufio.NewReader(conn)

	conn.Write([]byte(request))

	response, err := reader.ReadString('\n')
	if err != nil {
		fmt.Println("Error reading:", err.Error())
		return err
	}

	if response != "OK" {
		return errors.New("err Code")
	}

	fileName, err = reader.ReadString('\n')
	if err != nil {
		fmt.Println("Error reading:", err.Error())
		return err
	}

	fileSizeStr, err = reader.ReadString('\n')
	if err != nil {
		fmt.Println("Error reading:", err.Error())
		return err
	}

	destFilePath := filepath.Join(destPath, fileName)
	err = downloadFile(reader, destFilePath)
	if err != nil {
		fmt.Println("Error downloading:", err.Error())
		return err
	}

	fileSize, err = strconv.ParseInt(fileSizeStr, 10, 64)
	downloadedFileSize, err = getFileSize(destFilePath)
	if err != nil {
		fmt.Println(err)
	}

	fmt.Println(downloadedFileSize)
	fmt.Println(fileSize)

	return nil
}

func postFile(conn net.Conn, srcPath string, destPath string) error {
	return nil
}

func main() {

	var ip string
	var port string
	var opType string
	var srcPath string
	var destPath string
	var conn net.Conn

	if len(os.Args) < 5 {
		fmt.Println("Specify the address, port, operation type and the file path")
		return
	}

	ip = os.Args[1]
	port = os.Args[2]
	opType = os.Args[3]
	srcPath = os.Args[4]

	if len(os.Args) == 5 {
		destPath = "default"
	} else {
		destPath = os.Args[5]
	}

	/*validateIp();
	  validatePort();
	  validateType();
	  validatePath();*/

	conn, err := makeConnection(ip, port)
	if err != nil {
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
