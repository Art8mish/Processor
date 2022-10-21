#include <stdio.h>

struct str { int a; int b; };

int factorial( const int n)
{
    if ( n == 0 || n == 1 )
        return 1;

    return n * factorial( n - 1 );
}

int main(void)
{
    struct str s = { 0 };

    s = (struct str){ 0 };

    printf("%d\n", factorial( 6));

    return 0;
}