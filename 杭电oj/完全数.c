#include<stdio.h>
int main()
{
    int i, j, s, n;  /*变量i控制选定数范围，j控制除数范围，s记录累加因子之和*/
    printf("请输入所选范围上限：");
    scanf("%d", &n);  /* n的值由键盘输入*/
    for( i=2; i<=n; i++ )
    {
        s=0;  /*保证每次循环时s的初值为0*/
        
        for( j=1; j<i; j++ )
        {
            if(i%j == 0)  /*判断j是否为i的因子*/
                s += j;
        }
        if(s == i)  /*判断因子这和是否和原数相等*/
            printf("It's a perfect number:%d\n", i);
    }
    return 0;
}