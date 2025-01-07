#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>


int main(int argc, char *argv[]) {
	
    if (argc != 4) {
        write(STDOUT_FILENO, "Usage : getftp <serveur_ip> <port> <filename>\n", 46);
        exit(EXIT_FAILURE);
    }
    
    char *host = argv[1];
    char *port = argv[2];
    char *file = argv[3];
    
    char receivedACK[4];
    
    //--------------------------------- open socket for conection 
     
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
	
	// -------------------------- open and read file first
	
	// prepare path and buffer pointer
	
	char path[] = "/home/ensea/TPsynthese2/";
	strcpy(&path[24], file);
	
	// file status info 
	struct stat sb;
	if (stat(path, &sb) == -1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}
	
	// open file 
	int filefd = open(path, O_RDONLY);
	if (filefd == -1)
	{
		perror("error in opening file");
		exit(EXIT_FAILURE);
	}

	char fileData[sb.st_size];
	printf("ici dfile zize n : %ld\n", sb.st_size);
	// read file 
	if (read(filefd, &fileData, sb.st_size) == -1)
	{
		perror("error in reading file");
		exit(EXIT_FAILURE);
	}
	
	// -------------------- WRQ request for reading a file 
	char wrq[9+strlen(file)];

	wrq[0] = 0x00; 
	wrq[1] = 0x02;	
	strcpy(&wrq[2], file);
	wrq[2+strlen(file)] = 0x00;
	strcpy(&wrq[3+strlen(file)], "octet");
	wrq[8+strlen(file)] = 0x00;

	// -------------------- send wrq request 
	while(1)
	{
		
		if (sendto(sockfd, &wrq, 9+strlen(file), 0, rp->ai_addr, rp->ai_addrlen) != 9+strlen(file))
		{
			perror("error in sending request");
			exit(EXIT_FAILURE);
		}
		
		int receivedBytes = recvfrom(sockfd, receivedACK, 4, 0, rp->ai_addr, &rp->ai_addrlen);
		if (receivedBytes == -1)
		{ 
			perror("error in reading");
			exit(EXIT_FAILURE);
		}
		
		if(receivedACK[0] == 0x00 && receivedACK[1] == 0x04 &&			//received ack first two bytes = 0x0004
			receivedACK[2] == 0x00 && receivedACK[3] == 0x00)			//received ack last two bytes = block number
			break;
	}	

	
	for( int dataIndex = 0; dataIndex < strlen(fileData); dataIndex += 512 )
	{
		// ---------------- parse data 
		int blockNumber = (dataIndex/512) + 1;
		//int dataLen = strlen(fileData) - dataIndex < 512 ? strlen(fileData) - dataIndex : 512;
		char dataFrame[512];
		memcpy(dataFrame, &fileData[dataIndex], 512);
		
		
		
		int blockLen = strlen(dataFrame)+4-1;
		char parsedData[blockLen];
		
		parsedData[0] = 0x00; parsedData[1] = 0x03; 					// Opcode = 0x0003
		parsedData[2] = blockNumber>>8; parsedData[3] = blockNumber; 	// Block number
		strcpy(&parsedData[4], dataFrame);
		
		// ----------------- send data block
		while(1)
		{
			if (sendto(sockfd, parsedData, blockLen, 0, rp->ai_addr, rp->ai_addrlen) != blockLen)
			{
				perror("error in sending file data");
				exit(EXIT_FAILURE);
			}
			
			int receivedBytes = recvfrom(sockfd, receivedACK, 4, 0, rp->ai_addr, &rp->ai_addrlen);
			if (receivedBytes == -1)
			{ 
				perror("error in reading");
				exit(EXIT_FAILURE);
			}
			
			if(receivedACK[0] == 0x00 && receivedACK[1] == 0x04 &&						//received ack first two bytes = 0x0004
			   receivedACK[2] == blockNumber>>8 && receivedACK[3] == blockNumber)		//received ack last two bytes = block number
				break;	
		}   
	}

}

