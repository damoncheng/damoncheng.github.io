#ifndef SN_CONFIG 
#define SN_CONFIG 
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define SERVER "192.168.0.102"
#define PORT   8888

#define MAXLINE 160000
#define PTHREAD_NUM 30

struct page_tran {
   unsigned int page_num;
   char * buf;
};

struct pageInfo {
   unsigned int page_num;
   unsigned int start_offset;
   unsigned int end_offset;
   unsigned int data_size;
   struct pageInfo * next;
};

struct page_offset_tran {
   unsigned int page_num;
   unsigned int start_offset;
   unsigned int end_offset;
   char *buf;
};


struct page_offset_index {
   struct pageInfo * start_offset;
   struct pageInfo * end_offset;
} default_page_offset_index;

struct pageInfo *current = NULL;
struct pageInfo *next = NULL;
struct pageInfo *pool = NULL;
unsigned int on = 1;
unsigned int random_last = 0;
unsigned long next_sum = 0;
unsigned long next_num = 0;
pthread_mutex_t switch_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t allocate_data_mutex = PTHREAD_MUTEX_INITIALIZER;


static struct page_offset_index pthread_send_data[PTHREAD_NUM];
static unsigned int pthread_state[PTHREAD_NUM];

#endif
