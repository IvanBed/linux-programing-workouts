package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"path"

	"github.com/attilabuti/emlparser"
)

func main() {

	argsCount := len(os.Args)
	if argsCount != 2 {
		return
	}
	emlFilePath := os.Args[1]
	dir := emlFilePath[0 : len(emlFilePath)-len(path.Ext(emlFilePath))]

	reader, err := os.Open(emlFilePath)
	if err != nil {
		fmt.Println(err)
	}

	email, err := emlparser.Parse(reader)
	if err != nil {
		// handle error
	}

	/*for _, attachment := range(email.Attachments) {
	        fmt.Println(a.Filename)
	        fmt.Println(a.ContentType)
	        and read a.Data
	}*/

	for _, attachment := range email.Attachments {
		err = ioutil.WriteFile(path.Join(dir, attachment.Filename), attachment.Data, 0755)
		checkerr(err, "failed save attachment "+attachment.Filename)
	}

}
