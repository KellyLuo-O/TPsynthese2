#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int main(int argc, char *argv[]) {
	
    if (argc != 3) {
        perror("not enough argument");
        return EXIT_FAILURE;
    }
    
    char *host = argv[1];
    char *file = argv[2];


    return EXIT_SUCCESS;
}

