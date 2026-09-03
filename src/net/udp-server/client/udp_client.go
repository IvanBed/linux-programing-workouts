package main

import (
        "bufio"
        "fmt"
        "net"
        "os"
        "strings"
)

func udpConnect(ip string, port string) error {

        addr, err := net.ResolveUDPAddr("udp", ip+":"+port)
        if err != nil {
                fmt.Println("Error resolving address:", err)
                return err
        }

        conn, err := net.DialUDP("udp", nil, addr)
        if err != nil {
                fmt.Println("Error creating socket:", err)
                return err
        }
        defer conn.Close()

        for {
                reader := bufio.NewReader(os.Stdin)
                fmt.Print("Enter text: ")
                text, _ := reader.ReadString('\n')
                if strings.TrimRight(text, "\n") == "STOP" {
                        break
                }
                if _, err := conn.Write([]byte(text)); err != nil {
                        fmt.Println("Error sending datagram:", err)
                        return err
                }
                fmt.Println("Recv a resp from server!")
		buf := make([]byte, len(text))
		if _, err := conn.Read(buf); err != nil {
			fmt.Println("Error reading datagram:", err)
			return err
		}

		fmt.Println("Received from server:", string(buf))
        }

        return nil
}

func main() {

        args := os.Args[1:]

        if len(args) < 2 {
                fmt.Println("Specify the server IP and port");
                return
        }
        udpConnect(args[0], args[1])
}

