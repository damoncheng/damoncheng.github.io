#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define P_LENGTH 3
#define T_LENGTH 4
#define TH_LENGTH 10
#define INIT_LENGTH 512
#define ADD_LENGTH  20
#define INTERVAL 4


void print_p(double *p, int p_length)
{
    for(int i = 0; i < p_length; i++)
        printf("%f\t", p[i]);
}

char * encode_text(int * text, int * f_d)
{
    unsigned int T;
    unsigned int R = 1;
    unsigned int l, u, ll, uu;
    unsigned int m = 0;
    char * encode = NULL;

    encode = (char *)malloc(sizeof(char) * 1024);
    memset(encode, 0, 1024);

    T = f_d[P_LENGTH];
    while(R <= (T * INTERVAL))
        R = R << 1;

printf("T = %d, R = %d\n", T, R);

    l = 0;
    u = R - 1;

    for(int j = 0; j < T_LENGTH; j++)
    {
        ll = l;
        uu = u;

printf("before code = %s, ll = %d, uu = %d\n", encode, ll, uu);

        ll = l + floor((u - l + 1) * f_d[text[j] - 1] / T);
        uu = l + floor((u - l + 1) * f_d[text[j]] / T) - 1;

printf("encode code = %s, ll = %d, uu = %d\n", encode, ll, uu);
        while(1)
        {
            if(ll >= R/2)
            {
                encode = strcat(encode, "1");
                ll = 2 * ll - R ;
                uu = 2 * uu - R + 1;

                while(m-- > 0)
                    encode = strcat(encode, "0");

                m = 0;
printf("upper code = %s, ll = %d, uu = %d\n", encode, ll, uu);
            }
            else if(uu < R/2)
            {
                encode = strcat(encode, "0");
                ll = 2 * ll;
                uu = 2 * uu + 1;
                while(m-- > 0)
                    encode = strcat(encode, "1");

printf("down code = %s, ll = %d, uu = %d\n", encode, ll, uu);
                m = 0;
            }
            else if((ll >= R/4) && (uu < 3*R/4))
            {
                ll = 2 * ll - R/2;           
                uu = 2 * uu - R/2 + 1;
                m++;
printf("middle code = %s, ll = %d, uu = %d\n", encode, ll, uu);
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
        encode = strcat(encode, "1");
        while(m-- > 0)
            encode = strcat(encode, "0");
        encode = strcat(encode, "0");
    }
    else
    {
        encode = strcat(encode, "0");
        while(m-- > 0)
            encode = strcat(encode, "1");
        encode = strcat(encode, "1");
    }

printf("encode = %s\n", encode);
    
    return encode;

}

void decode_text(char * encode, int * f_d)
{

    unsigned int T, t = 0, tt;
    unsigned int R = 1;
    unsigned int l, u, ll, uu, m;
    unsigned int k = 0;
    unsigned int bit = 1;
    int i = 0;
    unsigned int word = 0;

    T = f_d[P_LENGTH];
    while(R <= (T * INTERVAL))
    {
        R = R << 1;
        word++;
    }

    l = 0;
    u = R - 1;
    
    for(i = 0; i < word; i++)
    {
        if(encode[i] ==  '1')
        {
            t += bit << (word - i - 1);
        }
printf("word = %d  t = %d\n", word, t);
    }


    for(int j = 0; j < T_LENGTH; j++)
    {


        m = floor(((t - l + 1) * T - 1) / (u - l + 1));
printf("word = %d  t = %d m = %d\n", word, t, m);
    
        while(m >= f_d[k])
            k++;

        printf("%d", k);

        ll = l + floor((u - l + 1) * f_d[k - 1] / T);
        uu = l + floor((u - l + 1) * f_d[k] / T) - 1;
        tt = t;
        k = 0;

printf("ll = %d  uu = %d\n", ll, uu);
        while(1)
        {
            if(ll >= R/2)
            {
                ll = 2 * ll - R ;
                uu = 2 * uu - R + 1;

                tt = 2 * tt - R;
                if(encode[i] == '1')
                    tt += 1;
                i++;

                
printf("upper ll = %d, uu = %d\n", ll, uu);
            }
            else if(uu < R/2)
            {
                ll = 2 * ll;
                uu = 2 * uu + 1;

                tt = 2 * tt;
                if(encode[i] == '1')
                    tt += 1;
                i++;
printf("down ll = %d, uu = %d\n", ll, uu);
            }
            else if((ll >= R/4) && (uu < 3*R/4))
            {
                ll = 2 * ll - R/2;           
                uu = 2 * uu - R/2 + 1;

                tt = 2 * tt - R/2;
                if(encode[i] == '1')
                    tt += 1;
                i++;
printf("middle ll = %d, uu = %d\n", ll, uu);
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

    /*
    int p_d[P_LENGTH] = {40, 1, 9};
    int f_d[P_LENGTH + 1] = {0};
    int text[T_LENGTH] = {1, 3, 2, 1, 2, 3, 2, 1, 1, 1, 2, 2};
    */

    int p_d[P_LENGTH] = {1, 10, 40};
    int f_d[P_LENGTH + 1] = {0};
    int text[T_LENGTH] = {3, 1, 2, 3};

    char * encode;

    int sum = 0;
    for(int i = 0; i < P_LENGTH; i++)
    {
        f_d[i] = sum;
        sum += p_d[i];
    }
    f_d[P_LENGTH] = sum;

    encode = encode_text(text, f_d);   
    printf("encode is %s\n", encode);
    printf("length of encode is %d\n", strlen(encode));

    decode_text(encode, f_d);
    printf("\n");

    return 0;
}
