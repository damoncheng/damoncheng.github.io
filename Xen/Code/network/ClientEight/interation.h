#include "tcp_send_data.h"
static void *sn_interation(void *arg)
{

  unsigned long byte_to_pagenum;
  struct pageInfo * cur_last = NULL;
  struct pageInfo * pre = NULL;
  int i,allocate;
  unsigned long record_next_sum;
  unsigned long record_next_num;
  unsigned long interval;
  
  struct timeval tv;
  double starttime;
  double endtime;

   while(on == 1)
   {
   
       //switch next and current
       pthread_mutex_lock(&switch_mutex);
printf("next_num %d : next_sum %d\n",next_num,next_sum);
       
       record_next_sum = next_sum;
       record_next_num = next_num;

       current = next;
       next = NULL;

       byte_to_pagenum = next_sum/1024/4;
       if(byte_to_pagenum <= 50)
          on = 0;
       next_sum = 0;
       next_num = 0;

       pthread_mutex_unlock(&switch_mutex);

printf("page num %lu\n",byte_to_pagenum);

       cur_last = current; 
       if(cur_last != NULL)
       {

printf("start locate current last!\n");
          if(on == 1)
          {
            //alloate it to PTHREAD_NUM threads
pthread_mutex_lock(&allocate_data_mutex);
            for(i=0;i<PTHREAD_NUM;i++)
            {
               pthread_state[i] = 1;
            }
            interval = record_next_num / PTHREAD_NUM;
printf("interval = %u\n",interval);
            for(allocate = 0,i = 0;cur_last->next != NULL;)
            {
               if((allocate%interval) == 0)
               {
printf("i:%d start_offset:%d data_start_offset:%u\n",i,allocate,cur_last);
                  pthread_send_data[i].start_offset = cur_last;
               }
               else if(((allocate+1)%interval) == 0)
               {
                   pthread_send_data[i].end_offset = cur_last;
printf("i:%d end_offset:%d\n data_end_offset:%u\n",i,allocate,cur_last);
                   i++;
               }

               pre = cur_last;
               cur_last = cur_last->next;

               if((allocate >= (record_next_num - 1)) || (i >= PTHREAD_NUM))
               {
                    if(i >= PTHREAD_NUM)
                        i--;
                    break;
               }

               allocate++;

            }

printf("allocate=%u\n", allocate);
            
            while(pre->next != NULL)
                pre = pre->next;
            pthread_send_data[i].end_offset = pre;

pthread_mutex_unlock(&allocate_data_mutex);

printf("wait send PTHREADS end\n");

            for(i=0;i<PTHREAD_NUM;i++)
            {
              if(pthread_state[i] == 1)
              i = -1;
            }

         }
         else
         {
printf("send last current\n");
gettimeofday(&tv,NULL);
starttime = tv.tv_sec + (double)tv.tv_usec/1000/1000;
tcp_send_data(current);
     
            while(cur_last->next != NULL)
            {
              cur_last = cur_last->next;
            }

         }
      
printf("end locate current last!\n");

       
       //add current to pool
       pthread_mutex_lock(&switch_mutex);

       cur_last->next = pool;
       pool = current;
       current = NULL;
          
       pthread_mutex_unlock(&switch_mutex);

     }
   }

printf("interation while end\n");
   
   while(random_last == 0)
   {
      continue;
   }

   //deal with last next
printf("at last, send remain next\n");
  tcp_send_data(next);
gettimeofday(&tv,NULL);
endtime = tv.tv_sec + (double)tv.tv_usec/1000/1000;

printf("stop time = %lf\n",endtime - starttime);

printf("interation end\n");

   return NULL;
}
