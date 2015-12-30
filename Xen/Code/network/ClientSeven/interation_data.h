static void *sn_interation_data(void *arg)
{

   int index;
   struct pageInfo * cur_offset  = NULL;
   struct pageInfo * end_offset = NULL;
   struct page_offset_tran tpage_offset;
   unsigned int data_size;

   int confd;
   struct sockaddr_in servaddr;
   socklen_t servlen;
   //char buf[MAXLINE];
   int n,i,count;
   char * num;
   int sendbuf;


   index =  *(int *)arg;
   cur_offset = pthread_send_data[index].start_offset;
   end_offset = pthread_send_data[index].end_offset;

printf("index = %d boot up\n",index);

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


   while(on == 1)
   {
   
       //index state for own pthread
       if(pthread_state[index] == 1)   
       {
pthread_mutex_lock(&allocate_data_mutex);
pthread_mutex_unlock(&allocate_data_mutex);

cur_offset = pthread_send_data[index].start_offset;
end_offset = pthread_send_data[index].end_offset;
printf("index:%d start send cur_offset:%d end_offset:%d\n",index,cur_offset,end_offset);                
          while(cur_offset != end_offset)
          {
              tpage_offset.page_num = cur_offset->page_num;  
              tpage_offset.start_offset = cur_offset->start_offset;
              tpage_offset.end_offset = cur_offset->end_offset;
              tpage_offset.buf = (char *)(*(page + cur_offset->page_num) + cur_offset->start_offset);
              data_size = cur_offset->end_offset - cur_offset->start_offset;

//printf("page:%d start_offset:%d end_offset:%d data_size:%d\n", 
       // tpage_offset.page_num,tpage_offset.start_offset,tpage_offset.end_offset,data_size);

              num = (char *)(&tpage_offset.page_num);
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

              num = (char *)(&tpage_offset.start_offset);
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

              num = (char *)(&tpage_offset.end_offset);
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
              while(count < data_size)
              {
                 if ( (n = send(confd, tpage_offset.buf + count, data_size - count, 0)) < 0)
                 {
                    perror("send error\n");
                    exit(1);
                 }
                 count += n;
              }

              cur_offset = cur_offset->next;
          }
pthread_mutex_lock(&allocate_data_mutex);
              pthread_state[index] = 0;
pthread_mutex_unlock(&allocate_data_mutex);
              printf("index:%d end send\n",index);                
       }

   }

   close(confd);
   return NULL;
}
