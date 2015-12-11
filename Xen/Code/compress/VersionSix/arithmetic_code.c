#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define P_LENGTH 255
#define T_LENGTH 512
#define INIT_LENGTH 512
#define ADD_LENGTH  20
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

    unsigned int len;
    char * content;

} Text;


void print_p(double *p, int p_length)
{
    for(int i = 0; i < p_length; i++)
        printf("%f\t", p[i]);
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

printf("add_bit : bit = %d, fill_len = %d, bit_len = %d, len = %d\n", 
        bit, encode->fill_len, encode->bit_len, encode->len);

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

void encode_text(Text text, Code * encode)
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

printf("t_lenth = %d, T = %d, R = %d\n", text.len, T, R);

    l = 0;
    u = R - 1;

    for(int j = 0; j < text.len; j++)
    {
        ll = l;
        uu = u;


printf("before text = %c, fill_len = %d, ll = %d, uu = %d\n", 
    text.content[j], encode->fill_len, ll, uu);

        f_dl = (unsigned int)text.content[j];
        for(unsigned int i = f_dl + 1; i < P_LENGTH + 1; i++)
            if(encode->f_d[i] != -1)
            {
                f_du = i;
                break;
            }


printf("f_dl = %d, f_du = %d\n", encode->f_d[f_dl], encode->f_d[f_du]);

        ll = l + floor((u - l + 1) * encode->f_d[f_dl] / T);
        uu = l + floor((u - l + 1) * encode->f_d[f_du] / T) - 1;

printf("encode fill_len = %d, ll = %d, uu = %d\n", encode->fill_len, ll, uu);

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

printf("upper encode_len = %d, fill_len = %d ll = %d, uu = %d\n", 
        encode->len, encode->fill_len, ll, uu);
            }
            else if(uu < R/2)
            {
                add_bit(encode, 0);

                ll = 2 * ll;
                uu = 2 * uu + 1;
                while(m-- > 0)
                    add_bit(encode, 1);

printf("down encode_len = %d, fill_len = %d, ll = %d, uu = %d\n", 
        encode->len, encode->fill_len, ll, uu);
                m = 0;
            }
            else if((ll >= R/4) && (uu < 3*R/4))
            {
                ll = 2 * ll - R/2;           
                uu = 2 * uu - R/2 + 1;
                m++;
printf("middle encode_len = %d, fill_len = %d, ll = %d, uu = %d\n", 
        encode->len, encode->fill_len, ll, uu);
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

int main(int argc, char **argv)
{

    int p_d[P_LENGTH] = {0};
    for(int i= 0; i < P_LENGTH; i++)
    {
        if(i == (int)'a')
            p_d[i] = 40; 
        else if(i == (int)'b')
            p_d[i] = 1;
        else if(i == (int)'c')
            p_d[i] = 9;
    }

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

    if( (text.content = (char *)malloc(sizeof(char) * T_LENGTH)) == NULL )
    {
        printf("malloc memery failed\n");
        exit(1);
    }
    memset(encode.content, 0, T_LENGTH);
    text.content[0] = 'a';
    text.content[1] = 'c';
    text.content[2] = 'b';
    text.content[3] = 'a';
    text.content[4] = 'c';
    text.content[5] = 'b';
    text.len = 6;
    encode.text_len = 6;


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

//-----------encode-text----------
    encode_text(text, &encode);   
//-----------decode-text-----------
    decode_text(&encode);

    printf("\n");

    return 0;
}
