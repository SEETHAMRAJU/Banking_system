
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/file.h>

#include "bank.h"
#include "account.h"
#include "credential.h"
int main()
{
	struct Bank b;
	
	strcpy(b.name, "ICCC");
	strcpy(b.branch, "BNGLR");
	b.last_customer = 0;

	int fd = open("bank.dat",O_WRONLY);
	
	/* account 
	 * cred 
	 *
	 * */
	int fd1 = open("account.dat",O_WRONLY);
	int ffd = open("credential.dat",O_WRONLY);
	struct Account acc;
	struct credential cred;
	for(int i=0;i<10;i++)
	{
		acc.ac_id = i;
		acc.transactions=0;
		acc.balance = 100+(10*i);
		acc.arr[0] = i;
		acc.arr[1] = 2*i;
		acc.type = 'J';
		
		sprintf(cred.username,"%d",i);
		sprintf(cred.password,"%dIC",i);

		cred.loc = i*sizeof(struct Account);
	
		write(fd1,&acc,sizeof(struct Account));
		write(ffd,&cred,sizeof(struct credential));
	}
	b.last_account = 10;
	write(fd,&b,sizeof(b));
	close(fd);
	close(fd1);
	close(ffd);


	return 0;
}
