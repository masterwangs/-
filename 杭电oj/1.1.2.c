#include<stdio.h>
int main ()
{int m,t=1,a,b;
scanf("%d",&m);
while(t<=m)
{  scanf("%d%d",&a,&b);
    t++;
    printf("%d\n",a+b);
}
return 0;
}
/*#include <stdio.h>
int main(int argc, char const *argv[])
{
	int a[999]={0},b[999]={0},c[999]={0};
	int q,d=0,e,cc;
	scanf("%d",&q);
    for (int i = 0; i < q; ++i)
    {
    	int aa,bb;
    	scanf("%d%d",&aa,&bb);
    	a[d]=aa;
    	b[d]=bb;
    	c[d]=aa+bb;
    	d++;
    }
    for (int i = 0; i < q; ++i)
    {
    	int d=0;
    	printf("%d\n",c[d] );
    	d++;
    }
	return 0;
}*/