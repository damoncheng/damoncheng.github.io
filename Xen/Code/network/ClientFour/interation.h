static void *sn_interation(void *arg)
{

  unsigned long byte_to_pagenum;
  struct pageInfo * cur_last = NULL;

   while(on == 1)
   {
   
       //switch next and current
       pthread_mutex_lock(&switch_mutex);
printf("next_num %d : next_sum %d\n",next_num,next_sum);
       
       current = next;
       next = NULL;

       byte_to_pagenum = next_sum/1024/4;
       if(byte_to_pagenum <= 50)
          on = 0;
       next_sum = 0;
       next_num = 0;

       pthread_mutex_unlock(&switch_mutex);

       //send current link's data
printf("page num %lu\n",byte_to_pagenum);

       cur_last = current; 
    if(cur_last != NULL)
    {

printf("start locate current last!\n");
       while(cur_last->next != NULL)
       {
          cur_last = cur_last->next;
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
printf("interation end\n");

   return NULL;
}
