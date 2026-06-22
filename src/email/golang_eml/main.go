package main

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/sg3des/eml"
)

func getRawDataFromEmlFile(emlFilePath string) ([]byte, error) {

	rawData, err := os.ReadFile(emlFilePath)
	if err != nil {
		fmt.Println(err)
	}

	return rawData, nil
}

func saveAttachments(message eml.Message, destPath string) {
	for _, attachment := range message.Attachments {
		destFullPath := filepath.Join(destPath, attachment.Filename)
		fmt.Printf("File name %s\n", destFullPath)
		if err := os.WriteFile(destFullPath, attachment.Data, 0775); err != nil {
			fmt.Println(err)
		}
	}
}

func main() {

	var message eml.Message
	argsCount := len(os.Args)
	if argsCount != 2 {
		return
	}
	emlFilePath := os.Args[1]

	emlRawData, err := getRawDataFromEmlFile(emlFilePath)
	if err != nil {
		fmt.Println("Can not get raw data")
		return
	}
	//fmt.Println(emlRawData)
	message, err = eml.Parse(emlRawData)
	saveAttachments(message, "/home/ivan/golang_eml")
}
