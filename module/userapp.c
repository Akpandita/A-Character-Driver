#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

#define DEVICE "/dev/akshaydevice"

int main()
{
	int i,fd;
	char ch,write_buff[100],read_buff[100];

	fd=open(DEVICE,O_RDWR);

	if(fd==-1)
	{
		printf("file %s either does not exist or has been locked by another process\n",DEVICE);
		exit(-1);
	}

	printf("r=read from device\nw=write to device\nenter command: ");
	scanf("%c",&ch);

	switch(ch){

		case 'w':
			printf("enter data: ");
			scanf("%[^\n]",write_buff);
			write(fd,write_buff,sizeof(write_buff));
			break;
		case 'r':
			read(fd,read_buff,sizeof(read_buff));
			printf("device: %s\n",read_buff);
			break;
		default:
			printf("command not recognized\n");
			break;
	}
	close(fd);
	return 0;
}