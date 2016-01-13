#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define P_LENGTH 256
#define T_LENGTH 512
#define INIT_LENGTH 1024
#define ADD_LENGTH  512
#define INTERVAL 4
#define PD_COUNT 5*4*1024

#define COMPRESS_SUFFIX ".sn"

typedef struct {

    unsigned int len;
    unsigned int fill_len;
    unsigned int bit_len;
    char * content;
    unsigned int text_len;
    int f_d[P_LENGTH + 1];

} Code;

typedef struct {

    unsigned long len;
    unsigned long read_len;
    unsigned offset;
    int original_fd;
    int encode_fd;
    char * content;

} Text;


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

void print_code(Code * encode)
{

printf("#------print code------#\n");

    unsigned int length = encode->fill_len * 8 + encode->bit_len;
    unsigned int byte, bit;

    for(unsigned int i = 0; i < length; i++)
    {
        byte = i / 8;
        bit = 7 -  (i % 8);

        if(encode->content[byte] & (0x01 << bit))
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }

    printf("\n");
    printf("length = %u\n", length);

}

void update_probability(int * tp_d, unsigned int fd_l)
{
    *(tp_d + fd_l) += 1;
}

void replace_probability(int * f_d, int * tp_d)
{

    int sum = 0;
    for(int i = 0; i < P_LENGTH; i++)
    {
        *(f_d + i) = sum;
        sum += *(tp_d + i);
    }
    *(f_d + P_LENGTH) = sum;

}

void extend_content(Code * encode)
{

printf("#-------extend_content\n");

    if( (encode->content = (char *)realloc((encode->content), ADD_LENGTH)) 
        == NULL )
    {
        printf("extend_content's realloc failed\n");
        exit(1);
    }
    memset(encode->content + encode->len, 0, ADD_LENGTH);
    encode->len += ADD_LENGTH;
}

void persist_struct(int fd, char * s, unsigned int len)
{

    if( write(fd, s, len) == -1 )
    {
        printf("persist_struct write failed\n");
        exit(1);
    }

}

void get_struct(Code * encode, int fd)
{
    unsigned int len;
    
    len = read(fd, (char *)(&encode->text_len), sizeof(unsigned int));
    while( len < sizeof(unsigned int) )
    {
        lseek(fd, 0, SEEK_SET);
        len = read(fd, (char *)(&encode->text_len), sizeof(unsigned int));
    }

    len = read(fd, (char *)(encode->f_d), (P_LENGTH  + 1) * sizeof(int));
    while(len < (P_LENGTH + 1) * sizeof(int))
    {
        lseek(fd, sizeof(unsigned int), SEEK_SET);
        len = read(fd, (char *)(encode->f_d), (P_LENGTH + 1) * sizeof(int));
    }
    

}


void persist_bit(Code *encode, Text * text)
{

    int fd; 

    fd = text->encode_fd;

printf("persist_bit:");
print_code(encode);
    if(encode->bit_len > 0)
        encode->fill_len += 1;

    if( write(fd, encode->content, encode->fill_len) == -1 )
    {
        printf("persist_bit write failed\n");
        exit(1);
    }

    encode->fill_len = 0;
    encode->bit_len = 0;
    memset(encode->content, 0, encode->len);

}

void add_bit(Code *encode, int bit, Text * text)
{

//printf("add_bit : bit = %d, fill_len = %d, bit_len = %d, len = %d\n", 
//        bit, encode->fill_len, encode->bit_len, encode->len);

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
    if((encode->fill_len) == encode->len)
        persist_bit(encode, text);

}

int get_bit(Code * encode, int offset, int encoded_fd)
{
    unsigned int byte_offset, bit_offset;
    //int len;
    unsigned int bit = 1;

    if(encode->fill_len == 0)
    {

//printf("#----------------get_bit read again\n");
        encode->bit_len += encode->len;
        encode->fill_len = read(encoded_fd, encode->content, INIT_LENGTH);
        encode->len = encode->fill_len;
//print_code(encode);
    }

    byte_offset = (offset - encode->bit_len * 8) / 8;
    bit_offset = (offset - encode->bit_len * 8) % 8;

//printf("fill_len = %d, bit_len = %d\n", encode->fill_len, encode->bit_len);
//printf("byte_offset = %d, bit_offset = %d\n", byte_offset, bit_offset);

    if(bit_offset == 7)
        encode->fill_len--;

    if(encode->content[byte_offset] & (bit << (7 - bit_offset)))
    {
        //printf("bit = %d\n", 1);
        return 1;
    }

    //printf("bit = %d\n", 0);
    return 0;
}

char get_byte_from_file(Text * text, unsigned long index)
{
    unsigned int len;
    char c;

    if(index >= text->read_len)
    {

printf("#---------------read from file\n");
        if((len = read(text->original_fd, text->content, INIT_LENGTH)) <= 0)       
        {
            printf("read data over boundray\n");
            exit(1);
        }

        text->read_len += len;
        text->offset = 0;
    }

    c = text->content[text->offset];
    text->offset++;


    return c;
}

void encode_text(Text * text, Code * encode)
{
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

    /*
    while(R <= (T * INTERVAL))
        R = R << 1;
    */

//printf("t_lenth = %u, T = %u, R = %u\n", text->len, T, R);

    l = 0;
    u = R - 1;


    persist_struct(text->encode_fd, (char *)(&encode->text_len), sizeof(unsigned int));
    persist_struct(text->encode_fd, (char *)(encode->f_d), (P_LENGTH + 1) * sizeof(int));
    for(unsigned long j = 0; j < text->len; j++)
    {
        ll = l;
        uu = u;


        //f_dl = (unsigned int)text.content[j];
        f_dl = (unsigned int)(unsigned char)get_byte_from_file(text, j);

        //here to update encode->f_d by cache
        if((j != 0) && ((j % (PD_COUNT)) == 0))
        {

printf("replace probability %lu\n", j);
            replace_probability(encode->f_d, tp_d);  
            for(int i = 0; i < P_LENGTH; i++)
                tp_d[i] = 1;
        }


//printf("before text = %u, fill_len = %u, ll = %u, uu = %u\n", 
//    f_dl, encode->fill_len, ll, uu);
        for(unsigned int i = f_dl + 1; i < P_LENGTH + 1; i++)
            if(encode->f_d[i] != -1)
            {
                f_du = i;
                break;
            }


//printf("f_dl = %u, f_du = %u\n", encode->f_d[f_dl], encode->f_d[f_du]);

        ll = l + floor((u - l + 1) * encode->f_d[f_dl] / T);
        uu = l + floor((u - l + 1) * encode->f_d[f_du] / T) - 1;

        //here to update cache probability table
        update_probability(tp_d, f_dl);

//printf("encode fill_len = %u, ll = %u, uu = %u\n", encode->fill_len, ll, uu);

        while(1)
        {
            if(ll >= R/2)
            {
                add_bit(encode, 1, text);
                    
                ll = 2 * ll - R ;
                uu = 2 * uu - R + 1;

                while(m-- > 0)
                    add_bit(encode, 0, text);

                m = 0;

//printf("upper encode_len = %u, fill_len = %u ll = %u, uu = %u\n", 
//        encode->len, encode->fill_len, ll, uu);
            }
            else if(uu < R/2)
            {
                add_bit(encode, 0, text);

                ll = 2 * ll;
                uu = 2 * uu + 1;
                while(m-- > 0)
                    add_bit(encode, 1, text);

//printf("down encode_len = %u, fill_len = %u, ll = %u, uu = %u\n", 
//        encode->len, encode->fill_len, ll, uu);
                m = 0;
            }
            else if((ll >= R/4) && (uu < 3*R/4))
            {
                ll = 2 * ll - R/2;           
                uu = 2 * uu - R/2 + 1;
                m++;
//printf("middle encode_len = %u, fill_len = %u, ll = %u, uu = %u\n", 
//        encode->len, encode->fill_len, ll, uu);
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
        add_bit(encode, 1, text);
        while(m-- > 0)
            add_bit(encode, 0, text);
        add_bit(encode, 0, text);
    }
    else
    {
        add_bit(encode, 0, text);
        while(m-- > 0)
            add_bit(encode, 1, text);
        add_bit(encode, 1, text);
    }

//  printf("#---snsn-----%d", encode->fill_len);

    persist_bit(encode, text);

//printf("encode = %s\n", encode);
//printf("encode = %u\n", encode->content[1]);
print_code(encode);
    
}

void decode_text(Code * encode, int encoded_fd)
{

    unsigned int T, t = 0, tt;
    unsigned int fd_l, fd_u;
    unsigned int R = 1;
    unsigned int l, u, ll, uu;
    unsigned int k = 0;
    unsigned int bit = 1;
    unsigned int i = 0;
    int m;
    unsigned int word = 0;


    int tp_d[P_LENGTH];
    for(int i = 0; i < P_LENGTH; i++)
        tp_d[i] = 1;

    get_struct(encode, encoded_fd);

//printf("\n#-----start encode->f_d------#\n");
//print_p(encode->f_d, P_LENGTH + 1);
//printf("\n#-----end encode->f_d------#\n");

    T = encode->f_d[P_LENGTH];
//printf("#---------docode text T= %d\n", T);
    while(R <= (4 * (PD_COUNT + P_LENGTH)))
    {
        R = R << 1;
        word++;
    }
    /*
    while(R <= (T * INTERVAL))
    {
        R = R << 1;
        word++;
    }
    */

    l = 0;
    u = R - 1;
    
    //get first word bit from content
    for(i = 0; i < word; i++)
    {
        if(get_bit(encode, i, encoded_fd) == 1)
        {
            t += bit << (word - i - 1);
        }
//printf("word = %u  t = %u\n", word, t);
    }

//printf("t_length = %u\n", encode->text_len);


    for(unsigned int j = 0; j < encode->text_len; j++)
    {


        m = floor(((t - l + 1) * T - 1) / (u - l + 1));
//printf("word = %u  t = %u m = %u\n", word, t, m);

        //here to update encode->f_d by cache
        if((j != 0) && ((j % (PD_COUNT)) == 0))
        {
            replace_probability(encode->f_d, tp_d);  
/*
printf("\n-----j = %u------#\n", j);
printf("\n#-----start tp_d------#\n");
print_p(tp_d, P_LENGTH);
printf("\n#-----end tp_d------#\n");
printf("\n#-----start encode->f_d------#\n");
print_p(encode->f_d, P_LENGTH + 1);
printf("\n#-----end encode->f_d------#\n");
*/
            for(int i = 0; i < P_LENGTH; i++)
                tp_d[i] = 1;
        }
    
        fd_l = 0;
        fd_u = 0;
        while(m >= encode->f_d[k])
        {
            if(encode->f_d[k] != -1)
                fd_l = k;
            k++;
        }
        fd_u = k;

printf("%c", (char)fd_l);
//printf("k = %u, encode->f_d[k] = %u\n", k, encode->f_d[k]);


        ll = l + floor((u - l + 1) * encode->f_d[fd_l] / T);
        uu = l + floor((u - l + 1) * encode->f_d[fd_u] / T) - 1;
        tt = t;
        k = 0;

        //here to update cache probability table
        update_probability(tp_d, fd_l);

//printf("fd_l = %u, fd_u = %u, ll = %u  uu = %u\n",fd_l, fd_u, ll, uu);
        while(1)
        {
            if(ll >= R/2)
            {
                ll = 2 * ll - R ;
                uu = 2 * uu - R + 1;

                tt = 2 * tt - R;
                if(get_bit(encode, i, encoded_fd) == 1)
                    tt += 1;
                i++;

                
//printf("upper ll = %u, uu = %u\n", ll, uu);
            }
            else if(uu < R/2)
            {
                ll = 2 * ll;
                uu = 2 * uu + 1;

                tt = 2 * tt;
                if(get_bit(encode, i, encoded_fd) == 1)
                    tt += 1;
                i++;
//printf("down ll = %u, uu = %u\n", ll, uu);
            }
            else if((ll >= R/4) && (uu < 3*R/4))
            {
                ll = 2 * ll - R/2;           
                uu = 2 * uu - R/2 + 1;

                tt = 2 * tt - R/2;
                if(get_bit(encode, i, encoded_fd) == 1)
                    tt += 1;
                i++;
//printf("middle ll = %u, uu = %u\n", ll, uu);
            }
            else
            {
                l = ll;
                u = uu;
                t = tt;
                break;
            }
        }

    }

}

/*
void get_probability_from_file(int fd, int * p_d)
{
    char buf[INIT_LENGTH + 1];
    unsigned int ret_len, index;
    while( (ret_len = read(fd, buf, INIT_LENGTH)) > 0)
    {
        
        for(int i = 0; i < ret_len; i++)
        {
            index = (unsigned int)(unsigned char)buf[i];
            *(p_d + index) += 1;
        }
    
    }
}
*/

void get_probability_from_file(int fd, int * p_d)
{
    char buf[ PD_COUNT + 1];
    unsigned int ret_len, index;

    if(  (ret_len = read(fd, buf, PD_COUNT)) <= 0 )
    {
        if(ret_len == 0)
        {
            printf("get_probability_from_file : file is zero\n");
            exit(1);
        }
        else
        {
            printf("get_probability_from file : read fail\n");
            exit(1);
        }
    }


   for(int i = 0; i < ret_len; i++)
   {
        index = (unsigned int)(unsigned char)buf[i];
        *(p_d + index) += 1;
   }

}


/*
int main(int argc, char **argv)
{

    int p_d[P_LENGTH];
    int decoded_fd;
    struct stat original_file_status;
    char * decoded_file;

    if(argc != 3)
    {
        printf("Usage : arithmetic_code <-e|-d> filepath\n");
        return 1;
    }
    char * original_file_name = argv[2];

    if(strcmp(argv[1], "-e") == 0)
    {

        char * original_file_name = argv[2];

        if( (decoded_fd = open(original_file_name, O_RDONLY))  == -1 )
        {
            printf("open %s failed\n", original_file_name);
            return 1;
        }

        if( (fstat(decoded_fd, &original_file_status)) == -1 )
        {
            printf("fstat %s failed\n", original_file_name);
            return 1;
        }

        for(int i = 0; i < P_LENGTH; i++)
            p_d[i] = 1;

        get_probability_from_file(decoded_fd, p_d);
        if(lseek(decoded_fd, 0, SEEK_SET) == -1)
        {
        
            printf("lseek failed\n");
            return 1;
        }

        printf("#-----------start p_d---------------#\n");
        print_p(p_d, P_LENGTH);
        printf("\n");
        printf("#-----------end p_d-----------------#\n");
        
        Code encode;
        Text text;

        encode.len = INIT_LENGTH;
        encode.fill_len = 0;
        encode.bit_len = 0;
        if( (encode.content = (char *)malloc(sizeof(char) * INIT_LENGTH)) == NULL )
        {
            printf("malloc memery failed\n");
            exit(1);
        }
        memset(encode.content, 0, INIT_LENGTH);

        int sum = 0;
        for(int i = 0; i < P_LENGTH; i++)
        {
            encode.f_d[i] = sum;
            sum += p_d[i];
        }
        encode.f_d[P_LENGTH] = sum;

        print_p(encode.f_d, P_LENGTH + 1);
        printf("\n");


        text.len = original_file_status.st_size;
        encode.text_len = text.len;
        text.read_len = 0;
        text.original_fd = decoded_fd;
        if( (text.content = (char *)malloc(sizeof(char) * (INIT_LENGTH + 1))) == NULL )
        {
            printf("text.content malloc memery failed\n");
            exit(1);
        }
        memset(text.content, 0, INIT_LENGTH);

        char * compress_file_name;
        int compress_file_name_len;
        compress_file_name_len = 
                strlen(original_file_name) + strlen(COMPRESS_SUFFIX) + 1;
        if( (compress_file_name = 
            (char *)malloc(sizeof(char) * (compress_file_name_len))) == NULL )
        {
            printf("text.compress_file_name malloc memery failed\n");
            exit(1);
        }
        memset(compress_file_name, 0, compress_file_name_len);
        strcat(compress_file_name, original_file_name);
        strcat(compress_file_name, COMPRESS_SUFFIX);
        if( (text.encode_fd = open(compress_file_name, O_WRONLY | O_CREAT | O_TRUNC))  == -1 )
        {
            printf("text.encode_fd open %s failed\n", argv[1]);
            return 1;
        }

    //-----------encode-text----------
    encode_text(&text, &encode);   
  }
  else if(strcmp(argv[1], "-d") == 0)
  {

    Code encode;
    char * compress_file_name;
    int encoded_fd;

    compress_file_name = argv[2];

    encode.len = 0;
    encode.fill_len = 0;
    encode.bit_len = 0;
    if( (encode.content = (char *)malloc(sizeof(char) * INIT_LENGTH)) == NULL )
    {
        printf("malloc memery failed\n");
        exit(1);
    }
    memset(encode.content, 0, INIT_LENGTH);

    if( (encoded_fd = open(compress_file_name, O_RDONLY))  == -1 )
    {
        printf("open %s failed\n", compress_file_name);
        return 1;
    }
//-----------decode-text-----------
    decode_text(&encode, encoded_fd);

  }
  else
  {
      printf("Usage : arithmetic_code <-e|-d> filepath\n");
  }


    return 0;
}
*/