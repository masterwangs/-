#include<stdio.h>
int main()
{
    int a,b;
    while(scanf("%x%x",&a,&b)!=EOF)//输入的为十六进制的数字
        printf("%d\n",a+b);//输出的为十进制数字
    return 0;

/*
eod 是end of file的缩写 
“！=EOF” 表示文件还没有结束
“=EOF” 表示到文件末尾了。
你在用函数读入文件数据的时候,
函数总会返回一个状态,是读取成功还是失败,
那么这个状态怎么表示呢,所以就约定俗成定义一个标识符表示这个状态,就有了EOF
*/
}
