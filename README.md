# CS 456 A1 - Socket Programming

> Yufei Yang
> 20610147

## Compile and Run

- The program is written in `c++14`, which is specified in the `Makefile`.

### Instructions

1. Compile the program
```
$ make
```

2. Run the server, which takes a integer as request code
```
$ ./server.sh <req_code>
```

3. Run the client, which takes four arguments, the first is a string of host machine's name, the second is negotiation port number printed by the server, the thrid is the same request code you input for the server, the last is a string with format `'some string'`
```
./client.sh <server_address> <n_port> <req_code> <message>
```

## Tested Machines

### Test 1

- client in `ubuntu1604-002.student.cs.uwaterloo.ca`
- server in `ubuntu1604-006.student.cs.uwaterloo.ca`

### Test 2

- both client and server in `ubuntu1604-002.student.cs.uwaterloo.ca`
