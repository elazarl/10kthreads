package main

import (
	"bytes"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"sync"
	"sync/atomic"
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

	var step1, step2, step3 sync.WaitGroup
	step1.Add(*nthreads)
	step2.Add(1)
	step3.Add(*nthreads)

	var conc int32 = 0
	var maxConc int32 = 0
	var total int32 = 0
	for i:=0; i<*nthreads; i++ {
		go func() {
			conn, err := net.Dial("tcp", *host)
			if err!=nil {
				fmt.Println("dial:", err)
				step1.Done()
				step2.Wait()
				step3.Done()
				return
			}
			c := atomic.AddInt32(&conc, 1)
			atomic.AddInt32(&total, 1)
			for c>maxConc && !atomic.CompareAndSwapInt32(&maxConc, maxConc, c) {
			}
			defer step3.Done()
			defer atomic.AddInt32(&conc, -1)
			defer conn.Close()
			fmt.Fprint(conn, *data)
			step1.Done()
			step2.Wait()
			out, err := ioutil.ReadAll(conn)
			panicOnErr("copy", err)
			if bytes.Compare(out, []byte(*data))!=0 {
				log.Fatalln("This is not an echo server: "+string(out))
			}
		}()
		time.Sleep(time.Microsecond)
	}
	step1.Wait()
	step2.Done()
	step3.Wait()
	fmt.Println("Max conc:", maxConc)
	fmt.Println("total:", total)
	fmt.Println("conc:", conc)
}

