package netutils

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"net"
	"os"
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
