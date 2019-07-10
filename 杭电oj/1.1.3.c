#include <stdio.h>
#include <math.h>
int main()
{
	int a,b;
	while(1){
		scanf("%d%d",&a,&b);
		printf("%d\n",a+b );
		if (a==0&&b==0)break;
	}
	return 0;
}