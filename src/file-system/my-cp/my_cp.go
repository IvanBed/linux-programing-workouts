package main

import (
	"fmt"
	"os"
	"path/filepath"
)

const (
	NOTHING = iota
	RECURSIVE
)

func makeOutputPath(destPath string, srcPath string) string {
	fi, err := os.Stat(destPath)
	if err == nil && fi.Mode().IsDir() {
		if destPath[len(destPath)-1] != '/' {
			destPath = destPath + "/"
		}

		destPath = filepath.Dir(destPath) + "/" + filepath.Base(srcPath)
	}
	return destPath
}

func copyFileInternal() {}

func copyFile(destPath string, srcPath string) {

	var buff_size int = 4096
	buff := make([]byte, buff_size)

	fmt.Println("Open src file", srcPath)
	srcFile, err := os.Open(srcPath)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer srcFile.Close()
	fmt.Println("Open dest file ", destPath)
	destFile, err := os.OpenFile(destPath, os.O_RDWR|os.O_CREATE, 0644)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer destFile.Close()

	for {
		inputCount, err := srcFile.Read(buff)
		if err != nil {
			fmt.Println(err)
		}

		if inputCount <= 0 {
			break
		}

		outputCount, err := destFile.Write(buff)
		if err != nil {
			fmt.Println(err)
		}

		if inputCount != outputCount {
			break
		}
	}
	fmt.Println("COPY DONE")
}

func getFlag(mode string) uint8 {

	var flag uint8

	for i := 0; i < len(mode); i++ {

		if mode[i] == 'r' || mode[i] == 'R' {
			flag |= RECURSIVE
		}
	}
	return flag
}

// rewrite with stringBuffer
func recursiveCopy(destDir string, srcDir string) {
	dirEntries, err := os.ReadDir(srcDir)

	if err != nil {
		//errors.append()
	}
	for _, entry := range dirEntries {
		entryFullPath := srcDir + "/" + entry.Name()

		if entry.IsDir() {
			recursiveCopy(destDir, entryFullPath)
		} else {
			os.MkdirAll(destDir+srcDir, 0755)
			copyFile(destDir+entryFullPath, entryFullPath)
		}
	}
}

func main() {

	var srcPath []string
	var destPath string
	var flag uint8
	argsCount := len(os.Args)

	if argsCount < 3 {
		return
	} else if argsCount == 3 {

		srcPath = append(srcPath, os.Args[1])
		destPath = os.Args[2]
	} else if argsCount >= 4 {
		flag = getFlag(os.Args[1])

		for i := 2; i < argsCount-1; i++ {
			srcPath = append(srcPath, os.Args[i])
		}
		destPath = os.Args[argsCount-1]
	}

	if len(srcPath) > 1 {
		destPathInfo, err := os.Stat(destPath)
		if err != nil {
			fmt.Println(err)
			return
		}
		if !destPathInfo.IsDir() {
			fmt.Printf("my_cp: target %s: Not a directory\n", destPath)
			return
		}
	}

	currentDestPath := destPath
	for _, srcFile := range srcPath {

		srcPathInfo, err := os.Stat(srcFile)
		if err != nil {
			fmt.Println(err)
			return
		}

		if flag == RECURSIVE {

			if !srcPathInfo.IsDir() {
				currentDestPath = makeOutputPath(destPath, srcFile)
				fmt.Println(currentDestPath)
				fmt.Println(srcFile)
				copyFile(currentDestPath, srcFile)
			} else {
				if destPath[len(destPath)-1] != '/' {
					currentDestPath = destPath + "/"
				}
				recursiveCopy(currentDestPath, srcFile)
			}

		} else {
			if !srcPathInfo.IsDir() {
				currentDestPath = makeOutputPath(destPath, srcFile)
				copyFile(currentDestPath, srcFile)
			}
		}
	}
}
