#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>

#define BUFFER_SIZE 516


int main(int argc, char *argv[]) {
	
	char receivedFile[BUFFER_SIZE];
	
    if (argc != 4) {
        write(STDOUT_FILENO, "Usage : getftp <serveur_ip> <port> <filename>\n", 46);
        exit(EXIT_FAILURE);
    }
    
    char *host = argv[1];
    char *port = argv[2];
    char *file = argv[3];
    
    
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */
    
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
	
	//create file
	char path[] = "/home/ensea/TPsynthese2/";
	strcpy(&path[24], file);
	
	int dlfd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU, S_IRWXG, S_IRWXO);
	if (dlfd == -1)
	{
		perror("error in new file");
		exit(EXIT_FAILURE);
	}
		
	// RRQ request for reading a file 
	char rrq[9+strlen(file)];

	rrq[0] = 0x00; 
	rrq[1] = 0x01;	
	strcpy(&rrq[2], file);
	rrq[2+strlen(file)] = 0x00;
	strcpy(&rrq[3+strlen(file)], "octet");
	rrq[8+strlen(file)] = 0x00;
	
	// send rrq request 
	if (sendto(sockfd, &rrq, 9+strlen(file), 0, rp->ai_addr, rp->ai_addrlen) != 9+strlen(file))
	{
		perror("error in writing request");
		exit(EXIT_FAILURE);
	}
	
	int receivedBytes;
	while((receivedBytes = recvfrom(sockfd, &receivedFile, BUFFER_SIZE, 0, rp->ai_addr, &rp->ai_addrlen)) > 0)
	{
		// receive file
		if (receivedBytes == -1)
		{ 
			perror("error in reading");
			exit(EXIT_FAILURE);
		}

		//send ack
		char ack[4] = {0x00, 0x04, receivedFile[2], receivedFile[3]};
		if (sendto(sockfd, &ack, 4, 0, rp->ai_addr, rp->ai_addrlen) != 4)
		{
			perror("error in writing request");
			exit(EXIT_FAILURE);
		}
	
		// parse and download file
		if (receivedBytes <= 4)
			break;
		
		//write in file 
		if(write(dlfd, &receivedFile[4], receivedBytes-4) == -1) 
		{
			perror("error in writing content");
			exit(EXIT_FAILURE);
		}
		
		if (receivedBytes < 516)
			break;
	}
	exit(EXIT_SUCCESS);

}
