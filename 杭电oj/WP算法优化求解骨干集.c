#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define tmax 1000 //最大迭代次数
#define vars_n 8//变元数目；
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
	if((fpout=fopen("d:\\ws\\cs.cnf","r"))==NULL)
	//if((fpout=fopen("d:\\ws\\CBS_k3_n100_m403_b10_8.cnf","r"))==NULL)//打开读写文件，读入公式
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
	edgenode *s1=NULL,*s2=NULL,*r=NULL,*fr=NULL;
	vexnode *head1=NULL,*head2=NULL;
    vexnode *p1=NULL,*p2=NULL,*f=NULL;

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

int wid(vexnode *head)
{
	int j,J=0;
	float h=0,h0=0,h1=0,h2=0;
	int flag,flag1,flag2,flag3=-1;
    int B_num=0;
	edgenode *s1=NULL,*s2=NULL,*s3=NULL;
    vexnode *p=NULL,*p1=NULL,*p2=NULL;
	varnode *v1=NULL,*f1=NULL,*r1=NULL;//指向H、c的链表

	p=head;
	
	for(j=1;j<=vars_n;j++)//创建变量存储单元
	{
		v1=(varnode *)malloc(sizeof(varnode));
		v1->H=0;
		v1->C=0;
		v1->value=-1;//标记变元赋值情况
		v1->var=j;
		v1->next=NULL;
		if(f1==NULL)
		{
			f1=v1;
			r1=v1;
		}
		else
		{
			r1->next=v1;
			r1=v1;
		}
	}
    
	
		flag=wp(head);
		flag1=0;
		flag2=0;
		int a=0;
		if(flag==1)//if convergence
		{
			for(j=1,r1=f1;j<=vars_n && r1!=NULL;j++,r1=r1->next)//计算腔域h及冲突值c
			{
				for(p1=p;p1!=NULL;p1=p1->next)
				{
					s1=p1->link;
	
					while(s1!=NULL)
					{
						if(abs(s1->adjvex)==j)
						{
							if(s1->flag>0) J=-1;
							else J=1;
							h+=(s1->warn)*J;
							if(s1->flag>0)
							{
								h1+=s1->warn;
							}
							if(s1->flag<0)
							{
								h2+=s1->warn;
							}
							break;
						}
						s1=s1->next;
					}//while
				
					J=0;
				}//for

				if(r1->H==0)
				{
					r1->H=-h;
					h0=h1*h2;
					if(h0>0)
					{
						r1->C=1;
					}
					else
					{
						r1->C=0;
					}
				}
				h=0;
				h0=0;
				h1=0;
				h2=0;
			}//for
			r1=f1;
			while(r1!=NULL)//判断是否存在冲突值
			{
				if(r1->C==1)
				{
					flag1=1;
					break;
				}
				r1=r1->next;
			}
			if(flag1==0)//对变量指配值及化简公式，不存在冲突域
			{
				//printf("333333\n");
				r1=f1;
				//int a=0;
				while(r1!=NULL)//直接对链表读取  取消图化简
				{
					if(r1->H>0){
						 B_num=B_num+1;
						 	//a=a+1;
					}
					if(r1->H<0){
						 B_num=B_num+1;
						 	//a=a+1;
					}
					if(r1->H==0)
					{
						a=a+1;
					}
					printf("x%d的  ",r1->var );
					printf("H值为 %f\n",r1->H);
					//printf("%d\n",B_num);
					//a=a+1;
					//printf("%d\n",a);
					r1=r1->next;
					
				}//while(r1)

			}//if(flag1)
			else
			{
				printf("unsat\n");
				//num=1;
				//break;
			}
        
		}//if(flag)
		else
		{
			printf("un-converged\n");//简化后得到的子公式WP可能不收敛
			//break;
		}

	
	return B_num;
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

	if((fpin=fopen("d:\\ws\\outresult.txt","w"))==NULL)//打开写文件，写入运行结果
	{
		printf("cannot open this fileaaa:\n");
		exit(0);
	}
	//printf("位置2\n");
	head=CreatAdjList();
	//printf("位置3\n");
	head=init(head);
	//printf("位置4\n");
	//conver+=wp(head);先不取值
	B_num=wid(head);//wid返回B_num值
	printf("骨干集的大小为 %d\n",B_num );
	//printf("位置5\n");
	fprintf(fpin,"%d  %d\n",rs,B_num);
    fclose(fpin);
}