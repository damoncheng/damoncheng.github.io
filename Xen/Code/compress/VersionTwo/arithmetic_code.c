#include <stdio.h>
#define P_LENGTH 3
#define T_LENGTH 4

void print_p(double *p, int p_length)
{
    for(int i = 0; i < p_length; i++)
        printf("%f\t", p[i]);
}

double encode_text(int * text, double * f_d)
{
    double l, u;
    double ll, uu;
    double code = 0.0;
    double bit = 0.5;
    int index;

    l = 0;
    u = 1;

    for(int i = 0; i < T_LENGTH; i++)
    {
        index = text[i];
        ll = l + (u - l) * f_d[index - 1];
        uu = l + (u - l) * f_d[index];

        while((ll < 0.5 && uu < 0.5) || (ll >= 0.5 && uu >= 0.5))
        {

            if(ll < 0.5 && uu < 0.5)
            {
                uu = 2 * uu;
                ll = 2 * ll;
                bit = bit / 2;
            }

            if(ll >=0.5 && uu >= 0.5)
            {
                uu = 2 * (uu - 0.5);
                ll = 2 * (ll - 0.5);

                code += bit;
                bit = bit / 2;
            }
        }


        l = ll;
        u = uu;
    }

    code += bit;

    return code;

}

void decode_text(double code, double * f_d)
{
    double l, u;
    double ll, uu;
    int value;
    double bit = 0.5;

    l = 0;
    u = 1;

    for(int j = 0; j < T_LENGTH; j ++)
    {

        ll = l;
        uu = u;
//printf("before code = %f l = %f u = %f \n", code, l, u);
        while((ll < 0.5 && uu < 0.5) || (ll >= 0.5 && uu >= 0.5))
        {
        
            if(ll < 0.5 && uu < 0.5)
            {
                ll = 2 * ll;
                uu = 2 * uu;

                code *= 2;
        //printf("-code = %f\n", code);
            }

            if(ll >= 0.5 && uu >= 0.5)
            {
                ll = 2 * (ll - 0.5);
                uu = 2 * (uu - 0.5);

                code *= 2;
                code -= 1;
        //printf("+code = %f\n", code);
            }
        
        }

        l = ll;
        u = uu;

        //printf("code = %f l = %f u = %f \n", code, l, u);

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
    }

}


int main(int argc, char **argv)
{

    double p_d[P_LENGTH] = {0.8, 0.02, 0.18};
    double f_d[P_LENGTH + 1] = {0};
    int text[T_LENGTH] = {1, 3, 2, 1};
    double code;

    double sum = 0;
    for(int i = 0; i < P_LENGTH; i++)
    {
        f_d[i] = sum;
        sum += p_d[i];
    }
    f_d[P_LENGTH] = sum;

    code = encode_text(text, f_d);
    printf("%f\n",code);
    decode_text(code, f_d);
    printf("\n");
    

    return 0;
}
