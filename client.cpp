/*
 * CLIENT source file - CS 456 A1
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
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

/*
 * Global variables
 */
char *  server_address;     // server address
int     n_port;             // negotiation port
int     req_code;           // request code
char *  msg;

int     r_port;

/*
 * Global constant
 */
const int MAX_BUFFER = 1024;    // max size for buffer

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
 * Function to negotiation using TCP sockets by client
 * modified from http://www.linuxhowtos.org/C_C++/socket.htm
 */
void negotiationClientTCP() {
    char buffer[MAX_BUFFER];

    // TCP client set up, similar to code provided from the link
    int tcp_sock_fd, temp;
    struct sockaddr_in tcp_serv_addr;
    struct hostent *tcp_server;
 
    tcp_sock_fd = socket(AF_INET, SOCK_STREAM, 0);      // SOCK_STREAM for TCP
    checkError(tcp_sock_fd == -1, "ERROR opening TCP socket");

    tcp_server = gethostbyname(server_address);
    checkError(tcp_server == NULL, "ERROR unkown host");

    // memset((char *) &tcp_serv_addr, 0, sizeof(tcp_serv_addr));
    // tcp_serv_addr.sin_family = AF_INET;
    // memcpy((char *) tcp_server->h_addr, (char *) &tcp_serv_addr.sin_addr.s_addr, tcp_server->h_length);
    // tcp_serv_addr.sin_port = htons(n_port);

    // // TCP connect by client
    // temp = connect(tcp_sock_fd, (struct sockaddr *) &tcp_serv_addr, sizeof(tcp_serv_addr));
    // checkError(temp == -1, "ERROR connecting");

    bzero((char *) &tcp_serv_addr, sizeof(tcp_serv_addr));
    tcp_serv_addr.sin_family = AF_INET;
    bcopy((char *)tcp_server->h_addr, 
         (char *)&tcp_serv_addr.sin_addr.s_addr,
         tcp_server->h_length);
    tcp_serv_addr.sin_port = htons(n_port);
    if (connect(tcp_sock_fd,(struct sockaddr *) &tcp_serv_addr,sizeof(tcp_serv_addr)) < 0) 
        checkError(true, "ERROR connecting");

    // clean up buffer and copy req_code to buffer
    memset(buffer, 0, MAX_BUFFER);
    sprintf(buffer, "%d", req_code);

    temp = write(tcp_sock_fd, buffer, strlen(buffer));
    checkError(temp == -1, "ERROR writing to socket");
    
    // clean up buffer
    memset(buffer, 0, MAX_BUFFER);

    temp = read(tcp_sock_fd, buffer, MAX_BUFFER - 1);
    checkError(temp == -1, "ERROR reading from socket");

    // get r_port from server
    r_port = atoi(buffer);

    // TCP disconnect by client
    close(tcp_sock_fd);
}

/*
 * Function to transaction using UDP sockets by client
 * modified from http://www.linuxhowtos.org/C_C++/socket.htm
 */
void transactionClientUDP() {
    char buffer[MAX_BUFFER];

    // UDP client set up, similar to code provided from the link
    int udp_sock_fd, temp;
    struct sockaddr_in udp_serv_addr, from;
    struct hostent *udp_server;
    unsigned int length = sizeof(struct sockaddr_in);

    udp_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);       // SOCK_DGRAM for UDP
    checkError(udp_sock_fd == -1, "ERROR opening UDP socket");

    udp_server = gethostbyname(server_address);
    checkError(udp_server == NULL, "ERROR unkown host");

    memset((char *) &udp_serv_addr, 0, sizeof(udp_serv_addr));
    udp_serv_addr.sin_family = AF_INET;
    memcpy((char *) udp_server->h_addr, (char *) &udp_serv_addr.sin_addr, udp_server->h_length);
    udp_serv_addr.sin_port = htons(r_port);

    // clean up buffer and copy msg to buffer
    memset(buffer, 0, MAX_BUFFER);
    sprintf(buffer, "%s", msg);

    temp = sendto(udp_sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *) &udp_serv_addr, length);
    checkError(temp == -1, "ERROR sendto socket");
    
    temp = recvfrom(udp_sock_fd, buffer, MAX_BUFFER, 0, (struct sockaddr *) &from, &length);
    checkError(temp == -1, "ERROR recvfrom socket");

    // print reversed string
    cout << buffer << endl;

    close(udp_sock_fd);
}


/*
 * Takes four arguments: server_address(int), n_port(int), req_code(int), msg(string)
 */
int main( int argc, char *argv[] ) {
    // Check number and formats of arguments
    checkError(argc != 5, "ERROR wrong number of arguments (expected 4).");
    checkError(!isInteger(argv[2]), "ERROR argument n_port should be integer");
    checkError(!isInteger(argv[3]), "ERROR argument req_code should be integer");
    
    // Assign input values
    server_address = argv[1];
    n_port = atoi(argv[2]);
    req_code = atoi(argv[3]);
    msg = argv[4];

    negotiationClientTCP();
    transactionClientUDP();

    return 0;
}
