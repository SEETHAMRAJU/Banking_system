
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/file.h>
#include<unistd.h>
#include "credential.h"


int main()
{
	char user[400] = "1",password[600]="1IC";
	int key=0;
	int fd = open("credential.dat",O_RDONLY);
	struct credential cre;
	strcpy(cre.username, user);
	strcpy(cre.password,password);
	cre.loc = 1;
	
	while(read(fd,&cre,sizeof(cre))){
		printf("%s %s %d\n",cre.username,cre.password,cre.loc);
	}
	//write(fd, &cre, sizeof(cre));
	close(fd);
	return 0;
}
