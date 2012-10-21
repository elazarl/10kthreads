package main

import (
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"net"
	"sync"
	"time"
)

func panicOnErr(msg string, err error) {
	if err!=nil {
		panic(msg+": "+err.Error())
	}
}

func main() {
	nthreads := flag.Int("n", -1, "how many threads will open connection concurrently")
	data := flag.String("data", "hello", "what will those threads write")
	host := flag.String("host", "localhost:1234", "all threads connect to host")
	flag.Parse()
	if *nthreads<=0 {
		fmt.Println("Must supply nthreads")
		flag.Usage()
		return
	}

	var step1, step2 sync.WaitGroup
	step1.Add(*nthreads)
	step2.Add(1)
	for i:=0; i<*nthreads; i++ {
		go func() {
			conn, err := net.Dial("tcp", *host)
			if err!=nil {
				fmt.Println("dial:", err)
				step1.Done()
				step2.Wait()
				return
			}
			//panicOnErr("dial", err)
			defer conn.Close()
			fmt.Fprint(conn, *data)
			step1.Done()
			step2.Wait()
			_, err = io.Copy(ioutil.Discard, conn)
			panicOnErr("copy", err)
		}()
		time.Sleep(time.Microsecond)
	}
	step1.Wait()
	step2.Done()
}

