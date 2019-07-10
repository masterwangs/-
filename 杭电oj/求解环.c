#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
 
int eage_size;
int vertax_size;
char filename_eage[10000];
char filename_vertax[10000];
 
int** eage_set;
char** vertax_set;
int** adjacentMatrix;
int* visitedFlag;
 
typedef struct SequenceStack
{
	int* base;
	int* top;
	int stackSize;
}SequenceStack;
 
void readEageDataFromFile();
void readVertaxDataFromFile();
void createAdjacentMatrix();
void DFS(int); 
void DFSTraverse();
void initialVisitedFlagArray();
void printVisitedVertax(int); 
void setVisitedFlag(int,int); 
int firstAdjacentVertax(int); 
int nextAdjacentVertax(int,int);
 
void initializeSequenceStack(SequenceStack*);
void pop_stack(SequenceStack*, int*);
void push_stack(SequenceStack*, int);
void print_stack(SequenceStack);
int empty_stack(SequenceStack);
void clear_stack(SequenceStack*);
void test_stack();
 
int main(int argc, char* argv[])
{
	if( argc != 5 )
	{
		printf("\tThis algorithm require 3 parameters"
				"\n\t\t1:the size of eage"
				"\n\t\t2:the filename contain eage-data"
				"\n\t\t3:the size of vertax"
				"\n\t\t4:the filename contain vertax-data");
		exit(0);
	}
	eage_size = atoi(argv[1]);
	strcat(filename_eage, argv[2]);
	vertax_size = atoi(argv[3]);
	strcat(filename_vertax, argv[4]);
	printf("eage_size : %d, vertax_size : %d, filename-eage : %s, filename-vertax : %s\n", eage_size, vertax_size, filename_eage, filename_vertax);
	readEageDataFromFile();
	readVertaxDataFromFile();
	createAdjacentMatrix();
	DFSTraverse();
	//test_stack(); 
	return 0; 
}
 
void readEageDataFromFile()
{
	FILE* f_read;
	if( NULL == (f_read = fopen(filename_eage, "r")))
	{
		printf("open file(%s) error!\n", filename_eage);
		exit(0);
	}
	//create dynamic array for storing original data form file @filename
	eage_set = (int**)malloc(sizeof(int*) * (eage_size + 1));
	if( !eage_set )
	{
		printf("malloc error: eage_set**\n");
		exit(0);
	}
	int i;
	for( i = 1; i <= eage_size; i++ )
	{
		eage_set[i] = (int*)malloc(sizeof(int) * (2 + 1));
		if( !eage_set[i] )
		{
			printf("eage_set[%d] malloc error", i);
			exit(0);
		}
	}
	//read original data from file
	for( i = 1; i <= eage_size; i++ )
	{
		if( 2 != fscanf(f_read, "%d %d", &eage_set[i][1], &eage_set[i][2]))
		{
			printf("fscanf error： %d\n", i);
			exit(0);
		}
	}
	//test
	printf("\n show the origin data from file\n");
	for( i = 1; i <= eage_size; i++ )
	{
		printf("%d\t%d\n", eage_set[i][1], eage_set[i][2]);
	}
	printf("\n");
	//test END
}
 
void readVertaxDataFromFile()
{
	//create the dynamic array for saving vertax-set information
	vertax_set = (char**)malloc(sizeof(char*) * (vertax_size + 1));
	if( !vertax_set )
	{
		printf("vertax_set malloc error");
		exit(0);
	}
	int i;
	for( i = 1; i <= vertax_size; i++ )
	{
		vertax_set[i] = (char*)malloc(sizeof(char) * (20 + 1));
		if( !vertax_set[i] )
		{
			printf("vertax_set[%d] malloc error");
			exit(0);
		}
	}
 
	//open file
	FILE* f_read;
	if( NULL == (f_read = fopen(filename_vertax, "r")))
	{
		printf("open file(%s) error", filename_vertax);
		exit(0);
	}
 
	//read vertax-set information
	for( i = 1; i <= vertax_size; i++ )
	{
		if( 1 != fscanf(f_read, "%s ", vertax_set[i]) )
		{
			printf("fscanf vertax_set[%d] error", i);
			exit(0);
		}
	}
 
	//test
	for( i = 1; i <= vertax_size; i++ )
	{
		printf("%s\n", vertax_set[i]);
	}
	printf("\n");
	//test END
}
 
void createAdjacentMatrix()
{
	//create the dynamic array for saving adjcaent matrix
	adjacentMatrix = (int**)malloc(sizeof(int*) * (vertax_size + 1));
	if( !adjacentMatrix )
	{
		printf("adjacentMatrix** malloc error");
		exit(0);
	}
	int i;
	for( i = 1; i <= vertax_size; i++ )
	{
		adjacentMatrix[i] = (int*)malloc(sizeof(int) * (vertax_size + 1));
		if( !adjacentMatrix[i] )
		{
			printf("adjacentMatrix[%d] malloc error");
			exit(0);
		}
	}
	//initial the value of adjacentMatrix
	int j;
	for( i = 1; i <= vertax_size; i++ )
	{
		for( j = 1; j <= vertax_size; j++ )
		{
			adjacentMatrix[i][j] = 0;
		}
	}
 
	//set the value for adjacentMatrix 
	for( i = 1; i <= eage_size; i++ )
	{
		adjacentMatrix[eage_set[i][1]][eage_set[i][2]] = 1;
		adjacentMatrix[eage_set[i][2]][eage_set[i][1]] = 1;
	}
 
	//test
	printf("\n show the information about adjacent matrix: \n");
	for( i = 1; i <= vertax_size; i++ )
	{
		for( j = 1; j <= vertax_size; j++ )
		{
			printf("%d ", adjacentMatrix[i][j]);
		}
		printf("\n");
	}
	//test END
}
 
int loop_count;
int heap;
int innerStep = 0;
int temp;
int isRecall;
SequenceStack loop_stack;
int pop_value;
void DFS(int startVertax)
{
	setVisitedFlag(startVertax, 1);
	int nextVertax;
	push_stack(&loop_stack, startVertax);
	nextVertax = firstAdjacentVertax(startVertax);
	innerStep++;
	for( ; ; )
	{
		if( nextVertax != -1 )
		{
			if( visitedFlag[nextVertax] == 1 && nextVertax == heap && innerStep == 2 )
			{
				nextVertax = nextAdjacentVertax(startVertax, nextVertax);
				continue;
			}
			else if( visitedFlag[nextVertax] == 1 && nextVertax == heap && innerStep != 2 )
			{
				print_stack(loop_stack);
				nextVertax = nextAdjacentVertax(startVertax, nextVertax);
				continue;
			}
			else if( visitedFlag[nextVertax] == 0 )
			{
				DFS(nextVertax);
			}
			if( isRecall == 1 )
			{
				innerStep--;
				temp = nextVertax;
				nextVertax = nextAdjacentVertax(startVertax, nextVertax);
				pop_stack(&loop_stack, &pop_value);
				setVisitedFlag(temp, 0);
				isRecall = 0;
				continue;
			}
			nextVertax = nextAdjacentVertax(startVertax, nextVertax);
		}
		else if( nextVertax == -1 )
		{
			isRecall = 1;
			break;
		}
	}
}
void DFSTraverse()
{
	initialVisitedFlagArray();
	initializeSequenceStack(&loop_stack);
	int i;
	for( heap = 1; heap <= vertax_size; heap++ )
	{
		for( i = 1; i <= vertax_size; i++ )
		{
			visitedFlag[i] = 0;
		}
		/*
		printf("print the visitedFlag array: ");
		for( i = 1; i <= vertax_size; i++ )
		{
			printf("%d ", visitedFlag[i]);
		}
		printf("\n");
		*/
		if( visitedFlag[heap] == 0 )
		{
			printf("\n-------------------the loop start and end with %d----------------\n", heap);
			clear_stack(&loop_stack);
			innerStep = 0;
			//printf("isRecall : %d, findLoop : %d, hasOthers : %d\n", isRecall, findLoop, hasOthers);
			isRecall = 0;
			DFS(heap);
		}
	}
}
void initialVisitedFlagArray()
{
	visitedFlag = (int*)malloc(sizeof(int) * (vertax_size + 1));
	if( !visitedFlag )
	{
		printf("visitedFlag* malloc error");
		exit(0);
	}
	int i;
	for( i = 1; i <= vertax_size; i++ )
		visitedFlag[i] = 0;
}
void printVisitedVertax(int vertaxID)
{
	printf("visited: %d \n", vertaxID);
}
void setVisitedFlag(int vertaxID, int value)
{
	visitedFlag[vertaxID] = value;
}
int firstAdjacentVertax(int vertaxID)
{
	int i;
	for( i = 1; i <= vertax_size; i++ )
	{
		if( adjacentMatrix[vertaxID][i] == 1 )
			return i;
	}
	return -1;
}
int nextAdjacentVertax(int vertaxID, int nextVertaxID)
{
	int i;
	for( i = nextVertaxID + 1; i <= vertax_size; i++ )
	{
		if( adjacentMatrix[vertaxID][i] == 1 )
			return i;
	}
	return -1;
}
void initializeSequenceStack(SequenceStack* stack)
{
	stack->base = (int*)malloc(sizeof(int) * (vertax_size + 1));
	if( !stack->base )
	{
		printf("Sequence stack malloc error!\n");
		exit(0);
	}
	stack->top = stack->base;
	stack->stackSize = vertax_size;
}
void pop_stack(SequenceStack* stack, int* value)
{
	if( empty_stack(*stack) == 1 )
	{
		printf("stack is empty , can not to pop!\n");
		exit(0);
	}
	*value = *(--(stack->top));
}
void push_stack(SequenceStack* stack, int value)
{
	*(stack->top) = value;
	(stack->top)++;
}
int empty_stack(SequenceStack stack)
{
	return stack.top == stack.base ? 1 : 0;
}
void print_stack(SequenceStack stack)
{
	int temp = *(stack.base);
	while( stack.top != stack.base )
	{
		printf("%d->", *((stack.base)++));
	}
	printf("%d\n", temp);
}
void clear_stack(SequenceStack* stack)
{
	stack->top = stack->base;
}
 
 