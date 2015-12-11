#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAXLINE 1024

int main(int argc, char **argv)
{
  
    int servfd, confd;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t servlen,clilen;
    char buf[MAXLINE];
    int n;

    if ( (servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
       perror("create socket\n");
       exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
    servaddr.sin_port = htons(8888);


    if( bind(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
       perror("bind error\n");
       exit(1);
    }

    if( listen(servfd, 10) < 0)
    {
       perror("listen error\n");
       exit(1);
    }

    servlen = sizeof(servaddr);
    if( (confd = accept(servfd, (struct sockaddr *)&cliaddr, &clilen)) < 0 )
    {
       perror("accept error");
       exit(1);
    }

    while(1)
    {
       if( (n = recv(confd, buf, MAXLINE, 0)) == 0 )
       {
          printf("transmit complete!\n");
	  break;
       }
       buf[n] = 0;

       printf("%s",buf);
    }


    return 0;
}
