package filesystem

import (
	"fmt"
	"os"
	"path/filepath"
	"strconv"
	"strings"
)

func getFileSize(filePath string) (int64, error) {

	fi, err := os.Stat(filePath)
	if err != nil {
		return -1, err
	}
	// get the size
	return fi.Size(), nil
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
