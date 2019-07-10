#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define tmax 1000 //最大迭代次数
#define vars_n 100//变元数目；
typedef int vextype;
typedef int adxtype;
int cls_m;//子句数目，
typedef struct node
{ adxtype adjvex;
  //float warn;//改为int
  int warn; 
  int flag;
  struct node *next;
}edgenode;//边缘节点
typedef struct vnode
{ vextype vertex;
  edgenode *link;
  struct vnode *next;
}vexnode;//顶点表
typedef struct vanode
{ vextype value;
  float H;
  vextype C;
  vextype var;
  struct vanode *next;
}varnode;
vexnode *head=NULL;
FILE *fpout,*fpin;
vexnode *CreatAdjList()//创建factor graphs 因子图
{ 
	int i=1,j=1,k=1;
	adxtype x;
	edgenode *s=NULL,*r=NULL,*fr=NULL;
	vexnode *head=NULL,*h=NULL,*f=NULL;
	if((fpout=fopen("d:\\ws\\CBS_k3_n100_m403_b10_42.cnf","r"))==NULL)//打开读写文件，读入公式
	//if((fpout=fopen("d:\\ws\\cs.cnf","r"))==NULL)//打开读写文件，读入公式
	//fopen 
	{
		printf("cannot open this file:\n");
		exit(0);
	}
	while(!feof(fpout)) //读入公式，创建公式的链式存储结构
		//feof文件结束判断，文件结束返回0，不结束返回一个非零的值
	{
		fscanf(fpout,"%d",&x);
		
		if(x!=0)//存储文字
		{    
			s=(edgenode *)malloc(sizeof(edgenode));
			// malloc在程序运行中动态的分配内存、
			// 返回的是分配地址指针
			// edgenode *	指针s指向edgenode类型的数据	
			s->adjvex=x;
			//s->warn=0.5;//abs(rand())%2;//可修改赋值为[0,1]
			s->warn=abs(rand())%2;
			
			if(x>0) s->flag=1;
			else s->flag=-1;
			
			s->next=NULL;
			if(r==NULL) 
			{
				r=s;
				fr=s;
			}
			else 
			{
				r->next=s;
				r=s;
			}
		}
		else//存储子句
		{
			h=(vexnode *)malloc(sizeof(vexnode));
			h->link=NULL;
			h->next=NULL;
			h->vertex=i++;
			h->link=fr;
			r=NULL;
			if(f==NULL)
			{
				f=h;
				head=h;
			}
			else
			{
				f->next=h;
				f=h;
			}
		}
	}
	fclose(fpout);
	return head;
} 

vexnode *init(vexnode *head)//初始化
{
	int i,flag;
	edgenode *s=NULL,*s1=NULL,*s2=NULL;
	vexnode *h=NULL,*h1=NULL;
	h=head;
	while(h!=NULL)
	{
		s=h->link;
		while(s!=NULL)
		{
			if(h->link->next==NULL)
			{
				s->warn=1;
				for(h1=head;h1!=NULL;h1=h1->next)
				{
					if(h1->vertex!=h->vertex)//在同一个子句中
					{
						s1=h1->link;
						s2=h1->link;
						while(s1!=NULL)
						{
							if(abs(s1->adjvex)==abs(s->adjvex))//是否是同一个文字，如果不是s1查找下一个文字
							{
								if(s1->flag==s->flag)
								{
									s1->warn=1;
									while(s2!=NULL)
									{
										if(s1!=s2)
										{
											s2->warn=0;
										}
										s2=s2->next;
									}
									break;
								}
								else
								{
									s1->warn=0;
									break;
								}
							}
							s1=s1->next;
						}//while
					}//if
				}//for
			}//if
			s=s->next;
		}
		h=h->next;
	}
	for(i=1;i<=vars_n;i++)
	{
		flag=0;
		h=head;
		while(h!=NULL)
		{
			s=h->link;
			while(s!=NULL)
			{
				if(i==abs(s->adjvex)) 
				{
					flag++;
					h1=h;
				}
				s=s->next;
			}
			if(flag>=2) break;
			h=h->next;
		}//while
		if(flag==1)
		{
			s1=h1->link;
			while(s1!=NULL)
			{
				s1->warn=0;
				s1=s1->next;
			}

		}
	}
	return head;
} 

vexnode *wp_update(vexnode *head)//警示更新函数
{
	//float j,J,w=1,h1=0;//改为int
	int j,J,w=1,h1=0;//改为int
	edgenode *s=NULL,*s1=NULL,*s2=NULL;
    vexnode *h=NULL,*p1=NULL;
	h=head;

    while(h!=NULL)//计算警示信息
	{  
		s=h->link;
		while(s!=NULL)
		{
		    s1=h->link;
		    if(h->link->next!=NULL)
			{
			    while(s1!=NULL)//计算警示信息w
				{
					h1=0;
			        if(s1->flag>0) J=-1;
			        else J=1;
			        if(abs(s->adjvex)!=abs(s1->adjvex))
					{
			            for(p1=head;p1!=NULL;p1=p1->next)//计算h
						{
			                if(p1!=h)
							{
					                s2=p1->link;
					                while(s2!=NULL)
									{
						                if(abs(s2->adjvex)!=abs(s1->adjvex)) 
										    s2=s2->next;
										else break;
									}
                                    if((p1->link!=NULL)&&(s2!=NULL))
									{
										if(s2->flag>0)   h1+=s2->warn;
										if(s2->flag<0)   h1-=s2->warn;
									}
					                   
							}//if

						}//for
						j=h1*J;
			            if(j<=0) j=0;
			            else j=1;
			            w=w*j;
					}//if			        
					s1=s1->next;					
				}//while
		        s->warn=(float)(0.0*s->warn*s->warn-0.0*s->warn+1)*w;
		        //警示信息更新(a*s->warn*s->warn-a*s->warn+1)*
				w=1;
		        s=s->next;
			}//if
		    else 
			{ 
				s->warn=1; 
				break;
			}
		 	
		}//while
		h=h->next;	     
	}//while
	return head;
}


int wp(vexnode *h)//警示函数
{
	int t;
	int conver=0;
	int diff=0;
	edgenode *s1=NULL,*s2=NULL,*r=NULL,*fr=NULL,*s5=NULL,*s6=NULL;
	vexnode *head1=NULL,*head2=NULL;
    vexnode *p1=NULL,*p2=NULL,*f=NULL,*hcs=NULL;
	//备份
	p1=h;
	head1=h;
	while(p1!=NULL)
	{
		s1=p1->link;
		while(s1!=NULL)
		{
	 
			s2=(edgenode *)malloc(sizeof(edgenode));
			s2->adjvex=s1->adjvex;
			if(p1->link->next==NULL)
			{
				s2->warn=1;
			}
			else
			{
				s2->warn=s1->warn;
			}
			//s2->warn=s1->warn;
			s2->flag=s1->flag;
			s2->next=NULL;
			if(r==NULL) 
			{
				r=s2;
				fr=s2;
			}
			else 
			{
				r->next=s2;
				r=s2;
			}
			s1=s1->next;
		}//while

		p2=(vexnode *)malloc(sizeof(vexnode));
        p2->link=NULL;
        p2->next=NULL;
        p2->vertex=p1->vertex;
		p2->link=fr;
		r=NULL;
		
		if(f==NULL)
		{
			f=p2;
			head2=p2;
		}
		else
		{
			f->next=p2;
		    f=p2;
		}
		p1=p1->next;
	}//while
	

	for(t=1;t<tmax;t++)//求解警示信息
	{
		diff=0;
		head1=wp_update(h);
		hcs=head1;
		printf("第一次%d\n",t );
		//printf("\n");
		while(hcs!=NULL)
		{
			s5=hcs->link;
			while(s5!=NULL)
			{
				printf("%d",s5->warn );
				s5=s5->next;
			}
			//printf("\n");
			hcs=hcs->next;
		}
		h=head1;
		p1=head1;
		p2=head2;
		while(p1!=NULL)
		{
			s1=p1->link;
			s2=p2->link;
			while(s1!=NULL)
			{
			    if(s1->warn!=s2->warn)
				{
					diff=1;
					break;
				}
				s1=s1->next;
				s2=s2->next;
			}
			if(diff==1) break;
			p1=p1->next;
			p2=p2->next;
		}
		if(p1!=NULL)
		{
			p1=head1;
			p2=head2;

			while(p1!=NULL)
			{
				s1=p1->link;
				s2=p2->link;
				while(s1!=NULL)
				{
					s2->adjvex=s1->adjvex;
					s2->warn=s1->warn;
					s2->flag=s1->flag;
					s1=s1->next;
					s2=s2->next;
				}//while

				p2->vertex=p1->vertex;
				p1=p1->next;
				p2=p2->next;
			}//while
	        	        
		}
		else
		{
			break;
		}
	}//for
  
	if(t<tmax)
	{
		
		conver=1;
	}
	else
	{
		conver=0;
		printf("un-converged\n"); 
	
	}
     
	return conver;
}

int main()
{ 
	char B[100];
	int B_num;
    double d=0;
	int conver=0;
	int deci_num=0;//无法给出解的实例数
	//printf("位置1\n");
	//vexnode *head=NULL;
	int instance;//实例
	int rs=10;//输入的骨干集大小
	printf("文件读取完成\n");
	head=CreatAdjList();
	printf("创建图完成\n");
	head=init(head);
	printf("初始化完成\n");
	printf("开始计算： \n");
	conver+=wp(head);
	//printf("%d\n",conver );

}