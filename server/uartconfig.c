#include <stdio.h>

void *thread_tty01(void *arguments)
{
	printf("\nIn thread connection_handler\n");
	FILE *file_ptr;
	int *newSocket = ((int *)arguments);
	char c; 
	printf("newSocket %d",*newSocket);
	
		file_ptr = fopen("/dev/ttyO1", "r");
		while(1)
		{
			c = fgetc(file_ptr);
			if (feof(file_ptr))
			{
				break;
			}
			send(*newSocket, &c, 1, 0);
		}


	fclose(file_ptr);
	printf("\nEXIT the connection handler\n");
	return NULL;
	
}

void *thread_tty04(void *arguments)
{
	printf("\nIn thread connection_handler\n");
	FILE *file_ptr;
	int *newSocket = ((int *)arguments);
	char c; 
	printf("newSocket %d",*newSocket);
	
		file_ptr = fopen("/dev/ttyO4", "r");
		while(1)
		{
			c = fgetc(file_ptr);
			if (feof(file_ptr))
			{
				break;
			}
			send(*newSocket, &c, 1, 0);
		}


	fclose(file_ptr);
	printf("\nEXIT the connection handler\n");
	return NULL;
}