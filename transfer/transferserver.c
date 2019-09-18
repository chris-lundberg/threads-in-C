#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define BUFSIZE 1219

#define USAGE                                                \
    "usage:\n"                                               \
    "  transferserver [options]\n"                           \
    "options:\n"                                             \
    "  -f                  Filename (Default: 6200.txt)\n" \
    "  -h                  Show this help message\n"         \
    "  -p                  Port (Default: 19121)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"filename", required_argument, NULL, 'f'},
    {"help", no_argument, NULL, 'h'},
    {"port", required_argument, NULL, 'p'},
    {NULL, 0, NULL, 0}};

int main(int argc, char **argv)
{
    int option_char;
    int portno = 19121;             /* port to listen on */
    char *filename = "6200.txt"; /* file to transfer */
    int maxnpending = 1;
    int mysock, newsock, clilen;
    char buffer[BUFSIZE];
    struct sockaddr_in server, client;

    setbuf(stdout, NULL); // disable buffering

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "p:hf:x", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        case 'f': // file to transfer
            filename = optarg;
            break;
        }
    }


    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }
    
    if (NULL == filename)
    {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }

        /* Instatiate socket and create connection*/
    mysock = socket(AF_INET, SOCK_STREAM, 0);
    if (mysock < 0)
        printf("ERROR opening socket");

    bzero((char *) &server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portno);

    int option = 1;
    setsockopt(mysock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        
    if(bind(mysock, (struct sockaddr *) &server, (socklen_t) sizeof(server)) < 0)
	    printf("ERROR on binding");

    /* Listen on the connection and accept connection*/
    listen(mysock, maxnpending);
    clilen = sizeof(client);

    while(1) {
      newsock = accept(mysock, (struct sockaddr *) &client,(socklen_t *) &clilen);
     
      if(newsock < 0)
         printf("ERROR on accept");

      FILE *outfile = fopen(filename, "r");

      bzero(buffer, BUFSIZE); 
      int block_size; 
      while((block_size = fread(buffer, sizeof(char), BUFSIZE, outfile))>0){
     
             if(send(newsock, buffer, block_size, 0) < 0){
	                printf("ERROR: Failed to Send File");
	                exit(1);
                }

      bzero(buffer, BUFSIZE);

      }	      
    }
    return 0;
}
