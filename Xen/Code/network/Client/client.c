#include "../include/page.h"
#include "../include/config.h"

#define MAXLINE 1024

struct page_tran {
   int page_num;
   char * buf;
};

int main(int argc, char **argv)
{

   int confd;
   struct sockaddr_in servaddr;
   socklen_t servlen;
   char buf[MAXLINE];
   int n,i,count;
   char * num;

   struct page_tran   tpage;

   initPage(&page);

   if ( (confd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
   {
       perror("create socket\n");
       exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr =  inet_addr(SERVER);
    servaddr.sin_port = htons(PORT);

    if( connect(confd, (struct sockaddr  *)&servaddr, sizeof(servaddr)) < 0 )
    {
       perror("connect error\n"); 
       exit(1);
    }

printf("connect successfully\n");

    for(i = 0; i < pageNums; i++)
    {

printf("pageNums:%d , i:%d\n", pageNums, i);
       tpage.page_num = i;
       tpage.buf = (char *)page[i];

       num = (char *)(&tpage.page_num);

       count = 0;
       while(count < sizeof(int))
       {
           if ( (n = send(confd, num + count, sizeof(int) - count, 0)) < 0)
           {
               perror("send error\n");
               exit(1);
           }
           count += n;
       }


       count = 0;
       while(count < 4*1024)
       {
           if ( (n = send(confd, tpage.buf + count, 1024*4 - count, 0)) < 0)
           {
               perror("send error\n");
               exit(1);
           }
           count += n;
       }
  

    }
/*
    if ( (n = send(confd, "123", sizeof("123"), 0)) < 0)
    {
       perror("send error\n");
       exit(1);
    }
*/

    freePage(&page);


   return 0;
}

