#include "test_FIFOthreads.h"
#include "threads.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int i;
	JobsTail FIFO;
	init_tail(&FIFO);
	Job* temp;
	for(i=0;i<10;i++)
	{
		tail_node* tempnode;
		temp=malloc(sizeof(Job));
		 push_tailnode(&FIFO,create_tailnode(temp));
		//printf("GreatSucc\n");
	}
	for(i=0;i<10;i++)
	{
		temp=destroy_tailnode(pop_tailnode(&FIFO));
		if(!temp){printf("Pop Returned NULL\n");exit(0);}else printf("Pop Returned %p\n",temp);
			print_tail(&FIFO);
	}
	for(i=0;i<10;i++)
	{
		tail_node* tempnode;
		temp=malloc(sizeof(Job));
		push_tailnode(&FIFO,create_tailnode(temp));
		print_tail(&FIFO);
	}
	for(i=0;i<5;i++)
	{
                temp=destroy_tailnode(pop_tailnode(&FIFO));
                if(!temp){printf("Pop Returned NULL\n");exit(0);}else printf("Pop Returned %p\n",temp);
                        print_tail(&FIFO);
		
	}
	        //temp=destroy_tailnode(pop_tailnode(&FIFO));
               // if(!temp){printf("Pop Returned NULL\n");exit(0);}else printf("Pop Returned %p\n",temp);
                 //       print_tail(&FIFO);
	for(i=0;i<5;i++)
	{
                tail_node* tempnode;   
                temp=malloc(sizeof(Job));
                push_tailnode(&FIFO,create_tailnode(temp));
                print_tail(&FIFO);
	//temp=malloc(sizeof(Job));
		//push_tail(&FIFO,temp);
		//print_tail(&FIFO);
		 //               temp=destroy_tailnode(pop_tailnode(&FIFO));
                //if(!temp){printf("Pop Returned NULL\n");exit(0);}else printf("Pop Returned %p\n",temp);
                  //      print_tail(&FIFO);

	}
	for(i=0;i<20;i++)
	{
                temp=destroy_tailnode(pop_tailnode(&FIFO));
                if(!temp){printf("Pop Returned NULL\n");exit(0);}else printf("Pop Returned %p\n",temp);
                        print_tail(&FIFO);

		//temp=pop_tail(&FIFO);
		//if(!temp){printf("Pop Returned NULL\n");exit(0);}else printf("Pop Returned %p\n",temp);
		//	print_tail(&FIFO);
	}
	
	
	
	
}
