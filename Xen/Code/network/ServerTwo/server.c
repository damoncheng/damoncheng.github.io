#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#define MAXLINE 500*4096

static void *sn_thread(void *arg);


int main(int argc, char **argv)
{
  
    int servfd, confd;
    pthread_t tid;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t servlen,clilen;
    int rcvbuf;
    int temp, templen;

    if ( (servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
       perror("create socket\n");
       exit(1);
    }

    rcvbuf = 4096000;
    if( setsockopt(servfd, SOL_SOCKET, SO_SNDBUF, &rcvbuf, sizeof(int)) < 0 )
    {
       perror("setsockopt error\n");
       exit(1);
    }
printf("set revbuf %d\n", rcvbuf);
    if( setsockopt(servfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(int)) < 0 )
    {
       perror("setsockopt error\n");
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

    if( listen(servfd, 15000) < 0)
    {
       perror("listen error\n");
       exit(1);
    }


    for(;;)
    {

       servlen = sizeof(servaddr);
       if( (confd = accept(servfd, (struct sockaddr *)&cliaddr, &clilen)) < 0 )
       {
           if(errno == EINTR)
           {
              continue;
           }
           else
           {
             perror("accept error");
             exit(1);
           }
       }
   
       getsockopt(confd, SOL_SOCKET, SO_RCVBUF, &temp, &templen);
       printf("conn recvbuf: %d\n", temp);
       pthread_create(&tid, NULL, &sn_thread, (void *)confd);
/*
       if( fork() == 0 ) 
       {
printf("new child\n");
           while( (n = recv(confd, buf, MAXLINE, 0)) != 0 )
           {
              if( n > 0 )
              {
                buf[n] = 0;
                //printf("%s",buf);
	      }
	      else
	      {
	        perror("recv error\n");
		exit(1);
	      }

           }
           printf("transmit complete!......\n");
       }
*/

    }
    
    return 0;
}

static void *sn_thread(void *arg)
{
    int n;
    char buf[MAXLINE];

    pthread_detach(pthread_self());

    while( (n = recv((int)arg, buf, MAXLINE, 0)) != 0 )
    {
        if( n > 0 )
        {
          buf[n] = 0;
          //printf("%s",buf);
        }
        else
        {
          perror("recv error\n");
          pthread_exit(NULL);
        }
    }
    printf("transmit complete!......\n");
    close((int)arg);
}
