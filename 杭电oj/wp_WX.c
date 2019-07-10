#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <windows.h>
using namespace std;

#define MAX_INT 9999
#define tmax 40 //最大迭代次数
#define vars_n 100//变元数目；

typedef int vextype;
typedef int adxtype;
int cls_m;//子句数目
int w_l,w_s;//权值储存
 vector<vector<long> > Graph(vars_n, vector<long>(vars_n, MAX_INT));
 vector<vector<long> > Graph_To(vars_n, vector<long>(vars_n, MAX_INT));

typedef struct node
{ adxtype adjvex;
  int warn;
  int flag;
  struct node *next;
}edgenode;

typedef struct vnode
{ vextype vertex;
  edgenode *link;
  struct vnode *next;
}vexnode;

typedef struct vanode
{ vextype value;
  vextype H;
  vextype C;
  vextype var;
  struct vanode *next;
}varnode;

FILE *fpout,*fpin;



void Instance(double d)//创建实例
{
    int i=0;
	long x,y,z,x_w,y_w,end=0;
    int ps=0,qs=0;
    int m=400;//树节点个数
    int minDist = MAX_INT;

   // m=(8*Graph.size()*(Graph.size()-1)*(Graph.size()-2)*d)/(6*Graph.size()*Graph.size())+0.5;

    cls_m=(int)m;

    if((fpout=fopen("d:\\results\\inresult.txt","w"))==NULL)//打开读写文件，读入公式
	{
		printf("cannot open this file:\n");
		exit(0);
	}

    //srand(time(NULL));
    for(int i = 0; i < m; i++){
         for(int j = 0; j < m; j++){
         Graph[i][j]=rand()%vars_n+1;
           w_l=Graph[i][j];

         }
    }
      //srand(time(NULL));
      for(int i = 0; i <m; i++){
         for(int j = 0; j < m; j++){
         Graph_To[i][j]=rand()%vars_n+1;
           w_s= Graph_To[i][j];

         }
    }


    for(i=1;i<= cls_m;i++)
	{
		for(int h = 0; h < Graph.size(); h++){
           for(int r = 0; r < Graph_To.size(); r++){
			x=(abs(rand())%2==1)?(Graph[h][r]%(rand()%10+1)+1):-(Graph[h][r]%(rand()%10+1)+1);
			y=(abs(rand())%2==1)?(Graph_To[h][r]%(rand()%10+1)+1):-(Graph_To[h][r]%(rand()%10+1)+1);
			z=(abs(rand())%2==1)?1:0;
			if((abs(x)!=abs(y))&&(abs(x)!=abs(z))&&(abs(y)!=abs(z))) break;

		   }
		}
		    fprintf(fpout,"%d  %d  %d    %d\n",x,y,z,end);
            //printf("********************");
    }


	fclose(fpout);

}
vexnode *CreatAdjList()//创建factor graphs
{
	int i=1,j=1,k=1;
	adxtype x;
	adxtype x_w;
	edgenode *s=NULL,*r=NULL,*fr=NULL;
	vexnode *head=NULL,*h=NULL,*f=NULL;

	if((fpout=fopen("d:\\results\\inresult.txt","r"))==NULL)//打开读写文件，读入公式
	{
		printf("cannot open this file:\n");
		exit(0);
	}
	while(!feof(fpout)) //读入公式，创建公式的链式存储结构
	{

		fscanf(fpout,"%d",&x);
		if(x!=0)//存储文字
		{
			s=(edgenode *)malloc(sizeof(edgenode));
			s->adjvex=x;
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

vexnode *init(vexnode *head)
{
	int i,flag;
	edgenode *s,*s1,*s2;
	vexnode *h,*h1;
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
					if(h1->vertex!=h->vertex)
					{
						s1=h1->link;
						s2=h1->link;
						while(s1!=NULL)
						{
							if(abs(s1->adjvex)==abs(s->adjvex))
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
	int j,J,w=1,h1=0;
	int min=w_l<w_s?w_l:w_s;
	edgenode *s,*s1,*s2;
    vexnode *h,*p1;
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
		        s->warn=w*min;
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
	edgenode *s1,*s2,*r=NULL,*fr=NULL;
	vexnode *head1,*head2;
    vexnode *p1,*p2,*f=NULL;

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
		printf("converged\n");

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
    double d=0;
	int conver=0;
	vexnode *head;
	int instance;
	if((fpin=fopen("d:\\results\\outresult.txt","w"))==NULL)//打开写文件，写入运行结果
	{
		printf("cannot open this file:\n");
		exit(0);
	}

		for(instance=1;instance<=30;instance++)
		{   srand((int) time(NULL));
		    Sleep(1000);
			Instance(d);
			head=CreatAdjList();
			head=init(head);
			conver+=wp(head);

		}

		fprintf(fpin,"%f  %f\n",d,conver/100.0);
		conver=0;
		printf("--------------\n");

    fclose(fpin);
    return 0;
}
