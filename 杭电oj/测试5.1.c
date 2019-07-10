// myRB.cpp : Defines the entry point for the console application.

#include "stdafx.h" 
#include "stdio.h" //换上的   
#include "stdlib.h"   
#include "math.h"
#include "time.h"

#define N 20 
#define d 11 
#define M 180 
#define MY_COUNT_MAX 1000    
#define epsilon 0.0001    
#define INSTANCE 100      
int Ngpair_number;        

typedef struct _CONSTRAINT_{	
	int var1;
	int var2;
	int Nogood[d][d]; 
}Constraint; 

typedef struct _BELIEF_{
	int var;
	int cons;
	double var_cons1[d],cons_var1[d],var_cons2[d],cons_var2[d];   //var_cons1,2变量向约束传递的信息，cons_var1,2约束向变量传递的信息
	struct _BELIEF_ *next,*last;
}Belief;      

typedef struct _VARIABLE_{
	int val;                          
	struct _BELIEF_ *belf; 
}Variable; 
                 
void instance();                    
void nogoods();                  
int Engy_function(struct _CONSTRAINT_ *cons,int val1,int val2);
bool test();                          

static Constraint *Cons_root;        
static Variable *Var_root;            
static bool flag=true;   //用于BP方程迭代：flag=true使用var_cons1,cons_var1; flag=false使用var_cons2,cons_var2
static bool Global_flag=true;

int main(int argc, char* argv[])
{
	FILE  *f1; 
	f1=fopen("result.txt", "w+");  
	srand(time(0));
    double p=0.0; 
	while (p<=0.0)
	{
		Ngpair_number=(int)((double)(p*d*d+0.5));
//		time_t t1,t2,start_time,ok_time;
		printf("p=%f\n",p);
//		time(&start_time);
//		printf("开始时间: %s\n" ,asctime(localtime( &start_time )));
		int i,j,l,k,n1,my_count; 
		int v3,v4,temp;
		int Othr_var,fix_var;
		int Val[N];  //变量赋值数组，变量i对应的值为val[i]
		int Free_engy[M];
		int i_varmin[N];
		int j_max[N];
		int var_flag[N],belf_flag[N];
		int Total,Sat_num=0;
		int BP_flag;    
//		int Bad_var,Re; 				
		double Cvar_cons,Sum; 
		double Vc_temp[d],Pr_var[d];
		double VVP[N][d]; 
		double Entropy_var[N],Entropy_min[N],Pr_max[N];
		Belief *belf_temp,*belf_temp1;

		for(int Instance=0;Instance<INSTANCE;Instance++)
		{
			Var_root=(Variable*)malloc((N+1)*sizeof(Variable));     
			Cons_root=(Constraint*)malloc((M+1)*sizeof(Constraint));   
		
			for(i=0;i<=N;i++)
				Var_root[i].belf=NULL;		
			instance();  
			nogoods();   		
			
			for(i=0;i<N;i++) 
			{
				belf_temp=Var_root[i].belf;
				if(belf_temp==NULL)
					continue;
				else
				{
					while(belf_temp!=NULL) 
					{
						for(j=0;j<d;j++)
						{
							belf_temp->cons_var2[j]=(double)rand()/RAND_MAX;
						}		
						belf_temp=belf_temp->next; 
					}
				}
			}
			
			for(i=0;i<N;i++)
			{
				belf_temp=Var_root[i].belf; 
				if(belf_temp==NULL)     
					continue;							
				if(belf_temp->next==NULL)  
				{
					for(j=0;j<d;j++)
						belf_temp->var_cons2[j]=1.0/d;
				}			
				else 
				{
					while(belf_temp!=NULL)
					{
						Cvar_cons=0.0;
						for(j=0;j<d;j++)
							Vc_temp[j]=1.0;
						belf_temp1=Var_root[i].belf;					
						while(belf_temp1!=NULL)
						{	
							for(j=0;j<d;j++)
							{
								if(belf_temp1->cons!=belf_temp->cons)  
									Vc_temp[j]*=belf_temp1->cons_var2[j]; 
							}
							belf_temp1=belf_temp1->next;
						}
						for(j=0;j<d;j++)
							Cvar_cons+=Vc_temp[j];
						for(j=0;j<d;j++)
						{
							belf_temp->var_cons2[j]=Vc_temp[j]/Cvar_cons; 
//							printf("%f\n",belf_temp->var_cons2[j]);
						}
						belf_temp=belf_temp->next; 
					}
				}
			}
			
			for(i=0;i<N;i++)
			{
				belf_temp=Var_root[i].belf;
				if(belf_temp==NULL) 
					continue;
				else
				{
					while(belf_temp!=NULL) 
					{
					    
						if(Cons_root[belf_temp->cons].var1==i)
							Othr_var=Cons_root[belf_temp->cons].var2;
						else
							Othr_var=Cons_root[belf_temp->cons].var1;
						belf_temp1=Var_root[Othr_var].belf;
						while(belf_temp1!=NULL)
						{
							if(belf_temp1->cons==belf_temp->cons)
								break;
							else
								belf_temp1=belf_temp1->next;
						}
						for(j=0;j<d;j++)
						{
							Sum=0.0;
							for(l=0;l<d;l++)
							{
								if(i<Othr_var)
									Sum+=(double)Engy_function(Cons_root+belf_temp->cons,j,l)*belf_temp1->var_cons2[l];
								else
									Sum+=(double)Engy_function(Cons_root+belf_temp->cons,l,j)*belf_temp1->var_cons2[l];								
							}
							belf_temp->cons_var2[j]=Sum;
//							printf("%f\n",belf_temp->cons_var2[j]);					
						} 
						belf_temp=belf_temp->next;
					}			
				}
			}
			
			flag=true;
			my_count=0;
			while(my_count<MY_COUNT_MAX) 
			{
				if(flag)  
				{
					
					for(i=0;i<N;i++)
					{
						belf_temp=Var_root[i].belf;
						if(belf_temp==NULL)
							continue;					
						if(belf_temp->next==NULL) 	
						{
							for(j=0;j<d;j++)
								belf_temp->var_cons1[j]=belf_temp->var_cons2[j];
						}
						else
						{
							while(belf_temp!=NULL)   
							{
								Cvar_cons=0.0;
								for(j=0;j<d;j++)
									Vc_temp[j]=1.0; 
								belf_temp1=Var_root[i].belf;
								while(belf_temp1!=NULL)
								{
									for(j=0;j<d;j++)
									{
										if(belf_temp1->cons!=belf_temp->cons)
										Vc_temp[j]*=belf_temp1->cons_var2[j];
									}
										belf_temp1=belf_temp1->next;	
								}
								for(j=0;j<d;j++)
									Cvar_cons+=Vc_temp[j];
								for(j=0;j<d;j++)
								{
									belf_temp->var_cons1[j]=Vc_temp[j]/Cvar_cons;
								}
								belf_temp=belf_temp->next;
							}					
						}
					}
					
					for(i=0;i<N;i++)
					{
						belf_temp=Var_root[i].belf;
						if(belf_temp==NULL) 
							continue;
						else
						{
							while(belf_temp!=NULL) 
							{
								
								if(Cons_root[belf_temp->cons].var1==i)
									Othr_var=Cons_root[belf_temp->cons].var2;
								else
									Othr_var=Cons_root[belf_temp->cons].var1;
								belf_temp1=Var_root[Othr_var].belf;
								while(belf_temp1!=NULL)
								{
									if(belf_temp1->cons==belf_temp->cons)
										break;
									else
										belf_temp1=belf_temp1->next;
								}
								for(j=0;j<d;j++)
								{
									Sum=0.0;
									for(l=0;l<d;l++)
									{
										if(i<Othr_var)
											Sum+=(double)Engy_function(Cons_root+belf_temp->cons,j,l)*belf_temp1->var_cons1[l];
										else
											Sum+=(double)Engy_function(Cons_root+belf_temp->cons,l,j)*belf_temp1->var_cons1[l];								
									}
									belf_temp->cons_var1[j]=Sum;
//							        printf("%f\n",belf_temp->cons_var2[j]);					
								} 
								belf_temp=belf_temp->next;
							}
						}
					}
					flag=false;  
				}
				else 
				{
				
					for(i=0;i<N;i++)
					{
						belf_temp=Var_root[i].belf;
						if(belf_temp==NULL) 
							continue;
						if(belf_temp->next==NULL) 
						{
							for(j=0;j<d;j++)
							{
								belf_temp->var_cons2[j]=belf_temp->var_cons1[j];
							}	
						}
						else
						{
							while(belf_temp!=NULL)
							{
								Cvar_cons=0.0;
								for(j=0;j<d;j++)
									Vc_temp[j]=1.0;

								belf_temp1=Var_root[i].belf;
								while(belf_temp1!=NULL)
								{
									for(j=0;j<d;j++)
									{
										if(belf_temp1->cons!=belf_temp->cons)
											Vc_temp[j]*=belf_temp1->cons_var1[j];
									}	
									belf_temp1=belf_temp1->next;
								}
								for(j=0;j<d;j++)
								{
									Cvar_cons+=Vc_temp[j];
								}
								for(j=0;j<d;j++)
								{
									belf_temp->var_cons2[j]=Vc_temp[j]/Cvar_cons;
								}
								belf_temp=belf_temp->next;
							}
						}						
					}
					
					for(i=0;i<N;i++)
					{
						belf_temp=Var_root[i].belf;
						if(belf_temp==NULL) 
							continue;
						else
						{
							while(belf_temp!=NULL) 
							{
								
								if(Cons_root[belf_temp->cons].var1==i)
									Othr_var=Cons_root[belf_temp->cons].var2;
								else
									Othr_var=Cons_root[belf_temp->cons].var1;
								belf_temp1=Var_root[Othr_var].belf;
								while(belf_temp1!=NULL)
								{
									if(belf_temp1->cons==belf_temp->cons)
										break;
									else
										belf_temp1=belf_temp1->next;
								}
								for(j=0;j<d;j++)
								{
									Sum=0.0;
									for(l=0;l<d;l++)
									{
										if(i<Othr_var)
											Sum+=(double)Engy_function(Cons_root+belf_temp->cons,j,l)*belf_temp1->var_cons2[l];
										else
											Sum+=(double)Engy_function(Cons_root+belf_temp->cons,l,j)*belf_temp1->var_cons2[l];								
									}
									belf_temp->cons_var2[j]=Sum;
//							        printf("%f\n",belf_temp->cons_var2[j]);					
								} 
								belf_temp=belf_temp->next;
							}			
						}
					}
					flag=true; 
				}
				my_count++;
				if(my_count==MY_COUNT_MAX)          
					BP_flag=1;              	
				if(test())               
				{
					BP_flag=0;
					break;  
				}							
			} 
			if(BP_flag)
			{
				Global_flag=false;
				printf("不收敛\n");
			}
			else 
			{
				
				for(i=0;i<N;i++)
				{
					belf_temp=Var_root[i].belf;
					if(belf_temp==NULL) 
						continue;
					else
					{
						for(j=0;j<d;j++)
							Pr_var[j]=1.0; 
						while(belf_temp!=NULL)
						{
							for(j=0;j<d;j++)
							{
								Pr_var[j]*=belf_temp->cons_var2[j];
							}
							belf_temp=belf_temp->next;
						}					
						for(j=0;j<d;j++)
						{
							VVP[i][j]=Pr_var[j];
//							printf("VVP[%d][%d]%f\n",i,j,VVP[i][j]);
						}
					}
				}

				
				n1=0;
				Entropy_min[n1]=log(d);
				i_varmin[n1]=0;
				for(i=0;i<N;i++)
				{
					Entropy_var[i]=0.0;
					for(j=0;j<d;j++)
					{
						Entropy_var[i]+=-VVP[i][j]*log(VVP[i][j]);
					}
					if(Entropy_var[i]<Entropy_min[n1])
					{
						Entropy_min[n1]=Entropy_var[i];
						i_varmin[n1]=i;
					}					
				}

			
				Pr_max[n1]=0.0;
				j_max[n1]=0;
                for(j=0;j<d;j++)
				{
					if(VVP[i_varmin[n1]][j]>Pr_max[n1])
					{
						Pr_max[n1]=VVP[i_varmin[n1]][j];
						j_max[n1]=j;
					}
					Val[i_varmin[n1]]=j_max[n1];  
				}
			
				
				for(n1=1;n1<N;n1++)
				{
				
					for(i=0;i<N;i++) 
					{
						belf_temp=Var_root[i].belf; 
						if(belf_temp==NULL)
							continue;
						for(k=0;k<n1;k++)
						{
							if(i==i_varmin[k])
							{
								var_flag[i]=0; 
								break;
							}
							else
								var_flag[i]=1;
						}
						if(!var_flag[i])   
							continue;
						else
						{
							while(belf_temp!=NULL) 
							{
								for(k=0;k<n1;k++)
								{
									fix_var=i_varmin[k];
									if(i<fix_var)
									{
										if(Cons_root[belf_temp->cons].var1==i&&Cons_root[belf_temp->cons].var2==fix_var)
										{
											belf_flag[i]=0;
											break;
										}
										else
											belf_flag[i]=1;
									}
									else
									{
										if(Cons_root[belf_temp->cons].var2==i&&Cons_root[belf_temp->cons].var1==fix_var)
										{
											belf_flag[i]=0;
											break;
										}
										else
											belf_flag[i]=1;
									}
								}
								if(!belf_flag[i])   
								{
									for(j=0;j<d;j++)
									{
										if((i<fix_var&&Engy_function(Cons_root+belf_temp->cons,j,Val[fix_var])==1)||(i>fix_var&&Engy_function(Cons_root+belf_temp->cons,Val[fix_var],j)==1))
											belf_temp->cons_var2[j]=1.0;
										else
											belf_temp->cons_var2[j]=0.0;
									}
								}
								else  
								{
									for(j=0;j<d;j++)
									{
										belf_temp->cons_var2[j]=(double)rand()/RAND_MAX;
									}	
								}								
								belf_temp=belf_temp->next;
							}
						}						
					}
					
					for(i=0;i<N;i++)
					{
						belf_temp=Var_root[i].belf; 
						if(belf_temp==NULL)      
							continue;
						for(k=0;k<n1;k++)
						{
							if(i==i_varmin[k])
							{
								var_flag[i]=0; 
								break;
							}
							else
								var_flag[i]=1;
						}
						if(!var_flag[i])   
							continue;
						else
						{
							if(belf_temp->next==NULL)  
							{
								for(j=0;j<d;j++)
									belf_temp->var_cons2[j]=1.0/d;
							}
							else 
							{
								while(belf_temp!=NULL)
								{
									Cvar_cons=0.0;
									for(j=0;j<d;j++)
										Vc_temp[j]=1.0;
									belf_temp1=Var_root[i].belf; 					
									while(belf_temp1!=NULL)
									{	
										for(j=0;j<d;j++)
										{
											if(belf_temp1->cons!=belf_temp->cons) 
												Vc_temp[j]*=belf_temp1->cons_var2[j]; 
										}
										belf_temp1=belf_temp1->next;
									}
									for(j=0;j<d;j++)
										Cvar_cons+=Vc_temp[j];
									for(j=0;j<d;j++)
									{
										belf_temp->var_cons2[j]=Vc_temp[j]/Cvar_cons; 
									}

									belf_temp=belf_temp->next; 
								}
							}							
						}														
					}
				
					for(i=0;i<N;i++)
					{
						belf_temp=Var_root[i].belf;
						if(belf_temp==NULL) 
							continue;
						for(k=0;k<n1;k++)
						{
							if(i==i_varmin[k])
							{
								var_flag[i]=0; 
								break;
							}
							else
								var_flag[i]=1;
						}
						if(!var_flag[i])  
							continue;
						else
						{
							while(belf_temp!=NULL) 
							{
								for(k=0;k<n1;k++)
								{
									fix_var=i_varmin[k];
									if(i<fix_var)
									{
										if(Cons_root[belf_temp->cons].var1==i&&Cons_root[belf_temp->cons].var2==fix_var)
										{
											belf_flag[i]=0; 
											break;
										}
										else
											belf_flag[i]=1;
									}
									else
									{
										if(Cons_root[belf_temp->cons].var2==i&&Cons_root[belf_temp->cons].var1==fix_var)
										{
											belf_flag[i]=0;
											break;
										}
										else
											belf_flag[i]=1;
									}
								}
								if(!belf_flag[i])   
								{
									for(j=0;j<d;j++)
									{
										if((i<fix_var&&Engy_function(Cons_root+belf_temp->cons,j,Val[fix_var])==1)||(i>fix_var&&Engy_function(Cons_root+belf_temp->cons,Val[fix_var],j)==1))
											belf_temp->cons_var2[j]=1;
										else
											belf_temp->cons_var2[j]=0;
									}
								}
								else
								{
									
									if(Cons_root[belf_temp->cons].var1==i)
										Othr_var=Cons_root[belf_temp->cons].var2;
									else
										Othr_var=Cons_root[belf_temp->cons].var1;
									belf_temp1=Var_root[Othr_var].belf;
									while(belf_temp1!=NULL)
									{
										if(belf_temp1->cons==belf_temp->cons)
											break;
										else
											belf_temp1=belf_temp1->next;
									}
									for(j=0;j<d;j++)
									{
										Sum=0.0;
										for(l=0;l<d;l++)
										{
											if(i<Othr_var)
												Sum+=(double)Engy_function(Cons_root+belf_temp->cons,j,l)*belf_temp1->var_cons2[l];
											else
												Sum+=(double)Engy_function(Cons_root+belf_temp->cons,l,j)*belf_temp1->var_cons2[l];								
										}
										belf_temp->cons_var2[j]=Sum;
		//					            printf("%f\n",belf_temp->cons_var2[j]);					
									} 																
								}
								belf_temp=belf_temp->next;
							}			
						}
					}
					
					flag=true;
					my_count=0;
					while(my_count<MY_COUNT_MAX) 
					{
						if(flag)   
						{
						
							for(i=0;i<N;i++)
							{
								belf_temp=Var_root[i].belf; 
								if(belf_temp==NULL)      
									continue;
								for(k=0;k<n1;k++)
								{
									if(i==i_varmin[k])
									{
										var_flag[i]=0; 
										break;
									}
									else
										var_flag[i]=1;
								}
								if(!var_flag[i])  
									continue;
								else
								{
									if(belf_temp->next==NULL)  
									{
										for(j=0;j<d;j++)
											belf_temp->var_cons1[j]=belf_temp->var_cons2[j];
									}
									else 
									{
										while(belf_temp!=NULL)
										{
											Cvar_cons=0.0;
											for(j=0;j<d;j++)
												Vc_temp[j]=1.0;
											belf_temp1=Var_root[i].belf; 				
											while(belf_temp1!=NULL)
											{	
												for(j=0;j<d;j++)
												{
													if(belf_temp1->cons!=belf_temp->cons)  
														Vc_temp[j]*=belf_temp1->cons_var2[j]; 
												}
												belf_temp1=belf_temp1->next;
											}
											for(j=0;j<d;j++)
												Cvar_cons+=Vc_temp[j];
											for(j=0;j<d;j++)
											{
												belf_temp->var_cons1[j]=Vc_temp[j]/Cvar_cons; 
											}
											belf_temp=belf_temp->next; 
										}
									}							
								}														
							}
							
							for(i=0;i<N;i++)
							{
								belf_temp=Var_root[i].belf;
								if(belf_temp==NULL) 
									continue;
								for(k=0;k<n1;k++)
								{
									if(i==i_varmin[k])
									{
										var_flag[i]=0; 
										break;
									}
									else
										var_flag[i]=1;
								}
								if(!var_flag[i])   
									continue;
								else
								{
									while(belf_temp!=NULL) 
									{
										for(k=0;k<n1;k++)
										{
											fix_var=i_varmin[k];
											if(i<fix_var)
											{
												if(Cons_root[belf_temp->cons].var1==i&&Cons_root[belf_temp->cons].var2==fix_var)
												{
													belf_flag[i]=0; 
													break;
												}
												else
													belf_flag[i]=1;
											}
											else
											{
												if(Cons_root[belf_temp->cons].var2==i&&Cons_root[belf_temp->cons].var1==fix_var)
												{
													belf_flag[i]=0;
													break;
												}
												else
													belf_flag[i]=1;
											}
										}
										if(!belf_flag[i])  
										{
											for(j=0;j<d;j++)
											{
												belf_temp->cons_var1[j]=belf_temp->cons_var2[j];
											}
										}
										else
										{
											
											if(Cons_root[belf_temp->cons].var1==i)
												Othr_var=Cons_root[belf_temp->cons].var2;
											else
												Othr_var=Cons_root[belf_temp->cons].var1;
											belf_temp1=Var_root[Othr_var].belf;
											while(belf_temp1!=NULL)
											{
												if(belf_temp1->cons==belf_temp->cons)
													break;
												else
													belf_temp1=belf_temp1->next;
											}
											for(j=0;j<d;j++)
											{
												Sum=0.0;
												for(l=0;l<d;l++)
												{
													if(i<Othr_var)
														Sum+=(double)Engy_function(Cons_root+belf_temp->cons,j,l)*belf_temp1->var_cons1[l];
													else
														Sum+=(double)Engy_function(Cons_root+belf_temp->cons,l,j)*belf_temp1->var_cons1[l];								
												}
												belf_temp->cons_var1[j]=Sum;
//					                            printf("%f\n",belf_temp->cons_var2[j]);					
											} 																
										}
										belf_temp=belf_temp->next;
									}			
								}
							}
							flag=false;
						}
						else
						{
							
							for(i=0;i<N;i++)
							{
								belf_temp=Var_root[i].belf; 
								if(belf_temp==NULL)      
									continue;
								for(k=0;k<n1;k++)
								{
									if(i==i_varmin[k])
									{
										var_flag[i]=0; 
										break;
									}
									else
										var_flag[i]=1;
								}
								if(!var_flag[i])  
									continue;
								else
								{
									if(belf_temp->next==NULL)  
									{
										for(j=0;j<d;j++)
											belf_temp->var_cons2[j]=belf_temp->var_cons1[j];
									}
									else 
									{
										while(belf_temp!=NULL)
										{
											Cvar_cons=0.0;
											for(j=0;j<d;j++)
												Vc_temp[j]=1.0;
											belf_temp1=Var_root[i].belf; 				
											while(belf_temp1!=NULL)
											{	
												for(j=0;j<d;j++)
												{
													if(belf_temp1->cons!=belf_temp->cons)  
														Vc_temp[j]*=belf_temp1->cons_var1[j]; 
												}
												belf_temp1=belf_temp1->next;
											}
											for(j=0;j<d;j++)
												Cvar_cons+=Vc_temp[j];
											for(j=0;j<d;j++)
											{
												belf_temp->var_cons2[j]=Vc_temp[j]/Cvar_cons; 
											}
											belf_temp=belf_temp->next;
										}
									}							
								}														
							}
							
							for(i=0;i<N;i++)
							{
								belf_temp=Var_root[i].belf;
								if(belf_temp==NULL) 
									continue;
								for(k=0;k<n1;k++)
								{
									if(i==i_varmin[k])
									{
										var_flag[i]=0; 
										break;
									}
									else
										var_flag[i]=1;
								}
								if(!var_flag[i])   
									continue;
								else
								{
									while(belf_temp!=NULL) 
									{
										for(k=0;k<n1;k++)
										{
											fix_var=i_varmin[k];
											if(i<fix_var)
											{
												if(Cons_root[belf_temp->cons].var1==i&&Cons_root[belf_temp->cons].var2==fix_var)
												{
													belf_flag[i]=0; 
													break;
												}
												else
													belf_flag[i]=1;
											}
											else
											{
												if(Cons_root[belf_temp->cons].var2==i&&Cons_root[belf_temp->cons].var1==fix_var)
												{
													belf_flag[i]=0;
													break;
												}
												else
													belf_flag[i]=1;
											}
										}
										if(!belf_flag[i])  
										{
											for(j=0;j<d;j++)
											{
												belf_temp->cons_var2[j]=belf_temp->cons_var1[j];
											}
										}
										else
										{
											
											if(Cons_root[belf_temp->cons].var1==i)
												Othr_var=Cons_root[belf_temp->cons].var2;
											else
												Othr_var=Cons_root[belf_temp->cons].var1;
											belf_temp1=Var_root[Othr_var].belf;
											while(belf_temp1!=NULL)
											{
												if(belf_temp1->cons==belf_temp->cons)
													break;
												else
													belf_temp1=belf_temp1->next;
											}
											for(j=0;j<d;j++)
											{
												Sum=0.0;
												for(l=0;l<d;l++)
												{
													if(i<Othr_var)
														Sum+=(double)Engy_function(Cons_root+belf_temp->cons,j,l)*belf_temp1->var_cons2[l];
													else
														Sum+=(double)Engy_function(Cons_root+belf_temp->cons,l,j)*belf_temp1->var_cons2[l];								
												}
												belf_temp->cons_var2[j]=Sum;
				//					            printf("%f\n",belf_temp->cons_var2[j]);					
											} 																
										}
										belf_temp=belf_temp->next;
									}			
								}
							}
							flag=true;
						}													
						my_count++;
						if(my_count==MY_COUNT_MAX)          
							BP_flag=1;              	
						if(test())               
						{
							BP_flag=0; 
							break;  
						}	
					}
					if(BP_flag)
					{
						Global_flag=false;
						break;
//						printf("不收敛\n");
					}
					else
					{
					
						for(i=0;i<N;i++)
						{
							belf_temp=Var_root[i].belf;
							if(belf_temp==NULL) 
								continue;
							for(k=0;k<n1;k++)
							{
								if(i==i_varmin[k])
								{
									var_flag[i]=0; 
									break;
								}
								else
									var_flag[i]=1;
							}
							if(!var_flag[i])   
								continue;
							else
							{
								for(j=0;j<d;j++)
									Pr_var[j]=1.0; 
								while(belf_temp!=NULL)
								{
									for(j=0;j<d;j++)
									{
										Pr_var[j]*=belf_temp->cons_var2[j];
									}
									belf_temp=belf_temp->next;
								}					
								for(j=0;j<d;j++)
								{
									VVP[i][j]=Pr_var[j];
			//						printf("VVP[%d][%d]%f\n",i,j,VVP[i][j]);
								}
							}
						}
					
						Entropy_min[n1]=log(d);
                        i_varmin[n1]=0;
						for(i=0;i<N;i++)
						{
							for(k=0;k<n1;k++)
							{
								if(i==i_varmin[k])
								{
									var_flag[i]=0; 
									break;
								}
								else
									var_flag[i]=1;
							}
							if(!var_flag[i])  
								continue;
							else
							{
								Entropy_var[i]=0.0;
								for(j=0;j<d;j++)
								{
									Entropy_var[i]+=-VVP[i][j]*log(VVP[i][j]);
								}
								if(Entropy_var[i]<Entropy_min[n1])
								{
									Entropy_min[n1]=Entropy_var[i];
									i_varmin[n1]=i;
								}								
							}
						}

					
						Pr_max[n1]=0.0;
						j_max[n1]=0;
						for(j=0;j<d;j++)
						{
							if(VVP[i_varmin[n1]][j]>Pr_max[n1])
							{
								Pr_max[n1]=VVP[i_varmin[n1]][j];
								j_max[n1]=j;
							}
							Val[i_varmin[n1]]=j_max[n1];  
						}
						
//						printf("Val[%d]=%d\n",i_varmax[n1],Val[i_varmax[n1]]);
					}	
				}
//				for(i=0;i<N;i++)
//				{
//					printf("Val[%d]=%d\n",i,Val[i]);
//				}
				if(!Global_flag)
				{
					printf("不收敛\n");
				}
				else
				{
				
					for(i=0;i<M;i++)
					{
						v3=Cons_root[i].var1;
						v4=Cons_root[i].var2;
						if(v3>v4)
						{
							temp=v3;
							v3=v4;
							v4=temp;
						}
						Free_engy[i]=Engy_function(Cons_root+i,Val[v3],Val[v4]);
	//					printf("Free_engy[%d]=%d\n",i,Free_engy[i]);
					}
					Total=0;
					for(i=0;i<M;i++)
					{
						Total+=Free_engy[i];
					}
	//				printf("Total=%d\n",Total);
					if(Total==M)
					{
						Sat_num=Sat_num+1;
					}
					else
					{
						printf("不收敛\n");
					}

				}				
			}

			for (int in = 0 ;in <= N ;in++) 
			{
				Belief *temp = Var_root[in].belf;
				while (temp != NULL)
				{
					Belief *t1 = temp->next;
					free(temp);
					temp = t1;
				}
			}
			free(Var_root);
			free(Cons_root);			
		}
		fprintf(f1,"%f   %f\n",p,(float)Sat_num/INSTANCE);
		p=p+0.005;
	}

		fclose(f1);
		return 0;
}
//生成二元约束
void instance()
{
	int v1,v2,t;
	Belief *belf_temp, *belf_temp1;
          
	for(int i=0;i<M;i++)       
	{
		v1 = rand()%N;
		v2 = rand()%(N-1);
		v2=((v2>=v1)?(v2+1):v2); 
		if(v1>v2)
		{
			t=v1;
			v1=v2;
			v2=t;
		}
		Cons_root[i].var1=v1;   
		Cons_root[i].var2=v2;		
	
		belf_temp=(Belief*)malloc(sizeof(Belief)); 
		belf_temp->cons=i;
		belf_temp->var=v1;
		belf_temp->next=NULL;
		belf_temp1=Var_root[v1].belf;
		if(belf_temp1!=NULL) 
		{
			while(belf_temp1->next!=NULL)
				belf_temp1=belf_temp1->next;
			belf_temp1->next =belf_temp;
			belf_temp->last =belf_temp1;
		}
		else
			Var_root[v1].belf=belf_temp; 
		
		belf_temp=(Belief*)malloc(sizeof(Belief));  
		belf_temp->cons=i;
		belf_temp->var=v2;
		belf_temp->next=NULL;
		belf_temp1=Var_root[v2].belf;
		if(belf_temp1!=NULL)
		{
			while(belf_temp1!=NULL&&belf_temp1->next!=NULL)
				belf_temp1=belf_temp1->next;
			belf_temp1->next =belf_temp;
			belf_temp->last =belf_temp1;
		}
		else
			Var_root[v2].belf=belf_temp;
		
	}
}
//生成不协调赋值对
void nogoods()
{
	int v1,v2;
	int j,k,l;
	bool flag[d][d];
	
	for(int i=0; i<M; i++)
	{
		for(j=0;j<d;j++)
		{
			for(k=0;k<d;k++)
			{
				Cons_root[i].Nogood[j][k]=0;
			}
		}
		
		for(j=0;j<d;j++)
		{
			for(l=0;l<d;l++)
			{
				flag[j][l]=0;
			}
		}
		for(int n=0; n<Ngpair_number; n++)
		{
			v1=rand()%d;
			v2=rand()%d;					
			while(flag[v1][v2]==1)
			{
				v1=rand()%d;
				v2=rand()%d;
			}
			flag[v1][v2]=1;	
			Cons_root[i].Nogood[v1][v2]=1;
		}		
	}	
}
//能量函数，若赋值可满足则返回1，否则返回0。
int Engy_function(Constraint *cons, int val1, int val2)
{
	int r1;
	r1=(cons->Nogood[val1][val2]+1)%2;
	return r1;
}
//判断迭代是否收敛
bool test()
{
	int i,j;
	Belief *belf_temp;
	for(i=0;i<N;i++)
	{
		belf_temp=Var_root[i].belf;
		while(belf_temp!=NULL)
		{
			for(j=0;j<d;j++)
			{
				if(fabs(belf_temp->var_cons1[j]-belf_temp->var_cons2[j])>epsilon||fabs(belf_temp->cons_var1[j]-belf_temp->cons_var2[j])>epsilon)
					return false;
			}
			if(belf_temp!=NULL)
				belf_temp=belf_temp->next;
		}
	}
	return true;
}


