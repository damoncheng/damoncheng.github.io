#define _BSD_SOURCE

#include "../include/page.h"
#include "../include/config.h"
#include "compress.h"
#include "interation.h"
#include "interation_data.h"


static void *sn_thread(void *arg);
static void *sn_random(void *arg);
//static void *sn_interation(void *arg);


int main(int argc, char **argv)
{
   
   int i;
   pthread_t random_pid;
   pthread_t interation_pid;
   pthread_t tid[PTHREAD_NUM+1];
   int n[PTHREAD_NUM];

   initPage(&page);

//turn on random access
pthread_create(&random_pid, NULL, &sn_random, NULL);



//first : send all
   for(i = 0; i < PTHREAD_NUM; i++)
   {
printf("thread create\n");
       n[i] = i;
       pthread_create(&tid[i], NULL, &sn_thread, (void *)&n[i]);
printf("thread create end\n");
   }

   for(i = 0; i< PTHREAD_NUM; i++)
   {
      pthread_join(tid[i],NULL );
   }



//second: interative transmit    
pthread_create(&interation_pid, NULL, &sn_interation, NULL);
   for(i = 0; i < PTHREAD_NUM; i++)
   {
       n[i] = i;
       pthread_create(&tid[i], NULL, &sn_interation_data, (void *)&n[i]);
   }


//third: stop transmit
   pthread_join(random_pid,NULL );
   pthread_join(interation_pid,NULL );
   for(i = 0; i< PTHREAD_NUM; i++)
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
   if(start == (PTHREAD_NUM - 1))
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

Code * code;

//for(i = start ; i < end; i++)
//{

//printf("pageNums:%d , i:%d\n", pageNums, i);
    if( (code = (Code *)malloc(sizeof(Code))) == NULL )
    {
         printf("sn_thread malloc failed\n");
         exit(1);
    }

    struct page_index one_page_index;
    one_page_index.start = start;
    one_page_index.end = end;
    //tpage.page_num = i;
    //tpage.buf = (char *)page[i];
    //num = (char *)(&tpage.page_num);

    init_encode(0, confd, code, one_page_index, default_page_offset_index); 
    encode_text(code);


/*
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
*/


//}

  close(confd);
  printf("sn_thread end\n");

  return NULL;
}

static void *sn_random(void *arg)
{

   unsigned int i;
   unsigned int page,start_offset,end_offset;
   struct pageInfo * pre = NULL;
   struct pageInfo * cur = NULL;
   struct pageInfo * node = NULL;
       
   //printf("random %u\n",time(NULL));
   srand((unsigned int)time(0));
   while(on == 1)
   {

usleep(200);
      page = rand() % (512*1024/4);  
      start_offset = rand() % (4*1024); 
      end_offset = rand() % (4*1024  - start_offset) + start_offset;

/*
  printf("page:%u start_offset:%u end_offset:%u\n",
          page,start_offset,end_offset);
*/

      //printf("%-5u\t%-5u\t%-5u\n",page,start_offset,end_offset);
      cur = next;
      pre = NULL;
      while(cur != NULL)
      {
         if(cur->page_num >= page)
            break;

//printf("cur page_num %d\n",cur->page_num);

          pre = cur;
          cur = cur->next;
      }
    
      pthread_mutex_lock(&switch_mutex);
      if( cur != NULL && cur->page_num == page)
      {
          if(start_offset < cur->start_offset)
          {
             cur->start_offset = start_offset;
             next_sum = next_sum + (cur->start_offset - start_offset);
          }

          if(end_offset > cur->end_offset)
          {
             cur->end_offset = end_offset;      
             next_sum = next_sum + (end_offset - cur->end_offset);
          }

      }
      else
      {
          if(pool != NULL)
          {
             node = pool; 
             pool = node->next;
          }
          else
          {
             node = (struct pageInfo *)malloc(sizeof(struct pageInfo)); 
             if (node == NULL)
             {
                 printf("node malloc pageInfo error\n");
                 continue;
             }
          }

          node->page_num = page;
          node->start_offset = start_offset;
          node->end_offset = end_offset;
          node->next = NULL;
          
          if(cur == NULL && pre == NULL)
          {
//printf("cur = NULL & pre = NULL\n");
             next = node;
          }
          else if(cur == NULL && pre != NULL)
          {
//printf("cur = NULL & pre != NULL\n");
             pre->next = node;
          }
          else if(cur != NULL && pre == NULL)
          {
//printf("cur != NULL & pre = NULL\n");
             next = node;
             node->next = cur;
          }
          else
          {
//printf("cur != NULL & pre != NULL\n");
             pre->next = node;
             node->next = cur;
          }

          next_sum = next_sum + (end_offset - start_offset);
          next_num++;
      }
      pthread_mutex_unlock(&switch_mutex);
      
   }

   random_last = 1;

printf("random end\n");

   return NULL;

}
