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

/* A buffer large enough to contain the longest allowed string */
#define BUFSIZE 1219

#define USAGE                                                                       \
    "usage:\n"                                                                      \
    "  echoclient [options]\n"                                                      \
    "options:\n"                                                                    \
    "  -s                  Server (Default: localhost)\n"                           \
    "  -p                  Port (Default: 19121)\n"                                  \
    "  -m                  Message to send to server (Default: \"Hello world.\")\n" \
    "  -h                  Show this help message\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"server", required_argument, NULL, 's'},
    {"port", required_argument, NULL, 'p'},
    {"message", required_argument, NULL, 'm'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

/* Main ========================================================= */
int main(int argc, char **argv)
{
    int option_char = 0;
    int mysock;
    struct sockaddr_in server;
    struct hostent *host;
    char buffer[BUFSIZE];
    char *hostname = "localhost";
    unsigned short portno = 19121;
    char *message = "Hello World!!";

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "s:p:m:hx", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 's': // server
            hostname = optarg;
            break;
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'm': // message
            message = optarg;
            break;
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        }
    }

    setbuf(stdout, NULL); // disable buffering

    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }

    if (NULL == message)
    {
        fprintf(stderr, "%s @ %d: invalid message\n", __FILE__, __LINE__);
        exit(1);
    }

    if (NULL == hostname)
    {
        fprintf(stderr, "%s @ %d: invalid host name\n", __FILE__, __LINE__);
        exit(1);
    }

    /*Instatiate server info*/
    host = gethostbyname(hostname);
      
    bzero((char *) &server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(portno);
    bcopy((char *)host->h_addr,
          (char *)&server.sin_addr.s_addr,
	  host->h_length);

    /*Instantiate socket and connect to the server*/
    mysock = socket(AF_INET, SOCK_STREAM, 0);
    if (mysock < 0) 
        printf("ERROR opening socket");

    if(connect(mysock,(struct sockaddr *)&server, sizeof(server)) < 0)
	printf("ERROR connecting");

    /*Send message to server*/
    send(mysock, message, strlen(message), 0);

    /*Read server response*/
    recv(mysock, buffer, BUFSIZE, 0);
    buffer[strlen(message)] = 0;
    printf("%s", buffer);
}

