#ifndef TCP_SEND_DATA
#define TCP_SEND_DATA

void tcp_send_data(struct pageInfo * next)
{

   int confd;
   struct sockaddr_in servaddr;
   socklen_t servlen;
   //char buf[MAXLINE];
   int n,i,count;
   char * num;
   int sendbuf;

   struct page_offset_tran tpage_offset;
   unsigned int data_size;
   struct pageInfo * temp;

   temp = next;

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

   while(temp != NULL)
   {
   
       tpage_offset.page_num = temp->page_num;
//       tpage_offset.start_offset = temp->start_offset;
//       tpage_offset.end_offset = temp->end_offset;
//       tpage_offset.buf = (char *)(*(page + temp->page_num) + temp->start_offset);
       tpage_offset.start_offset = 0;
       tpage_offset.end_offset = 4096;
       tpage_offset.buf = (char *)(*(page + temp->page_num) + 0);
       data_size = temp->end_offset - temp->start_offset;

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


       temp = temp->next;     
     
   }


   close(confd);


}



#endif
