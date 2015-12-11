#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define PAGE_SIZE 4096

static char ** page;
static int pageNums;

void initPage(char *** page);
void freePage(char *** page);


void initPage(char *** page)
{
   int i;
   char c;

   pageNums = 512*1024/4;
   *page = (char **)malloc(512*1024/4*sizeof(char *));

   for(i = 0; i< pageNums; i++)
   {
      (*page)[i] = (char *)malloc(4*1024*sizeof(char));
      memset((*page)[i], 49, 4*1024);
      //printf("page[0] size %d\n", sizeof(page[0]));
      //printf("page[i][0] = %c\n", page[i][0]);
   }
}

void freePage(char ***page)
{
   free(*page);
}
