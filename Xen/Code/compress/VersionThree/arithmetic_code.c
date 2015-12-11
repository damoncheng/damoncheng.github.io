#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define P_LENGTH 3
#define T_LENGTH 4
#define TH_LENGTH 10
#define INIT_LENGTH 512
#define ADD_LENGTH  20

typedef struct {

    unsigned int offset;
    char * code;

} Code;

void print_p(double *p, int p_length)
{
    for(int i = 0; i < p_length; i++)
        printf("%f\t", p[i]);
}

void update_offset(int * bit_offset, int * byte_offset)
{

    if(*bit_offset == 0)
    {
        *bit_offset = 7;
        (*byte_offset)++;
    }
    else
    {
        (*bit_offset)--;
    }

}

Code encode_text(int * text, double * f_d)
{
    double l, u;
    double ll, uu;
    Code scode;
    char * byte = NULL;
    unsigned int byte_offset = 0;
    char bit = 0x01;
    unsigned int bit_offset = 7;
    unsigned int m = 0;
    unsigned int index;

    l = 0;
    u = 1;
    scode.code = (char *)malloc(sizeof(char) * INIT_LENGTH);
    memset(scode.code, 0, INIT_LENGTH);
    byte = scode.code;

    for(int i = 0; i < T_LENGTH; i++)
    {

        //here own master, the scale will be reduced
        index = text[i];
        ll = l + (u - l) * f_d[index - 1];
        uu = l + (u - l) * f_d[index];

        //printf("#-----------------");
        printf("#----ll = %f, uu = %f\n", ll, uu);

        while((ll < 0.5 && uu < 0.5) || (ll >= 0.5 && uu >= 0.5) 
                || (ll >= 0.25 && (uu < 0.75)))
        {

            if((ll < 0.5) && (uu < 0.5))
            {
                uu = 2 * uu;
                ll = 2 * ll;
                printf("0, byte_offset = %d, bit_offset = %d\n", 
                        byte_offset, bit_offset);
                printf("    ll = %f, uu = %f\n", ll, uu);

                update_offset(&bit_offset, &byte_offset);
                for(int h = 0; h <  m; h++)
                {
                    *(byte + byte_offset) |= bit << bit_offset;           
                    update_offset(&bit_offset, &byte_offset);
                }

                m = 0;
            }
            else if((ll >=0.5) && (uu >= 0.5))
            {
                uu = 2 * (uu - 0.5);
                ll = 2 * (ll - 0.5);

                *(byte + byte_offset) |= bit << bit_offset;           
                printf("1, byte_offset = %d, bit_offset = %d\n", 
                        byte_offset, bit_offset);
                printf("    ll = %f, uu = %f\n", ll, uu);

                update_offset(&bit_offset, &byte_offset);
                for(int h = 0; h < m; h++)
                {
                    update_offset(&bit_offset, &byte_offset);
                }

                m = 0;
            }
            else if((ll >= 0.25) && (uu < 0.75))
            {
                uu = 2 * (uu - 0.25);
                ll = 2 * (ll - 0.25);

                printf("2, byte_offset = %d, bit_offset = %d\n", 
                        byte_offset, bit_offset);
                printf("    ll = %f, uu = %f\n", ll, uu);

                m++;
            }

        }


        l = ll;
        u = uu;
    }

/*
    if(l >= 0.25)
    {
    
        *(byte + byte_offset) |= bit << bit_offset;           
        update_offset(&bit_offset, &byte_offset);

        for(int h = 0; h < m; h++)
        {
            update_offset(&bit_offset, &byte_offset);
        }
    
    }
    else
    {

        update_offset(&bit_offset, &byte_offset);
        for(int h = 0; h <  m; h++)
        {
            *(byte + byte_offset) |= bit << bit_offset;           
            update_offset(&bit_offset, &byte_offset);
        }
    }
*/

    *(byte + byte_offset) |= bit << bit_offset;           
    scode.offset = byte_offset;

    return scode;

}

void decode_text(Code scode, double * f_d)
{
    double l, u;
    double ll, uu;
    double code = 0.0;
    unsigned int byte_offset = 0;
    unsigned int bit_offset = 7;
    unsigned int bit = 0x01;
    double fbit = 0.5;
    unsigned int m = 0;

    for(int i = 0; i < 6; i++)
    {
        if(scode.code[byte_offset] & (bit << bit_offset))
        {
            if(i == 0)
            {
                code += fbit;
            }
            else
            {
                code += fbit / (2 << (i - 1));
            }
        }

        bit_offset--;
    }

    printf("%f\n", code);

    l = 0;
    u = 1;

    for(int j = 0; j < T_LENGTH; j++)
    {

        ll = l;
        uu = u;
printf("before code = %f l = %f u = %f \n", code, l, u);
        while((ll < 0.5 && uu < 0.5) || (ll >= 0.5 && uu >= 0.5) 
                || (ll >= 0.25 && uu < 0.75))
        {
        
            if((ll < 0.5) && (uu < 0.5))
            {
                ll = 2 * ll;
                uu = 2 * uu;

                code *= 2;
                if(scode.code[byte_offset] & (bit << bit_offset))
                {
                    code += (fbit / (2 << 4));
                }

                update_offset(&bit_offset, &byte_offset);

                for(int h = 0; h < m; h++)
                {
                    if(scode.code[byte_offset] & (bit << bit_offset))
                    {
                        code += (fbit / (2 << 4));
                    }
                    update_offset(&bit_offset, &byte_offset);

                }

                m = 0;
printf("-code = %f ll = %f uu = %f \n", code, ll, uu);
            }
            else if((ll >= 0.5) && (uu >= 0.5))
            {
                ll = 2 * (ll - 0.5);
                uu = 2 * (uu - 0.5);

                code *= 2;
                code -= 1;
                if(scode.code[byte_offset] & (bit << bit_offset))
                {
                    code += (fbit / (2 << 4));
                }

                update_offset(&bit_offset, &byte_offset);

                printf("m = %d\n", m);
                for(int h = 0; h < m; h++)
                {
                    if(scode.code[byte_offset] & (bit << bit_offset))
                    {
                        code += (fbit / (2 << 4));
                    }

                    update_offset(&bit_offset, &byte_offset);
                }

                m = 0;
printf("+code = %f ll = %f uu = %f \n", code, ll, uu);
            }
            else if((ll >= 0.25) && (uu < 0.75))
            {
                ll = 2 * (ll - 0.25);
                uu = 2 * (uu - 0.25);
                m++; 
printf("=code = %f ll = %f uu = %f \n", code, ll, uu);
            }


        
        }

        l = ll;
        u = uu;

printf("middle code = %f l = %f u = %f \n", code, l, u);

        for(int i = 0; i < P_LENGTH + 1; i++)
        {
            ll = l + f_d[i - 1] * (u - l);
            uu = l + f_d[i] * (u - l);
            if(code > ll && code <= uu)
            {
                printf("%d", i);
                l = ll;
                u = uu;
                break;
            }
        }

printf("after code = %f l = %f u = %f \n", code, l, u);

    }

}


int main(int argc, char **argv)
{

    double p_d[P_LENGTH] = {0.8, 0.02, 0.18};
    double f_d[P_LENGTH + 1] = {0};
    int text[TH_LENGTH] = {1, 3, 2, 1, 2, 3, 3, 2, 1, 1};
    Code scode;

    double sum = 0;
    for(int i = 0; i < P_LENGTH; i++)
    {
        f_d[i] = sum;
        sum += p_d[i];
    }
    f_d[P_LENGTH] = sum;

    scode = encode_text(text, f_d);   
    printf("start = %x\n", scode.code[0]);
    printf("start = %x\n", scode.code[1]);
    printf("offset = %d\n", scode.offset);
    decode_text(scode, f_d);
    printf("\n");

    return 0;
}
