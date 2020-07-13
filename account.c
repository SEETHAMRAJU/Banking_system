
#include<stdio.h>
#include<stdlib.h>
#include "account.h"
#include<sys/file.h>
#include<unistd.h>
#include<string.h>
#include "util.c"

#define LEAST_BALANCE 5
int temp[6]={-1,-1,-1,-1,-1,-1};



void split(char *data,int len)
{
	char t[500]="";int j=0;
	for(int i=0;i<len && j<6; i++)
	{
		if(data[i]==';')
		{
			if(strcmp(t,"")!=0){
				temp[j++] = char_to_num(t,strlen(t));
				if(temp[j-1]<-1)
					temp[j-1]=-1;
			}
			else
				temp[j++]=-1;
			strcpy(t, "");
		}
		else
			strncat(t,&data[i],1);
	}
	temp[j] = char_to_num(t,strlen(t));
	if(temp[j]<=0)
		temp[j]=-1;
// 400;0;
//	printf("temp %d %d",j,temp[j]);
//	printf("------\n");
}

void delete_account(int loc, int fd)
{
	lseek(fd, loc, SEEK_SET);

	struct Account acc;
	struct flock fl;
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = loc;
	fl.l_len = sizeof(struct Account);

	fcntl(fd, F_SETLKW, &fl);

	read(fd, &acc, sizeof(struct Account));
	acc.transactions=-1;
	lseek(fd, loc, SEEK_SET);
	write(fd, &acc, sizeof(struct Account));

	fl.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &fl);
}

int transact(int loc, int fd, char *money, int deposit)
{
	lseek(fd, loc,SEEK_SET);
	struct flock fl;
	fl.l_type = F_WRLCK;
	fl.l_start = loc;
	fl.l_whence = SEEK_SET;
	fl.l_len = sizeof(struct Account);

	struct Account acc;

	fcntl(fd,F_SETLKW,&fl);// try to put it at SETLKW

	read(fd,&acc,sizeof(struct Account));
	//deposit = True then the transaction is deposit amount else it means its withdraw
	if(deposit)
	{
		acc.balance+=char_to_num(money,strlen(money));
		acc.transactions++;
	}
	else
	{
		int mo = char_to_num(money,strlen(money));
		//Incase the account does not have balance
		if(mo>acc.balance-LEAST_BALANCE)
		{
			fl.l_type=F_UNLCK;
			fcntl(fd,F_SETLK, &fl);
			return -1;
		}
		acc.balance-=mo;
	}
	lseek(fd,loc,SEEK_SET);
	write(fd,&acc,sizeof(acc));
	fl.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&fl);	


	return acc.balance;
}

void getBalance(int fd, int loc,char temp[])
{
	lseek(fd,loc,SEEK_SET);
	struct Account acc;
	struct flock fl;
	fl.l_type = F_RDLCK;
	fl.l_whence=SEEK_SET;
	fl.l_start  = loc;
	fl.l_len = sizeof(struct Account);

	fcntl(fd, F_SETLKW, &fl);

	read(fd, &acc, sizeof(struct Account));

	sprintf(temp,"Your Account Balance : %d\n",acc.balance);

	fl.l_type = F_UNLCK;

	fcntl(fd, F_SETLK, &fl);


	return ;
}

int add_account(int fd,int acc_num, char *data, int len)
{
	//account.dat 
	struct Account acc;
	
	acc.ac_id = acc_num;
	
	temp[0]=-1,temp[2]=-1,temp[1]=-1;
	split(data,len);
	printf("%s %d %d %d\n",data,temp[0],temp[1],temp[2]);
	
	acc.balance = temp[0];
	acc.transactions = 0;
	acc.arr[0] = temp[1];
	acc.arr[1] = temp[2];
	
	if(acc.arr[0]!=-1)
		acc.type = 'S';
	if(acc.arr[1]>-1)
		acc.type = 'J';

	lseek(fd, 0, SEEK_END);
	temp[0]=-1;temp[1]=-1;temp[2]=-1;
	int res = write(fd, &acc, sizeof(acc));

	return res;
}

void modify_account(char converse[], int len, int fd, char tempi[], int loc)
{
	lseek(fd,loc, SEEK_SET);
	
	temp[0]=-1,temp[1]=-1,temp[2]=-1;
	split(converse, len);

	struct flock fl;
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = loc;
	fl.l_len = sizeof(struct Account);

	fcntl(fd, F_SETLKW, &fl);

	struct Account acc;
	read(fd, &acc,sizeof(struct Account));
	
	if(temp[0]>-1)
		acc.balance = temp[0];
	if(temp[1]>-1)
		acc.arr[0]=temp[1];
	if(temp[2]>-1)
		acc.arr[1]=temp[2];
	if(acc.arr[1]>-1 && acc.arr[0]>-1)
		acc.type = 'J';
	else
		acc.type = 'S';
	lseek(fd, loc, SEEK_SET);
	write(fd,&acc,sizeof(struct Account));
	fl.l_type = F_UNLCK;
	fcntl(fd,F_SETLK, &fl);
	strcpy(tempi,"Account modified");
}

void search_account(int loc, int fd, char temp[])
{
	if(loc==-1)
	{
		temp = "YOUR ACCOUNT DOESN'T EXIST";
		return ;
	}
	lseek(fd, loc ,SEEK_SET);

	struct flock fl;
	fl.l_type=F_RDLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = loc;
	fl.l_len = sizeof(struct Account);

	fcntl(fd, F_SETLKW,&fl );// this setlock wait might resutl in a dead lock situation.
	struct Account acc;
	int te = read(fd, &acc, sizeof(struct Account));
	if(acc.transactions==-1)
	{
		temp = "RECORD NOT FOUND";
		//sprintf(temp,"%s","RECORD NOT FOUND\n");
		return;
	}
	char p[500];
	sprintf(p, "ACCOUNT ID %d", acc.ac_id);
	strcpy(temp, p);

	strcat(temp, "\nTYPE : ");
	sprintf(p,"%c",acc.type);
	strcat(temp, p);

	strcat(temp, "\nBALANCE : ");
	sprintf(p, "%d\n", acc.balance);
	strcat(temp, p);

	strcat(temp,"ACCOUNT HOLDERS : ");
	sprintf(p,"%d\n",acc.arr[0]);
	if(acc.arr[1]>0)
		sprintf(p,"%d %d\n",acc.arr[0],acc.arr[1]);
	strcat(temp,p);
	fl.l_type=F_UNLCK;
	fcntl(fd, F_SETLK, &fl);
	//return temp;
}

/*
int main()
{
	int fd = open("temporary.dat",O_RDWR);


	char tempo[500],conve[600]="1999;100;";

	add_account(fd,1,conve,strlen(conve));
	strcpy(conve,"2999;2;4");
	add_account(fd,2,conve,strlen(conve));

	char p[500],m[500];
	search_account(0,fd,p);
	search_account(24,fd,m);
	printf("%s\n\n%s",p,m);
	char l[500]="10000;3;9";
	// modify_account(l,strlen(l),fd,p,0);

	search_account(0,fd,p);
	printf("%s\n",p);
	close(fd);
	return 0;
}*/
