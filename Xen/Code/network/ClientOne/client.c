#include "../include/page.h"
#include "../include/config.h"

static void *sn_thread(void *arg);

#define MAXLINE 160000
#define PTHREAD_NUM 10
//now the best is 30

struct page_tran {
   int page_num;
   char * buf;
};

int main(int argc, char **argv)
{
   
   int i;
   pthread_t tid[PTHREAD_NUM+1];
   int n[PTHREAD_NUM+1];

   initPage(&page);

   for(i = 0; i <= PTHREAD_NUM; i++)
   {
printf("thread create\n");
       n[i] = i;
       pthread_create(&tid[i], NULL, &sn_thread, (void *)&n[i]);
printf("thread create end\n");
   }

   for(i = 0; i<= PTHREAD_NUM; i++)
   {
      pthread_join(tid[i],NULL );
   }

   freePage(&page);

printf("main return\n");

   return 0;
}

static void *sn_thread(void *arg)
{
   
   int confd;
   struct sockaddr_in servaddr;
   socklen_t servlen;
   //char buf[MAXLINE];
   int n,i,count,start,interval,end;
   char * num;
   int sendbuf;

   interval = pageNums / PTHREAD_NUM;
   start = *(int *)arg;
   if(start == PTHREAD_NUM)
   {
      end = pageNums;
      start = interval * start;
   }
   else
   {
      end = (start + 1) * interval;
      start = interval * start;
   }
printf("start:%d, end:%d\n",start, end);
   struct page_tran   tpage;

//printf("i = %d page[i]=%d\n",i,page[i]);

   if ( (confd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
   {
       perror("create socket\n");
       exit(1);
   }
   
   sendbuf = 4096000;
   if( setsockopt(confd, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(int)) < 0)
   {
      perror("setsockopt error:\n");
      exit(1);
   }
   if( setsockopt(confd, SOL_SOCKET, SO_RCVBUF, &sendbuf, sizeof(int)) < 0)
   {
      perror("setsockopt error:\n");
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

for(i = start ; i < end; i++)
{

//printf("pageNums:%d , i:%d\n", pageNums, i);
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
    while(count < PAGE_SIZE)
    {
       if ( (n = send(confd, tpage.buf + count, PAGE_SIZE - count, 0)) < 0)
       {
          perror("send error\n");
          exit(1);
       }
       count += n;
    }

}

  close(confd);
  printf("thread end\n");

  return NULL;
}
