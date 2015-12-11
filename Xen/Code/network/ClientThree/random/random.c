#include <stdio.h>
#include  <stdlib.h>

int main(int argc,char **argv)
{
  unsigned int i;
  unsigned int page,start_offset,end_offset;
  
  //printf("random %u\n",time(NULL));
  srand((unsigned int)time(0));
for(i = 0;i < 10;i++)  
{
  page = rand() % (512*1024/4);  
  start_offset = rand() % (4*1024); 
  end_offset = rand() % (4*1024 - start_offset) + start_offset;

  printf("%-5u\t%-5u\t%-5u\n",page,start_offset,end_offset);
}

  exit(0);
}

