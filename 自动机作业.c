#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define ALPHABETLENGTH 53
#define GETMIN(x,y) ((x)<=(y)?(x):(y))
 

int IsSuffix(char *pattern,int k,int q,char a);//判定pattern的前k个字符是不是（pattern的前q个字符加上字符a组成的）字符串的后缀

void Create(int*** array,char *pattern);//创建自动机（二维数组），并且根据给定的pattern完成自动机的初始化

int DFAMatcher(char* T,int** array,char *pattern);//根据创建的自动机进行模式匹配，并返回模式在给定文本中第一次出现的结束位置

void Delete(int*** array,char *pattern);//在程序结束时，将创建的自动机（二维数组）进行销毁

int SearchChar(char a);//一个小函数，用来查找给定的字符a在预先设定的字母表中的位置

char alphabet[ALPHABETLENGTH]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";//预先设定的字母表，包括26个大小写的字母以及一个空格，共53个字符
 
/*
*通过函数来进行二维数组的分配，需要用到三重指针，传进去的是一个指针数组的地址，
*直接传指针数组的话会造成悬垂指针，数组的构建需要根据pattern来构建
*二维数组实际上就相当于自动机（DFA）了
*/
void Create(int*** array,char *pattern)
{
    
    int i,j,k;//临时变量
   
    int patternlength=strlen(pattern); //pattern的长度
    
    int x=strlen(pattern)+1;//二位数组的行数等于pattern中字符数加1
    
    int y=ALPHABETLENGTH;//二维数组的列数等于字母表中所有的字符个数，这里我采用的是26个小写字母加上26个大写字母
    //开始分配二维数组的空间，如果分配失败的话则要撤销已分配的单元。这里分两种情况，
    //一种是一开始就没有空间可分配，另一种是分配了一部分以后空间不足。
    *array=(int**)malloc(sizeof(int)*x);
    if(NULL==array)
    {
        fprintf(stderr,"\nspace is not enough!\n");
        return;
    }
    for(i=0; i<x; i++)
    {
        if(((*array)[i]=(int*)malloc(sizeof(int)*y))==NULL)
        {
            while(--i>=0)
            {
                free((*array)[i]);
            }
            free(*array);
            fprintf(stderr,"\nspace is not enough!\n");
            return;
        }
    }
    //下面开始初始化二维数组的自动机表了
    for(i=0; i<=patternlength; i++)
    {
        for(j=0; j<ALPHABETLENGTH; j++)
        {
            k=GETMIN(patternlength+1,i+2);
            do
            {
                --k;
 
            }
            while(k>0 && !IsSuffix(pattern,k,i,alphabet[j]));
            (*array)[i][j]=k;
        }
    }
    for(i=0; i<patternlength+1; i++)
    {
        for(j=0; j<ALPHABETLENGTH; j++)
        {
            printf("%d ",(*array)[i][j]);
        }
        printf("\n");
    }
}
 
//为了实现Pk是Pqa的后缀，k和q是字符数组P的下标表示数组P的前k和前q个字符，a是一个字符表示连接在字符串Pq后面
int IsSuffix(char *pattern,int k,int q,char a)
{
    int cmp;
    char Q[q+1];
    Q[q]=a;
    strncpy(Q,pattern,q);
    cmp=strncmp(pattern,Q+q-(k-1),k);
    if(cmp==0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
 
//查找字符变量a在字母表中的位置
int SearchChar(char a)
{
    int i=0;
    while(alphabet[i]!=a)
    {
        ++i;
    }
    if(i>(ALPHABETLENGTH-1))
    {
        i=-1;
    }
    return i;
}
//利用自动机进行匹配
int DFAMatcher(char* T,int** array,char *pattern)
{
    int i;
    int n=strlen(T);
    int m=strlen(pattern);
    int q=0;
    int position=0;
 
    for(i=0; i<n; i++)
    {
        position=SearchChar(T[i]);
        if(position<0)
        {
            fprintf(stderr,"字符[%c]不存在\n",T[i]);
            return -1;
        }
        q=array[q][position];
        if(q==m)
        {
            printf("find!\n");
            break;
        }
    }
    if(q!=m)
    {
        printf("unfind\n");
        i=-1;
    }
    return i;//如果匹配成功返回pattern在字符串的结束位置，否则返回-1；
}
//程序结束进行销毁二维数组
void Delete(int*** array,char *pattern)
{
    int i;
    int m=strlen(pattern);
    for(i=m; i>=0; i--)
    {
        free((*array)[i]);
    }
    free((*array));
}
 
int main(void)
{
    //char a[100]="defabcababacaghijkl";
    //char b[10]="ababaca";
    char a[100];
    char b[10];
    printf("输入总的字符串:"); 
	scanf("%s",a);
    printf("输入比对的字符串:"); 
	scanf("%s",b);
    
    int **array;
    int i;
    printf("开始构建自动机：\n");
    Create(&array,b);
    printf("自动机构建完毕!\n");
    int end=DFAMatcher(a,array,b);
    int first=end-strlen(b)+1;
    if(end>=0)
    {
        printf("输入字符串：%s\n",a);
        printf("模式：%s\n",b);
        printf("结果：\n");
        printf("%s\n",a);
        for(i=0; i<strlen(a); i++)
        {
            if(i==end || i==first)
            {
                printf("|");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\nEnd Position:%d",end);
    }
    else
    {
        printf("结果出错了！");
    }
    Delete(&array,b);
    return 1;
}