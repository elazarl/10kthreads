# Is thread per connection scalable?

## Origin

A very experience colleague has told me once, that one can't use a thread
per-connection model for handling connections.

- "You must use some sort of `epoll` or `select`, it wouldn't work otherwise!
- "Why? What would happen if I use a thread per connection?"
- "It wouldn't work, why won't you just try it and let me know?"

So I tried it.

## Background

When a server waits for connection, it usually needs to handle more than one concurrent connection. In order to do that you can either use `select` like mechanism with asynchronous io, something like:

    active_conn = {}
    server = socket.socket(socket.AF_INET,
        socket.SOCK_STREAM)
    server.bind((host,port)) 
    server.listen(backlog) 
    while true:
        in, out, ex = select.select(active_conn+server,
            [],[])
        if server_has_new_conn(server, in):
            addClient(online_conn)
        else:
            handleConn(in, out, active_conn)

Whenever handleConn wishes to write or read from the socket, it'll do so asynchronously, and go back to the main loop.

Another option is to spawn a thread for each concurrent connection. Keep everything synchrounous, but spawn a thread for every new request.

    server = socket.socket(socket.AF_INET,
        socket.SOCK_STREAM)
    server.bind((host,port)) 
    server.listen(backlog)
    while true:
        client, addr = server.accept()
        thread.start_new_thread(handler, client)

The common wisdom about select vs threads is, that the `epoll` based techniques is faster than spawning a thread for each `request`. People far wiser than me has [dispelled this myth](http://www.mailinator.com/tymaPaulMultithreaded.pdf).

But an online PDF is not as convincing as running code

## Code
### Source

This repository contains three executables.

In the main directory is the `10kthreads` TCP echo server, which simply spawns a thread for every new request.

In `client` directory there's a Go (golang) async clients that opens `N` concurrent connections, and verify they all echo a short string back to the client.

In `ref_async_server` there's a simple Go (golang) echo server, relying on Go's internal dispatcher (which in turn is using asynchronous read and `epoll`).

### Execution

Note that in order to actually have 10K concurrent connections, you must set the `ulimit` accordingly. The easiest way to do that is to login as root, and use `ulimit -n`:

    $ sudo su -
    # ulimit -n 10100
    # ./bin/10kthreads >/tmp/server.out &
    [1] 16358
    # ./bin/client -n 10000
    Max conc: 10000
    total: 10000
    conc: 0
    # kill -SIGINT %1
    # cat /tmp/server.out
    EXITING
    resident 
    434176
    Max conc: 10000
    Total: 1000
    Now: 0
    [1]+  Done                    ./bin/10kthreads > /tmp/server

To change ulimit for a specific user see `/etc/security/limits.conf` or `/etc/launchd.conf` in Mac OS X.

*Max conc*, is the maximum concurrent sockets alive as recorded by the server or client, *total* is the total number of connections opened.

### Bugs and TODO

On high contention client sometimes stalls, a single thread is waiting at `epoll`, and the rest are holding a futex, probablly waiting on the `WaitGroup` to die.

The size of the stack for each thread should be determinable by command line arguments.