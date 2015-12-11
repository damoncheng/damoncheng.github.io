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
    double l, u, ll, uu;
    int t;

    l = 0;
    u = 1;

    for(int j = 0; j < T_LENGTH; j++)
    {
        t = text[j];
        ll = l;
        uu = u;
        l = ll + (uu - ll) * f_d[t - 1];
        u = ll + (uu - ll) * f_d[t];
        //printf("fd[t] = %f l = %f  u = %f\n", f_d[t], l, u);
    }

    printf("l = %f, u = %f, result = %f\n", l, u, (l + u) / 2 );

    return (l + u) / 2;

}

void decode_text(double code, double * f_d)
{

    double l, u, ll, uu;
    double tll, tuu;
    int i, j;
    l = 0;
    u = 1;

    for(j = 0; j < T_LENGTH; j++)
    {
        ll = l;
        uu = u;
        for(i = 0; i < P_LENGTH; i++)
        {
            ll = l + (u - l) * f_d[i];
            uu = l + (u - l) * f_d[i + 1];
            //printf("uu = %f, ll = %f\n", uu, ll);
            if(ll <= code && code <= uu)
            {
                u = uu;
                l = ll;
                printf("%d", i+1);
                break;
            }
        }

        //printf("u = %f, l = %f\n", u, l);
    }
    printf("\n");

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

    //print_p(f_d, P_LENGTH + 1);
    code = encode_text(text, f_d);
    decode_text(code, f_d);

    return 0;
}
