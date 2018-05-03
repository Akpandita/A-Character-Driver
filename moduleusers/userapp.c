#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

#define DEVICE "/dev/myDevice"

int main()
{
	int i,fd,user;
	char ch,write_buff[100],read_buff[100],string[100000],string2[100000];

	fd=open(DEVICE,O_RDWR);
	FILE *fp2;

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
			scanf("%s",write_buff);
			/*fp2=fopen("userfile.txt","a");
			fprintf(fp2,"%s",write_buff);
			fclose(fp2);
			printf("...........");
			fp2=fopen("userfile.txt","r");
			while(!feof(fp2)){
				printf(",.......");
				fscanf(fp2,"%s",string2);
				strcat(string,string2);
			}
			fclose(fp2);*/
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