#define P_LENGTH 256
#define T_LENGTH 512
#define INIT_LENGTH 8 * 1024
#define ADD_LENGTH  512
#define INTERVAL 4
#define PD_COUNT 5*4*1024

#define _SN_DEBUG

struct page_index {

    unsigned int start;
    unsigned int end;

} default_page_index;

typedef struct {

    unsigned int content_len;
    unsigned int fill_len;
    unsigned int bit_len;
    int socket_fd;
    char * content;
    struct page_index one_page_index;
    unsigned int page_offset_len;
    struct page_offset_index one_page_offset_index;
    unsigned int type;
    unsigned int text_len;
    int f_d[P_LENGTH + 1];

} Code;

void print_p(int *p, int p_length)
{
    for(int i = 0; i < p_length; i++)
    {
         printf("%d\t", p[i]);

         if((i + 1) % 10 == 0)
             printf("\n");
    }

    printf("\n");

}



/*
    0 : send all page
    1 : iterate send dirty page
*/
void get_probability_from_struct(unsigned int type, int * p_d, 
   struct page_index one_page_index, struct page_offset_index one_page_offset_index)
{

#ifndef _SN_DEBUG
printf("#-------get_probability_from_struct start----------#\n");
#endif

    unsigned int wait_content = PD_COUNT;
    unsigned char * a;
    unsigned char b;

    /*0 : send all page*/
    if(type == 0)
    {
        for(unsigned int i = one_page_index.start; i <= one_page_index.end; i++)
        {
           a = (unsigned char *)&i;
           for(unsigned int j = 0; j < 4; j++)
           {
                *(p_d + *(a + j)) += 1;
                if(--wait_content == 0)
                    break;
           }

           if(wait_content == 0)
                break;

           for(unsigned int j = 0; j < PAGE_SIZE; j++)
           {
                //printf("i = %d\n", i);
                b = (unsigned char)((*page)[i] + j);       
                
                *(p_d + b) += 1;
                if(--wait_content == 0)
                    break;
           }

           if(wait_content == 0)
                break;

           //printf("wait_content = %d\n", wait_content);
           //exit(1);

        }
    }
    /*1 : send dirty page */
    else if(type == 1)
    {
        struct pageInfo * cur_page_offset;
        unsigned int page_num, start_offset, end_offset;
        cur_page_offset = one_page_offset_index.start_offset;

        while(cur_page_offset != one_page_offset_index.end_offset)
        {

            a = (unsigned char *)&cur_page_offset->page_num;
            for(unsigned int j = 0; j < 4; j++)
            {
                *(p_d + *(a + j)) += 1;
                if(--wait_content == 0)
                    break;
            }

            if(wait_content == 0)
                break;

            a = (unsigned char *)&cur_page_offset->start_offset;
            for(unsigned int j = 0; j < 4; j++)
            {
                *(p_d + *(a + j)) += 1;
                if(--wait_content == 0)
                    break;
            }

            if(wait_content == 0)
                break;

            a = (unsigned char *)&cur_page_offset->end_offset;
            for(unsigned int j = 0; j < 4; j++)
            {
                *(p_d + *(a + j)) += 1;
                if(--wait_content == 0)
                    break;
            }

            if(wait_content == 0)
                break;

           page_num = cur_page_offset->page_num;
           start_offset = cur_page_offset->start_offset;
           end_offset = cur_page_offset->end_offset;
           for(unsigned int j = start_offset; j < end_offset; j++)
           {
                //printf("i = %d\n", i);
                b = (unsigned char)((*page)[page_num] + j);       
                
                *(p_d + b) += 1;
                if(--wait_content == 0)
                    break;
           }

           if(wait_content == 0)
                break;
        }
        
    }

#ifndef _SN_DEBUG
printf("#--------get_probability_from_struct end---------#\n");
#endif

};

void init_encode(unsigned int type, int socket_fd, Code * encode, unsigned int cf ,
    struct page_index one_page_index, struct page_offset_index one_page_offset_index)
{

#ifndef _SN_DEBUG
printf("#--------init_encode start---------#\n");
#endif

    int p_d[P_LENGTH];

    for(int i = 0; i < P_LENGTH; i++)
        p_d[i] = 1;

    if(type == 0)
    {

        encode->type = type;
        encode->one_page_index = one_page_index;
        encode->content_len = INIT_LENGTH;
        encode->fill_len = 0;
        encode->bit_len = 0;
        encode->socket_fd = socket_fd;
        if( (encode->content = (char *)malloc(sizeof(char) * INIT_LENGTH)) == NULL )
        {
                printf("malloc memery failed\n");
                exit(1);
        }
        memset(encode->content, 0, INIT_LENGTH);

        encode->text_len = (one_page_index.end - one_page_index.start) * (sizeof(unsigned int) + PAGE_SIZE);
        get_probability_from_struct(type, p_d, one_page_index, default_page_offset_index);

        int sum = 0;
        for(int i = 0; i < P_LENGTH; i++)
        {
               encode->f_d[i] = sum;
               sum += p_d[i];
        }
        encode->f_d[P_LENGTH] = sum;

    }
    else if(type == 1)
    {

printf("type = 1 : init_encode start\n");
        unsigned int loop = 0;

        encode->type = type;
        encode->one_page_offset_index = one_page_offset_index;
        encode->content_len = INIT_LENGTH;
        encode->fill_len = 0;
        encode->bit_len = 0;
        encode->socket_fd = socket_fd;
        encode->page_offset_len = 0;

        if(cf == 1)
        {
            if( (encode->content = (char *)malloc(sizeof(char) * INIT_LENGTH)) == NULL )
            {
                printf("malloc memery failed\n");
                exit(1);
            }
            memset(encode->content, 0, INIT_LENGTH);
        }

        encode->text_len = 0;
        struct pageInfo * cur_page_offset;
        cur_page_offset = one_page_offset_index.start_offset;
        while(cur_page_offset != one_page_offset_index.end_offset)
        {
printf("#-----while text_len = %u------\n", encode->text_len);
            loop++;
            encode->text_len += sizeof(cur_page_offset->page_num) + 
                sizeof(cur_page_offset->start_offset) + sizeof(cur_page_offset->end_offset)
                    + (cur_page_offset->end_offset - cur_page_offset->start_offset);
            if(loop == 2)
                break;
            cur_page_offset = cur_page_offset->next;
        }

        if(cf == 1)
        {
            get_probability_from_struct(type, p_d, default_page_index, 
                one_page_offset_index);

            int sum = 0;
            for(int i = 0; i < P_LENGTH; i++)
            {
                   encode->f_d[i] = sum;
                   sum += p_d[i];
            }
            encode->f_d[P_LENGTH] = sum;
        }

    
    }

//#ifndef _SN_DEBUG
printf("#-----text_len = %u------\n", encode->text_len);
printf("#--------init_encode end---------#\n");
//#endif
    

};

void transmit_struct(int socket_fd, char * buf, unsigned int size)
{

#ifndef _SN_DEBUG
printf("#--------transmit_struct start----------#\n");
#endif

    int count = 0;
    int n;
    while(count < size)
    {
         if ( (n = send(socket_fd, buf + count, size - count, 0)) < 0)
         {
               perror("send error\n");
               exit(1);
         }
         count += n;
    }

#ifndef _SN_DEBUG
printf("#--------transmit_struct end----------#\n");
#endif

}

void update_probability(int * tp_d, unsigned int fd_l)
{
#ifndef _SN_DEBUG
printf("#---------update_probability start--------#\n");
#endif
    *(tp_d + fd_l) += 1;
#ifndef _SN_DEBUG
printf("#---------update_probability end--------#\n");
#endif
}

void replace_probability(int * f_d, int * tp_d)
{
#ifndef _SN_DEBUG
printf("#---------replace probability start--------#\n");
#endif
    int sum = 0;
    for(int i = 0; i < P_LENGTH; i++)
    {
         *(f_d + i) = sum;
         sum += *(tp_d + i);
    }
    *(f_d + P_LENGTH) = sum;
#ifndef _SN_DEBUG
printf("#---------replace probability end--------#\n");
#endif
}


char get_byte_from_struct(Code * encode, unsigned int j)
{
#ifndef _SN_DEBUG
printf("#--------get_byte_from_struct start--------#\n");
#endif
    unsigned int unit_struct = 0;

    if(encode->type == 0)
    {
        unsigned int page_index;
        unsigned int bit_index;
        struct page_tran tpage;
        char * a;

        unit_struct = sizeof(unsigned int) + PAGE_SIZE;
        page_index = j / unit_struct;
        bit_index = j % unit_struct;
        tpage.page_num = page_index;
        tpage.buf = (char *)(page[page_index]);

        if(bit_index < sizeof(unsigned int))
        {
            a = (char *)&tpage.page_num;  
            return *(a + bit_index);
        }
        else
        {
            a = tpage.buf + (bit_index - sizeof(unsigned int));
            return *a;
        }

    }
    else if(encode->type == 1)
    {

//printf("type = 1 get_byte_from_struct start\n");
        unsigned int page_index;
        unsigned int bit_index;
        struct page_offset_tran tpage_offset;
        unsigned cur_struct_len = 0;
        char * a;
        unsigned char b;

        tpage_offset.page_num = encode->one_page_offset_index.start_offset->page_num;
        tpage_offset.start_offset = encode->one_page_offset_index.start_offset->start_offset;
        tpage_offset.end_offset = encode->one_page_offset_index.start_offset->end_offset;
        tpage_offset.buf = (char *)(*(page + tpage_offset.page_num) + tpage_offset.start_offset);


        cur_struct_len = sizeof(tpage_offset.page_num) + sizeof(tpage_offset.start_offset)
                + sizeof(tpage_offset.end_offset) + 
                (tpage_offset.end_offset - tpage_offset.start_offset);

        page_index = tpage_offset.page_num;
        bit_index = j - encode->page_offset_len;

        if(bit_index < sizeof(unsigned int))
        {
            a = (char *)&tpage_offset.page_num;  
            b =  *(a + bit_index);
        }
        else if(bit_index < 2 * sizeof(unsigned int))
        {
            a = (char *)&tpage_offset.start_offset;
            b = *(a + bit_index - sizeof(unsigned int));
        }
        else if(bit_index < 3 * sizeof(unsigned int))
        {
            a = (char *)&tpage_offset.end_offset;
            b = *(a + bit_index - 2 * sizeof(unsigned int));
        }
        else
        {
            a = tpage_offset.buf + (bit_index - 3 * sizeof(unsigned int));
            b = *a;
        }
        
//printf("#--------j = %u, extend->text_len = %u\n", j, encode->text_len);

        if((j - encode->page_offset_len + 1) % cur_struct_len == 0)
        {

//printf("#--------extend text->len = %u\n", cur_struct_len);

            if(encode->one_page_offset_index.start_offset != 
            encode->one_page_offset_index.end_offset)
            {
                encode->page_offset_len += cur_struct_len;  
                encode->one_page_offset_index.start_offset = 
                    encode->one_page_offset_index.start_offset->next;

                if(encode->one_page_offset_index.start_offset != 
                encode->one_page_offset_index.end_offset)
                {
                     encode->text_len += 
                             3 * sizeof(unsigned int) +
                            (encode->one_page_offset_index.start_offset->next->end_offset -
                            encode->one_page_offset_index.start_offset->next->start_offset);

//printf("add encode->text = %u\n", encode->text_len);

                }
            }
        }

        return b; 
    }
#ifndef _SN_DEBUG
printf("#--------get_byte_from_struct end--------#\n");
#endif
}

void transmit_bit(Code *encode)
{
#ifndef _SN_DEBUG
printf("#-----------transmit_bit start---------#\n");
#endif
    if(encode->bit_len > 0)
        encode->fill_len += 1;

    transmit_struct(encode->socket_fd, encode->content, encode->fill_len);

    encode->fill_len = 0;
    encode->bit_len = 0;
    memset(encode->content, 0, encode->content_len);
#ifndef _SN_DEBUG
printf("#-----------transmit_bit end---------#\n");
#endif
}

void add_bit(Code *encode, int bit)
{
#ifndef _SN_DEBUG
printf("#-----------add_bit start---------#\n");
#endif
    if(bit == 1)
        *(encode->content + encode->fill_len) |= bit << (7 - encode->bit_len);

    if(encode->bit_len == 7)
    {
        encode->fill_len++;
        encode->bit_len = 0;
    }
    else
    {
        encode->bit_len++;
    }

    if(encode->fill_len == encode->content_len)
        transmit_bit(encode);
#ifndef _SN_DEBUG
printf("#-----------add_bit end---------#\n");
#endif
}


void encode_text(Code * encode)
{
#ifndef _SN_DEBUG
printf("#---------encode_text start------#\n");
#endif
    unsigned int T;
    unsigned int R = 1;
    unsigned int l, u, ll, uu;
    unsigned int m = 0; 
    unsigned int f_dl, f_du;
    unsigned char bit = 0x01;
    unsigned int bit_offset = 7;

    int tp_d[P_LENGTH];
    for(int i = 0; i < P_LENGTH; i++)
        tp_d[i] = 1;

    T = encode->f_d[P_LENGTH];
    while(R <= (4 * (PD_COUNT + P_LENGTH)))
        R = R << 1;

    l = 0;
    u = R - 1;

    transmit_struct(encode->socket_fd, (char *)(&encode->type), sizeof(unsigned int));
    transmit_struct(encode->socket_fd, (char *)(&encode->text_len), sizeof(unsigned int));
    transmit_struct(encode->socket_fd, (char *)(&encode->f_d), (P_LENGTH + 1) * sizeof(int));


    for(unsigned long j = 0; j < encode->text_len; j++)
    {
        ll = l;
        uu = u;

//printf("encode_text  j = %u, text_len = %u\n", j, encode->text_len);

        f_dl = (unsigned int)(unsigned char)get_byte_from_struct(encode, j);

/*
    printf("before ll = %u, uu = %u, byte = %u\n", ll, uu, f_dl);
        if(j == 20480)
        {
            print_p(tp_d, P_LENGTH);
            print_p(encode->f_d, P_LENGTH + 1);
        }
*/

        if((j != 0) && ((j % (PD_COUNT)) == 0))
        {
            replace_probability(encode->f_d, tp_d);
            for(int i = 0; i < P_LENGTH; i++)
                tp_d[i] = 1;
        }

        for(unsigned int i = f_dl + 1; i < P_LENGTH + 1; i++)
            if(encode->f_d[i] != -1)
            {
                f_du = i;
                break;
            }

        ll = l + floor((u - l + 1) * encode->f_d[f_dl] / T);
        uu = l + floor((u - l + 1) * encode->f_d[f_du] / T) - 1;

        update_probability(tp_d, f_dl);

/*
    if(j == 20482)
    {
        print_p(tp_d, P_LENGTH);
        print_p(encode->f_d, P_LENGTH + 1);
    }
    printf("after ll = %u, uu = %u, R = %u\n", ll, uu, R);
*/

        while(1)
        {
            if(ll >= R/2)
            {
                add_bit(encode, 1);
                ll = 2 * ll - R;
                uu = 2 * uu - R + 1;

                while(m-- > 0)
                    add_bit(encode, 0);

//printf("up\n");

                m = 0;
            }
            else if(uu < R/2)
            {
                add_bit(encode, 0);

                ll = 2 * ll;
                uu = 2 * uu + 1;

                while(m-- > 0)
                    add_bit(encode, 1);

//printf("down\n");
                m = 0;
            }
            else if((ll >= R/4) && (uu < 3*R/4))
            {
                ll = 2 * ll - R / 2;
                uu = 2 * uu - R / 2 + 1;
                m++;
//printf("middle\n");
            }
            else
            {
                l = ll;
                u = uu;
                break;
            }
        }

    }

    if(l >= R/4)
    {
        add_bit(encode, 1);
        while(m-- > 0)
            add_bit(encode, 0);
        add_bit(encode, 0);
    }
    else
    {
        add_bit(encode, 0);
        while(m-- > 0)
            add_bit(encode, 1);
        add_bit(encode, 1);
    }

    transmit_bit(encode);

#ifndef _SN_DEBUG
printf("#---------encode_text end------#\n");
#endif

};
