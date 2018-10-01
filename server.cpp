/*
 * SERVER source file - CS 456 A1
 *
 * ID: 20610147
 * Name: Yufei Yang
 *
 * Acknowledgement: I am the sole contributor to this assignment
 * Help from http://www.linuxhowtos.org/C_C++/socket.htm
 */

#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <algorithm>

using namespace std;

/*
 * Global variables
 */
int     req_code;           // request code

int     n_port;
int     r_port;

/*
 * Global constant
 */
const int MAX_BUFFER    = 1024;    // max size for buffer
const int INITIAL_PORT  = 1024;  // initial port number

/*
 * Function to put out a message on error exits (condition true).
 */
void checkError(bool condition, string str) {
    if (condition) {
        cerr << str << endl;
        exit(0);
    }
}

/*
 * Function to check if an argument is integer
 */
bool isInteger(char * arg) {
    char *end;
    strtol(arg, &end, 10);
    return (*end == '\0');
}

/*
 * Function for both negotiation using TCP and transaction using UDP by server, they
 *   are not separted because we want to determine r_port before finish all commnications
 *   by TCP, however, comments will clearly label them
 * modified from http://www.linuxhowtos.org/C_C++/socket.htm
 */
void TCPandUDP() {
    char buffer[MAX_BUFFER];
    int temp;

    // TCP server set up, similar to code provided from the link
    int tcp_sock_fd, new_tcp_sock_fd;
    struct sockaddr_in tcp_serv_addr, tcp_cli_addr;
    socklen_t clilen = sizeof(tcp_cli_addr);

    tcp_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    checkError(tcp_sock_fd == -1, "ERROR opening TCP socket");

    memset((char *) &tcp_serv_addr, 0, sizeof(tcp_serv_addr));
    tcp_serv_addr.sin_family = AF_INET;
    tcp_serv_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_serv_addr.sin_port = htons(n_port);

    // Logic to find available port for n_port
    while (::bind(tcp_sock_fd, (struct sockaddr *) &tcp_serv_addr, sizeof(tcp_serv_addr)) == -1) {
        n_port++;
        tcp_serv_addr.sin_port = htons(n_port);
    }

    // Output negotiation port number
    printf("SERVER_PORT=%d\n", n_port);

    // Continur listening for clients
    while (true) {
        listen(tcp_sock_fd, 5);
        new_tcp_sock_fd = accept(tcp_sock_fd, (struct sockaddr *) &tcp_cli_addr, &clilen);
        checkError(new_tcp_sock_fd == -1, "ERROR on accept TCP connection");

        // cleab up buffer
        memset(buffer, 0, MAX_BUFFER);

        temp = read(new_tcp_sock_fd, buffer, MAX_BUFFER - 1);
        checkError(temp == -1, "ERROR reading from socket");

        // check if req_code from client match
        checkError(req_code != atoi(buffer), "ERROR client failed to match request code");

        /*
         * Start of initializing UDP socket
         */

        // UDP server set up, similar to code provided from the link
        int udp_sock_fd;
        struct sockaddr_in udp_serv_addr, from;
        socklen_t fromlen = sizeof(struct sockaddr_in);

        udp_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
        checkError(udp_sock_fd == -1, "ERROR opening UDP socket");

        memset(&udp_serv_addr, 0, sizeof(udp_serv_addr));
        udp_serv_addr.sin_family = AF_INET;
        udp_serv_addr.sin_addr.s_addr = INADDR_ANY;

        // initialize r_port
        r_port = n_port + 1;

        udp_serv_addr.sin_port = htons(r_port);

        // Logic to find available port for r_port
        while (::bind(udp_sock_fd, (struct sockaddr *) &udp_serv_addr, sizeof(udp_serv_addr)) == -1) {
            r_port++;
            udp_serv_addr.sin_port = htons(r_port);
        }

        /*
         * End of initializing UDP socket
         */

        // Clean up buffer and copy r_port to buffer
        memset(buffer, 0, MAX_BUFFER);
        sprintf(buffer, "%d", r_port);

        // send r_port to client
        temp = write(new_tcp_sock_fd, buffer, MAX_BUFFER - 1);
        checkError(temp == -1, "ERROR writing to socket");

        /*
         * Start of UDP transaction
         */

        // clean up buffer        
        memset(buffer, 0, MAX_BUFFER);

        // get string from client
        temp = recvfrom(udp_sock_fd, buffer, MAX_BUFFER, 0, (struct sockaddr *) &from, &fromlen);
        checkError(temp == -1, "ERROR recvfrom socket");

        // reverse the string from client
        string str = buffer;
        reverse(str.begin(), str.end());
        sprintf(buffer,"%s", str.c_str());

        // send reversed string to client
        temp = sendto(udp_sock_fd, buffer, MAX_BUFFER, 0, (struct sockaddr *) &from,fromlen);
        checkError(temp == -1, "ERROR sendto socket");
    
        /*
         * End of UDP transaction
         */
    }

    close(new_tcp_sock_fd);
    close(tcp_sock_fd);
}

/*
 * Takes one argument: req_code(int)
 */
int main ( int argc, char *argv[] ) {
    // Check number and formats of arguments
    checkError(argc != 2, "ERROR wrong number of arguments (expected 1).");
    checkError(!isInteger(argv[1]), "ERROR argument req_code should be integer");

    // Assign input value
    req_code = atoi(argv[1]);

    // intialize negotiation port number
    n_port = INITIAL_PORT;

    TCPandUDP();

    return 0;
}
