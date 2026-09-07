package main

import (
	"bufio"
	"errors"
	"fmt"
	"io"
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

	fmt.Println("downloadFile")

	destFile, err := os.OpenFile(filePath, os.O_CREATE|os.O_RDWR, perms)
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer destFile.Close()

	writer = bufio.NewWriter(destFile)
	recvData := make([]byte, 4096)
	// Добавить timeout
	for {
		bytes, err := reader.Read(recvData)
		if err != nil {
			if err != io.EOF {
				fmt.Println("Read error : ", err)
			} else {
				fmt.Println("End of the file")
			}
			break
		}
		fmt.Println("read: ", bytes)
		for i := 0; i < bytes; i++ {
			writer.WriteByte(recvData[i])
		}
		writer.Flush()
	}
	return nil
}

func getFile(conn net.Conn, srcPath string, destPath string) error {

	var fileSizeStr string
	var fileSize int64
	var downloadedFileSize int64

	var request string = "GET:" + srcPath + "\n"

	var stop string = "STOP\n"
	var letsgo string = "GO\n"

	var response string

	var reader *bufio.Reader

	reader = bufio.NewReader(conn)

	fmt.Println("getFile")

	conn.Write([]byte(request))

	response, err := reader.ReadString('\n')
	if err != nil {
		fmt.Println("Error reading:", err.Error())
		return err
	}
	fmt.Println("response")
	if response != "OK\n" {
		fmt.Println(response)
		conn.Write([]byte(stop))
		return errors.New("err Code")
	} else {
		fmt.Println(response)
		conn.Write([]byte(letsgo))
	}

	fileSizeStr, err = reader.ReadString('\n')
	if err != nil {
		fmt.Println("Error reading:", err.Error())
		return err
	}

	fmt.Println("fileSizeStr: ", fileSizeStr)
	fmt.Println("filename: ", filepath.Base(srcPath))

	destFilePath := filepath.Join(destPath, filepath.Base(srcPath))
	err = downloadFile(reader, destFilePath)
	if err != nil {
		fmt.Println("Error downloading:", err.Error())
		return err
	}
	fmt.Println("Download end")
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
