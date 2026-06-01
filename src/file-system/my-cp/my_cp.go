package main

import (
	"errors"
	"fmt"
	"os"
	"path/filepath"
)

type Mod uint8

const (
	NOTHING   = 0
	RECURSIVE = 1
	NOCLOBBER = 2
	VERBOSE   = 4
)

func makeDestPath(destPath string, srcPath string) string {
	fi, err := os.Stat(destPath)
	if err == nil && fi.Mode().IsDir() {
		destPath = filepath.Join(destPath, filepath.Base(srcPath))
	}
	if err != nil {
		fmt.Println(err)
	}
	return destPath
}

func fileExists(path string) bool {

	if _, err := os.Stat(path); errors.Is(err, os.ErrNotExist) {
		return false
	} else {
		return true
	}
}

func copyFile(destPath string, srcPath string, mod uint8, perms os.FileMode) {

	if checkMod(mod, NOCLOBBER) && fileExists(destPath) {
		fmt.Printf("%s file exists, skipped\n", destPath)
		return
	}

	var buff_size int = 4096
	buff := make([]byte, buff_size)

	destFile, err := os.OpenFile(destPath, os.O_RDWR|os.O_CREATE, perms)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer destFile.Close()

	srcFile, err := os.Open(srcPath)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer srcFile.Close()

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

func getMod(modStr string) uint8 {

	var mod uint8

	for i := 0; i < len(modStr); i++ {

		if modStr[i] == 'r' || modStr[i] == 'R' {
			mod |= RECURSIVE
		} else if modStr[i] == 'n' || modStr[i] == 'N' {
			mod |= NOCLOBBER
		} else if modStr[i] == 'v' || modStr[i] == 'V' {
			mod |= VERBOSE
		}
	}
	return mod
}

func checkMod(mod uint8, flag uint8) bool {
	var mask uint8
	var iters uint8
	mask = 1
	iters = 0
	for flag&mask != 1 {
		flag >>= 1
		iters++
	}
	return (mod>>iters)&mask == 1
}

func recursiveCopy(destDir string, srcDir string, mod uint8, perms os.FileMode, errors []error) {
	dirEntries, err := os.ReadDir(srcDir)

	if err != nil {
		errors = append(errors, err)
	}
	for _, entry := range dirEntries {
		entryFullPath := filepath.Join(srcDir, entry.Name())
		destFulPath := filepath.Join(destDir, entry.Name())

		os.MkdirAll(destDir, 0755)
		if entry.IsDir() {
			fmt.Println(destFulPath)
			recursiveCopy(destFulPath, entryFullPath, mod, perms, errors)
		} else {
			copyFile(destFulPath, entryFullPath, mod, perms)
		}
	}
}

func main() {

	var srcPath []string
	var destPath string
	var mod uint8
	var argsCount int
	var errors []error
	var currentDestPath string
	var perms os.FileMode
	argsCount = len(os.Args)

	if argsCount < 3 {
		return
	} else if argsCount == 3 {
		srcPath = append(srcPath, os.Args[1])
		destPath = os.Args[2]
	} else if argsCount >= 4 {
		mod = getMod(os.Args[1])

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
	errors = make([]error, 0)

	for _, srcFile := range srcPath {
		srcPathInfo, err := os.Stat(srcFile)
		if err != nil {
			fmt.Println(err)
			return
		}
		perms = srcPathInfo.Mode().Perm()
		currentDestPath = makeDestPath(destPath, srcFile)
		if !srcPathInfo.IsDir() {
			copyFile(currentDestPath, srcFile, mod, srcPathInfo.Mode().Perm())
		} else {
			if checkMod(mod, RECURSIVE) {
				recursiveCopy(currentDestPath, srcFile, mod, perms, errors)
			} else {
				fmt.Printf("cp: -r not specified; omitting directory %s\n", srcFile)
			}
		}
	}

	for _, err := range errors {
		fmt.Println(err)
	}
}
