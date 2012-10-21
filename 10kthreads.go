package main

import (
	"fmt"
	"io/ioutil"
	"net"
	"os"
	"os/signal"
	"sync/atomic"
)

func main() {
	l, err := net.Listen("tcp", "localhost:1234")
	if err != nil {
		fmt.Println("listen:", err)
		return
	}
	s := make(chan os.Signal)
	closed := int32(0)
	signal.Notify(s, os.Interrupt)
	go func() {
		<-s
		atomic.StoreInt32(&closed, 1)
		fmt.Println("closing")
		l.Close()
	}()
	totalConc := int32(0)
	maxConc := int32(0)
	conc := int32(0)
	for {
		c, err := l.Accept()
		if closed != 0 {
			fmt.Println("closed")
			break
		}
		if err != nil {
			fmt.Println("accept:", err)
			return
		}
		go func(c net.Conn) {
			defer c.Close()
			atomic.AddInt32(&totalConc, 1)
			curr := atomic.AddInt32(&conc, 1)
			for curr > maxConc && !atomic.CompareAndSwapInt32(&maxConc, maxConc, curr) {
			}
			defer atomic.AddInt32(&conc, -1)
			b, err := ioutil.ReadAll(c)
			if err != nil {
				fmt.Println("read:", err)
				return
			}
			if _, err := c.Write(b); err != nil {
				fmt.Println("write:", err)
				return
			}
		}(c)
	}
	fmt.Println("Done")
	fmt.Println("Total connections:", totalConc)
	fmt.Println("Max concurrent connections:", maxConc)
	fmt.Println("Current connections", conc)
}
