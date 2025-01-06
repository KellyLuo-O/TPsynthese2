#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>

/* int socketConnect(char *host, char *port)
 * 
 * return socket file descriptor 
 * 
 */
int socketConnect(char *host, char *port) {

    struct addrinfo hints, *result;
   
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */
    
    // getaddrinfo
    int sockfd = getaddrinfo(host, port, &hints, &result);
    if (sockfd != 0 )
    {
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(sockfd));
		exit(EXIT_FAILURE);
	}
	
	/* getaddrinfo returns a list result of structure addrinfo
	 * Try socket each address until successuful */
	struct addrinfo *rp;
	for (rp = result; rp != NULL; rp = rp->ai_next) 
	{
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1)
			continue;
		else 
			break;	
	}
	if (rp == NULL)
	{
	   fprintf(stderr, "Could not connect\n");
	   exit(EXIT_FAILURE);
	}
	
	freeaddrinfo(result);
	
	return sockfd;
}

int main(int argc, char *argv[]) {
	
    if (argc != 4) {
        write(STDOUT_FILENO, "Usage : getftp <serveur_ip> <port> <filename>\n", 46);
        exit(EXIT_FAILURE);
    }
    
    char *host = argv[1];
    char *port = argv[2];
    char *file = argv[3];
    
    int sockfd = socketConnect(host, port);
    
    // RRQ request for reading a file 
	char rrq[12+strlen(file)];

	rrq[0] = 0x00; 
	rrq[1] = 0x01;	
	strcpy(&rrq[2], file);
	rrq[2+strlen(file)] = 0x00;
	strcpy(&rrq[3+strlen(file)], "netascii");
	rrq[11+strlen(file)] = 0x00;

}
