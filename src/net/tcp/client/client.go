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

const Reset = "\033[0m"
const Red = "\033[31m"
const Green = "\033[32m"

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

func downloadVisualization(segmentSize int, currentSize int, linesCnt *int) {

	if *linesCnt == 0 || *linesCnt*segmentSize <= currentSize {
		fmt.Print(Green, "#")
		*linesCnt++
	}
}

func downloadFile(reader *bufio.Reader, filePath string, fileSize int64) error {

	var writer *bufio.Writer
	var perms os.FileMode = 0666

	destFile, err := os.OpenFile(filePath, os.O_CREATE|os.O_RDWR, perms)
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer destFile.Close()

	writer = bufio.NewWriter(destFile)
	recvData := make([]byte, 4096)

	segmentSize := int(fileSize / 50)
	currentSize := 0
	lineCnt := 0
	// Добавить timeout
	fmt.Println("Start downloading!")
	fmt.Print("Progress: ")
	for {
		bytes, err := reader.Read(recvData)
		if err != nil {
			if err != io.EOF {
				fmt.Println(Red, "Download error : ", err)
				fmt.Print(Reset)
				return err
			} else {
				//fmt.Println("End of the file")
				break
			}
		}
		currentSize += bytes
		for i := 0; i < bytes; i++ {
			writer.WriteByte(recvData[i])
		}
		writer.Flush()
		downloadVisualization(segmentSize, currentSize, &lineCnt)
	}
	fmt.Println(Reset, "\nDone!")
	fmt.Println("Bytes: ", currentSize)
	return nil
}

func makeDestFilePath(srcPath string, destPath string) (string, error) {

	fileName := filepath.Base(srcPath)
	filePath := filepath.Join(destPath, fileName)
	sameNameCnt := 0

	dirEntries, err := os.ReadDir(destPath)
	if err != nil {
		fmt.Println("Could not read directory")
		return "", err
	}
	fmt.Println("file name: ", fileName)
	for _, entry := range dirEntries {

		if len(entry.Name()) >= len(fileName) && strings.HasPrefix(fileName, entry.Name()[:len(fileName)]) {
			sameNameCnt++
			fmt.Println("true")
		}
	}
	if sameNameCnt > 0 {
		filePath += "_" + strconv.Itoa(sameNameCnt)
	}
	return filePath, nil
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

	conn.Write([]byte(request))

	response, err := reader.ReadString('\n')
	if err != nil {
		fmt.Println("Error reading:", err.Error())
		return err
	}
	if response != "OK\n" {
		fmt.Println(response)
		conn.Write([]byte(stop))
		return errors.New("err Code")
	} else {
		//fmt.Println(response)
		conn.Write([]byte(letsgo))
	}

	fileSizeStr, err = reader.ReadString('\n')
	if err != nil {
		fmt.Println("Error reading:", err.Error())
		return err
	}
	fileSizeStr = strings.TrimSpace(fileSizeStr[1:])
	fileSize, _ = strconv.ParseInt(fileSizeStr, 10, 64)

	/*fmt.Println("fileSizeStr:", fileSizeStr)
	fmt.Println("fileSize:", fileSize)
	fmt.Println("filename: ", filepath.Base(srcPath))*/

	destFilePath, err := makeDestFilePath(srcPath, destPath)
	if err != nil {
		return err
	}

	err = downloadFile(reader, destFilePath, fileSize)
	if err != nil {
		fmt.Println("Error downloading:", err.Error())
		return err
	}

	downloadedFileSize, err = getFileSize(destFilePath)
	if err != nil {
		fmt.Println("Could not get file size:", err.Error())
	}

	if downloadedFileSize != fileSize {

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
