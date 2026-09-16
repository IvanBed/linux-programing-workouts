package network

import (
	"bufio"
	"client/internal/filesystem"

	"sync"

	"errors"
	"fmt"
	"io"
	"net"
	"os"
	"strconv"
	"strings"
)

type VisualizationArgs struct {
	RoutineId   int
	SegmentSize int
	CurrentSize int
	LinesCnt    *int
	EndFlag     bool
}

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

func downloadFile(routineId int, reader *bufio.Reader, filePath string, fileSize int64, visualizationChannel chan VisualizationArgs) error {

	var writer *bufio.Writer
	var perms os.FileMode = 0666
	var visualization VisualizationArgs

	//fmt.Println(filePath)
	destFile, err := os.OpenFile(filePath, os.O_CREATE|os.O_RDWR, perms)
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer destFile.Close()

	writer = bufio.NewWriter(destFile)
	recvData := make([]byte, 4096)

	visualization.RoutineId = routineId
	visualization.SegmentSize = int(fileSize / 50)
	visualization.LinesCnt = new(int)
	visualization.EndFlag = false

	// Добавить timeout
	//fmt.Println("Start downloading!")
	//fmt.Print("Progress: ")
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
		visualization.CurrentSize += bytes
		for i := 0; i < bytes; i++ {
			writer.WriteByte(recvData[i])
		}
		writer.Flush()
		//fmt.Print("Channel start")
		visualizationChannel <- visualization
		//fmt.Print("Channel end")
	}
	visualization.EndFlag = true
	visualizationChannel <- visualization

	//fmt.Println(Reset, "\nDone!")
	//fmt.Println("Bytes: ", currentSize)
	return nil
}

func GetFile(routineId int, ip string, port string, srcPath string, destPath string, visualizationChannel chan VisualizationArgs, wg *sync.WaitGroup) error {

	var fileSizeStr string
	var fileSize int64
	var downloadedFileSize int64
	var conn net.Conn

	var request string = "GET:" + srcPath + "\n"
	var stop string = "STOP\n"
	var letsgo string = "GO\n"

	var response string
	var reader *bufio.Reader

	defer wg.Done()

	conn, err := makeConnection(ip, port)
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer conn.Close()

	reader = bufio.NewReader(conn)

	conn.Write([]byte(request))

	response, err = reader.ReadString('\n')
	if err != nil {
		fmt.Println("Error reading:", err.Error())
		return err
	}
	if response != "OK\n" {
		//fmt.Println(response)
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

	destFilePath, err := filesystem.MakeDestFilePath(srcPath, destPath)
	if err != nil {
		return err
	}

	err = downloadFile(routineId, reader, destFilePath, fileSize, visualizationChannel)
	if err != nil {
		fmt.Println("Error downloading:", err.Error())
		return err
	}

	downloadedFileSize, err = filesystem.GetFileSize(destFilePath)
	if err != nil {
		fmt.Println("Could not get file size:", err.Error())
	}

	if downloadedFileSize != fileSize {

	}
	/*fmt.Println(downloadedFileSize)
	fmt.Println(fileSize)*/

	return nil
}

func PostFile(conn net.Conn, srcPath string, destPath string) error {
	return nil
}
