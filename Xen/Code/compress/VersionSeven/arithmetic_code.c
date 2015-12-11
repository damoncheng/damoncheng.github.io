#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define P_LENGTH 255
#define T_LENGTH 512
#define INIT_LENGTH 1024
#define ADD_LENGTH  512
#define INTERVAL 4

typedef struct {

    unsigned int len;
    unsigned int fill_len;
    unsigned int bit_len;
    unsigned int text_len;
    int f_d[P_LENGTH + 1];
    char * content;

} Code;

typedef struct {

    unsigned long len;
    unsigned long read_len;
    unsigned offset;
    int original_fd;
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

}

void print_code(Code * encode)
{

printf("#------print code------#\n");

    int length = encode->fill_len * 8 + encode->bit_len;
    int byte, bit;

    for(int i = 0; i < length; i++)
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
    printf("length = %d\n", length);

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

void add_bit(Code *encode, int bit)
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
    if(encode->fill_len == encode->len)
        extend_content(encode);

}

int get_bit(Code * encode, int offset)
{
    unsigned int byte_offset, bit_offset;
    unsigned bit = 1;

    byte_offset = offset / 8;
    bit_offset = offset % 8;

    if(encode->content[byte_offset] & (bit << (7 - bit_offset)))
        return 1;

    return 0;
}

char get_byte_from_file(Text * text, unsigned long index)
{
    int len;
    char c;

    if(index >= text->read_len)
    {
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
    


    T = encode->f_d[P_LENGTH];
    while(R <= (T * INTERVAL))
        R = R << 1;

//printf("t_lenth = %d, T = %d, R = %d\n", text->len, T, R);

    l = 0;
    u = R - 1;

    for(unsigned long j = 0; j < text->len; j++)
    {
        ll = l;
        uu = u;



        //f_dl = (unsigned int)text.content[j];
        f_dl = (unsigned int)get_byte_from_file(text, j);
printf("before text = %c, fill_len = %d, ll = %d, uu = %d\n", 
    f_dl, encode->fill_len, ll, uu);
        for(unsigned int i = f_dl + 1; i < P_LENGTH + 1; i++)
            if(encode->f_d[i] != -1)
            {
                f_du = i;
                break;
            }


printf("f_dl = %d, f_du = %d\n", encode->f_d[f_dl], encode->f_d[f_du]);

        ll = l + floor((u - l + 1) * encode->f_d[f_dl] / T);
        uu = l + floor((u - l + 1) * encode->f_d[f_du] / T) - 1;

//printf("encode fill_len = %d, ll = %d, uu = %d\n", encode->fill_len, ll, uu);

        while(1)
        {
            if(ll >= R/2)
            {
                add_bit(encode, 1);
                    
                ll = 2 * ll - R ;
                uu = 2 * uu - R + 1;

                while(m-- > 0)
                    add_bit(encode, 0);

                m = 0;

//printf("upper encode_len = %d, fill_len = %d ll = %d, uu = %d\n", 
//        encode->len, encode->fill_len, ll, uu);
            }
            else if(uu < R/2)
            {
                add_bit(encode, 0);

                ll = 2 * ll;
                uu = 2 * uu + 1;
                while(m-- > 0)
                    add_bit(encode, 1);

//printf("down encode_len = %d, fill_len = %d, ll = %d, uu = %d\n", 
//        encode->len, encode->fill_len, ll, uu);
                m = 0;
            }
            else if((ll >= R/4) && (uu < 3*R/4))
            {
                ll = 2 * ll - R/2;           
                uu = 2 * uu - R/2 + 1;
                m++;
//printf("middle encode_len = %d, fill_len = %d, ll = %d, uu = %d\n", 
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

//printf("encode = %s\n", encode);
//printf("encode = %d\n", encode->content[1]);
print_code(encode);
    
}

void decode_text(Code * encode)
{

    unsigned int T, t = 0, tt;
    unsigned int fd_l, fd_u;
    unsigned int R = 1;
    unsigned int l, u, ll, uu;
    unsigned int k = 0;
    unsigned int bit = 1;
    int i = 0;
    int m;
    unsigned int word = 0;

printf("#---------docode text\n");

    T = encode->f_d[P_LENGTH];
    while(R <= (T * INTERVAL))
    {
        R = R << 1;
        word++;
    }

    l = 0;
    u = R - 1;
    
    //get first word bit from content
    for(i = 0; i < word; i++)
    {
        if(get_bit(encode, i) == 1)
        {
            t += bit << (word - i - 1);
        }
//printf("word = %d  t = %d\n", word, t);
    }


    for(int j = 0; j < encode->text_len; j++)
    {


        m = floor(((t - l + 1) * T - 1) / (u - l + 1));
//printf("word = %d  t = %d m = %d\n", word, t, m);
    
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
//printf("k = %d, encode->f_d[k] = %d\n", k, encode->f_d[k]);

        ll = l + floor((u - l + 1) * encode->f_d[fd_l] / T);
        uu = l + floor((u - l + 1) * encode->f_d[fd_u] / T) - 1;
        tt = t;
        k = 0;

//printf("fd_l = %d, fd_u = %d, ll = %d  uu = %d\n",fd_l, fd_u, ll, uu);
        while(1)
        {
            if(ll >= R/2)
            {
                ll = 2 * ll - R ;
                uu = 2 * uu - R + 1;

                tt = 2 * tt - R;
                if(get_bit(encode, i) == 1)
                    tt += 1;
                i++;

                
//printf("upper ll = %d, uu = %d\n", ll, uu);
            }
            else if(uu < R/2)
            {
                ll = 2 * ll;
                uu = 2 * uu + 1;

                tt = 2 * tt;
                if(get_bit(encode, i) == 1)
                    tt += 1;
                i++;
//printf("down ll = %d, uu = %d\n", ll, uu);
            }
            else if((ll >= R/4) && (uu < 3*R/4))
            {
                ll = 2 * ll - R/2;           
                uu = 2 * uu - R/2 + 1;

                tt = 2 * tt - R/2;
                if(get_bit(encode, i) == 1)
                    tt += 1;
                i++;
//printf("middle ll = %d, uu = %d\n", ll, uu);
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

void get_probability_from_file(int fd, int * p_d)
{
    char buf[INIT_LENGTH + 1];
    int ret_len, index;
    while( (ret_len = read(fd, buf, INIT_LENGTH)) > 0)
    {
        
        for(int i = 0; i < ret_len; i++)
        {
            index = (int)buf[i];
            *(p_d + index) += 1;
        }
    
    }
}

int main(int argc, char **argv)
{

    int p_d[P_LENGTH] = {0};
    int decoded_fd;
    struct stat original_file_status;
    char * decoded_file;

    if(argc != 2)
    {
        printf("Usage : arithmetic_code filepath\n");
        return 1;
    }

    if( (decoded_fd = open(argv[1], O_RDONLY))  == -1 )
    {
        printf("open %s failed\n", argv[1]);
        return 1;
    }

    if( (fstat(decoded_fd, &original_file_status)) == -1 )
    {
        printf("fstat %s failed\n", argv[1]);
        return 1;
    }

    get_probability_from_file(decoded_fd, p_d);
    if(lseek(decoded_fd, 0, SEEK_SET) == -1)
    {
    
        printf("lseek failed\n");
        return 1;
    }

    //print_p(p_d, P_LENGTH);
    
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
        if(p_d[i] != 0)
        {
            encode.f_d[i] = sum;
            sum += p_d[i];
        }
        else
        {
            encode.f_d[i] = -1;
        }
    }
    encode.f_d[P_LENGTH] = sum;

    text.len = original_file_status.st_size;
    encode.text_len = text.len;
    text.read_len = 0;
    text.original_fd = decoded_fd;
    if( (text.content = (char *)malloc(sizeof(char) * (INIT_LENGTH + 1))) == NULL )
    {
        printf("text malloc memery failed\n");
        exit(1);
    }
    memset(text.content, 0, INIT_LENGTH);

//-----------encode-text----------
    encode_text(&text, &encode);   
//-----------decode-text-----------
    decode_text(&encode);
    printf("\n");

    return 0;
}
