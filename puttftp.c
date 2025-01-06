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
	
}

