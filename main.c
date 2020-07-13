
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/file.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<pthread.h>
#include<arpa/inet.h>



#include "account.c"
#include "credential.h"
#include "bank.h"
#define error(x) printf("ERROR -- %s\n",x);
#define clear(x) memset(x,'\0',sizeof(x))
#define NCLIENTS 50
#define N 1000

//last_account = the current empty account
//Global Variables 
int FD_ACCOUNT,FD_CRED;
struct Bank bank;

char user_admin[10]="admin",pass_admin[10]="123";
struct credential userdatabase[N];
int account_location[N];
pthread_mutex_t lock;



void check_error(int x,int y, char *m)
{
	if(x!=y)
	{
		error(m);
		exit(0);
	}
	return ;
}
int verify(char *x,char *y, char *z,int l)
{
	int flag = 0;int a=0,b=0;
	for(int i=0;i<l;i++)
	{
		if(x[i]==';')
		{
			flag =1;
			continue;
		}
		if(flag)
		{
			if(x[i]!=z[a])
				return 0;
			a++;
		}
		else 
		{
			if(x[i]!=y[b])
				return 0;
			b++;
		}
	}

	return 1;
}

void create_admin_session(int socket)
{
	char message[1000] = "Welcome Admin\n1.Search account\n2.Modify Account\n3.Delete Account\n4.Add Account\n5.Logout and exit\n"; 
	char converse[1000]="";
	send(socket,message,strlen(message), 0);
	
	char reply[1];
	while(1)
	{
		recv(socket,reply, sizeof(char),0);
		if(reply[0]=='5')
		{
			send(socket, "BYE", 3, 0);
			return ;
		}
		else if(reply[0]=='1')
		{
			/*account search by id*/	
			memset(converse,'\0',sizeof(converse));
			strcpy(converse,"Enter Account Number : ");		
			send(socket, converse, strlen(converse), 0);
			
			clear(converse);
			recv(socket, converse, 1000, 0);
			
			char temporary[500]="";
			printf("%d\n",userdatabase[char_to_num(converse,strlen(converse))].loc);
			if(userdatabase[char_to_num(converse,strlen(converse))].loc==-1)
				strcpy(temporary,"Record not found");	
			else
				search_account(userdatabase[char_to_num(converse,strlen(converse))].loc, FD_ACCOUNT, temporary);
				strcpy(converse, temporary);
			send(socket, converse, strlen(converse), 0);
		}
		else if(reply[0] == '2')
		{
			/*Modify by ID account*/
			clear(converse);
			strcpy(converse,"Enter Account Number : ");
			send(socket, converse, strlen(converse),0);
			clear(converse);
			recv(socket, converse, 1000, 0);
			int no = char_to_num(converse,strlen(converse));
			char returnval[500];

			strcpy(converse, "Enter the updated details in the following way <BALANCE>;<HOLDER1>;<HOLDER2>(IF you don't want to update a field leave it Eg. 200;;2) : \n");
			send(socket, converse, strlen(converse), 0);
			clear(converse);
			recv(socket, converse, 1000,0);
			
			modify_account(converse,strlen(converse), FD_ACCOUNT, returnval,userdatabase[no].loc);
			clear(converse);
			strcpy(converse, returnval);
			send(socket, converse, strlen(converse), 0);
		}
		else if(reply[0] == '3')
		{
			clear(converse);
			strcpy(converse, "Enter Account Number : ");
			send(socket, converse, strlen(converse), 0);

			clear(converse);
			recv(socket, converse, 1000, 0);
			delete_account(account_location[char_to_num(converse,strlen(converse))], FD_ACCOUNT);
			userdatabase[char_to_num(converse,strlen(converse))].loc = -1;
			char temp[100]="";
			strcpy(temp, converse);
			clear(converse);
			sprintf(converse, "YOU ACCOUNT %s HAS BEEN REMOVED!!",temp);
			send(socket, converse, strlen(converse),0);
				
		}
		else if(reply[0] == '4')
		{
			/* Adding an account */
			strcpy(converse, "Enter details with the following format\n<BALANCE>;<HOLDER-1-ID>;<HOLDER-2-ID>\n");

			send(socket, converse, strlen(converse),0);
			clear(converse);
			recv(socket, converse, 1000, 0);

			char returnval[500];
			int ret = add_account(FD_ACCOUNT, bank.last_account++, converse, strlen(converse));
			clear(converse);
			sprintf(converse, "YOUR ACCOUNT NUMBER : %d\nYOUR PASSWORD : %d%s%s\n",bank.last_account-1,bank.last_account-1,"I","C");
			send(socket, converse, 1000, 0);
			
			sprintf(userdatabase[bank.last_account-1].username ,"%d",bank.last_account-1);
			sprintf(userdatabase[bank.last_account-1].password, "%d%c%c",bank.last_account-1,'I','C');
			if(bank.last_account-1!=0)
				userdatabase[bank.last_account-1].loc = userdatabase[bank.last_account-2].loc+sizeof(struct Account);
			else
				userdatabase[bank.last_account-1].loc = 0;
		/*
		 *Its a possibility that the array account_location might be avoided.
		 */	account_location[bank.last_account -1] = userdatabase[bank.last_account-1].loc;
			printf("%s %s %d\n",userdatabase[bank.last_account-1].username, userdatabase[bank.last_account-1].password, userdatabase[bank.last_account-1].loc);
			
		}
		else
		{
			clear(converse);
			strcpy(converse, "OPTION INVALID\n");
			send(socket, converse, strlen(converse), 0);
			//return ;
		}
	}
	return ;
}


void create_session(struct credential cred, int socket)
{
	char converse[1000];
	int ac_no = char_to_num(cred.username,strlen(cred.username));
	sprintf(converse,"Welcome %s !!\n1.Deposit\n2.Withdraw\n3.Balance Enquiry\n4.Password Change\n5.View Details\n6.Exit && Logout\n",cred.username);
	send(socket, converse, strlen(converse),0);
	char reply[1];
	/**/

	/**/
	while(1)
	{
		clear(converse);
		recv(socket, reply,sizeof(char),0);
		if(reply[0]=='1')
		{
				/*Deposit*/
			clear(converse);
			strcpy(converse, "Enter Amount : ");
			send(socket, converse, strlen(converse),0);
			clear(converse);

			recv(socket,converse, 1000,0);

				// start mutex
			pthread_mutex_lock(&lock);
			int rep = transact(cred.loc,FD_ACCOUNT,converse,1);
			//end mutex
			pthread_mutex_unlock(&lock);
			clear(converse);
			sprintf(converse,"Your current balance : %d\n",rep);
			send(socket,converse,strlen(converse),0);
		}
		else if(reply[0]=='2')
		{
				/*Withdraw*/
			clear(converse);
			strcpy(converse, "Enter Amount : ");
			send(socket, converse, strlen(converse),0);

			clear(converse);
				
			recv(socket, converse, 1000,0);

				//start mutex
			pthread_mutex_lock(&lock);
			int rep = transact(cred.loc, FD_ACCOUNT, converse, 0);
				//end mutex
			pthread_mutex_unlock(&lock);
			clear(converse);
			if(rep==-1)
				strcpy(converse,"Insufficient Balance!!!\n");
			else
				sprintf(converse,"You current balance : %d\n",rep);
			send(socket, converse, strlen(converse),0);
		}

		else if(reply[0]=='3')
		{
			// Balance Enquiry
			clear(converse);
			getBalance(FD_ACCOUNT, cred.loc, converse);
			send(socket, converse,strlen(converse), 0);

		}
		else if(reply[0]=='4')
		{
			/*Change Password */
			// converse="Enter new Password : ";
			strcpy(converse, "Enter New Password : ");
			send(socket, converse, strlen(converse), 0);
			//start mutex 
			pthread_mutex_lock(&lock);
			clear(converse);
			recv(socket, converse, 1000, 0);
			strcpy(cred.password,converse);
			clear(userdatabase[ac_no].password);
			strcpy(userdatabase[ac_no].password, converse);
			clear(converse);
			strcpy(converse,"Your Password change has been successfull!!\n");
			send(socket, converse, strlen(converse),0);
			//end mutex
			pthread_mutex_unlock(&lock);
		}
		else if(reply[0]=='5')
		{
			/*View Details*/
			clear(converse);
			search_account(cred.loc, FD_ACCOUNT,converse);
			send(socket,converse,strlen(converse),0);
		}
		else if(reply[0]=='6')
		{
			send(socket, "BYE",3,0);
			return;
		}
	}
	return ;
}


void * clientFunction(void *args)
{
	char client_message[1000], username[600],password[400];
	int newSocket = *((int *)args);
  	char message[2000] ="Connection Established";
	send(newSocket, message, strlen(message),0 );
	recv(newSocket , client_message , 2000 , 0);
	printf("%s\n",client_message);	
	if(verify(client_message,user_admin, pass_admin, strlen(client_message)))//;
	{
		create_admin_session(newSocket);
		pthread_exit(NULL);
	}
	else
	{
		int i=0;
		while(!verify(client_message,userdatabase[i].username, userdatabase[i].password,strlen(client_message)) && i<N)
			i++;
		if(i==N)
			pthread_exit(NULL);
		if(userdatabase[i].loc!=-1)
			create_session(userdatabase[i],newSocket);
		else
		{
			send(newSocket,"BYE",3,0);
		}
	}
	pthread_exit(NULL);
}
/*
 * accno. -> index of the array
 * balance 
 * arr[2]; arr[0] -> cust 
 * transaction = -1
 * type J, Single
 * */
/*
 * loc -> offset
 *  usernae -> 
 *  passweod ->
 * */
int main()
{
	//Global values for the bank like the number of customers
	char *bankf = "bank.dat";
	char *accountf = "account.dat";
	char *credef = "credential.dat";

	pthread_mutex_init(&lock, NULL);

	int fd = open(bankf,O_RDONLY);
	if(fd<0)
	{
		error("File not found")
		return 0;
	}
	FD_ACCOUNT = open(accountf, O_RDWR);
	int result = read(fd,&bank,sizeof(bank));
	if(result<0)
	{
		error("Read from the file failed");
		return 0;
	}
	close(fd);
	
	fd = open(credef,O_RDONLY);
	struct credential temporary;
	int i=0;
	while(read(fd,&temporary, sizeof(temporary)))
		userdatabase[i++]=temporary;
	while(i--)
		account_location[char_to_num(userdatabase[i].username,strlen(userdatabase[i].username))] = userdatabase[i].loc;
	i=0;	
	printf("Welcome to %s Bank, %s\n",bank.name,bank.branch);
	
	//here goes the intereactions and all
	struct sockaddr_in addr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
	int server_fd = socket(PF_INET,SOCK_STREAM, 0);
	//check_error(server_fd, 0,"Server Not working");
	

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);
	memset(addr.sin_zero, '\0' ,sizeof(addr.sin_zero));
	bind(server_fd, (struct sockaddr *) &addr, sizeof(addr));
	int listening = listen(server_fd, NCLIENTS); 
	pthread_t tid[NCLIENTS+10];
	int count = 0,newClient;
	while(1)
	{
		addr_size = sizeof(serverStorage);
		newClient = accept(server_fd, (struct sockaddr *) &serverStorage, &addr_size);
		
		int thread_creation = pthread_create(&tid[count], NULL, clientFunction, &newClient);
			
		check_error(thread_creation, 0, "thread creation failed\n");
		if(count > 50)
			break;
		count++;
		/*if(count%3==0 || count==50)
		{
			int cur_fd = open("credential.dat",O_WRONLY);
			for(int i=0;i<bank.last_account+1;i++)
				write(cur_fd,&userdatabase[i],sizeof(struct credential));
			close(cur_fd);
		}*/
	}
	close(FD_ACCOUNT);
	return 0;
}
