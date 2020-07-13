
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<arpa/inet.h>

int main()
{
	printf("Client side program\n");
	struct sockaddr_in server;
	socklen_t addr_size;

	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	server.sin_family = AF_INET;
	server.sin_port = htons(8080);

	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	memset(server.sin_zero, '\0', sizeof(server.sin_zero));

	//connecting to server
	printf("Connecting to the server..");
	int n =5;
	while(n--)
		printf(".");
	printf("\n");

	char message[5000],reply[5000],username[400];
	addr_size = sizeof(server);
	connect(clientSocket, (struct sockaddr *) &server, addr_size);
	
	recv(clientSocket, message, 2000, 0);
	printf("-->>>>%s<<<<---\n",message);
	
	
	sleep(2);
	printf("Username : ");scanf("%s",username);
	char *pass = getpass("Password : ");
	
	char cred[1000];
	strcpy(cred,username);
	strcat(cred, ";");
	strcat(cred,pass);
	send(clientSocket, cred, 1000,0);
	
	while(1)
	{
		memset(message,'\0',sizeof(message));
		printf("-----------------------------------------------\n");
		recv(clientSocket, message, 1000, 0);
		if(strncmp(message,"BYE",strlen("BYE"))==0)
		{
			printf("Thank You, Logging Out\n");
			break;
		}
		printf("%s",message);
		scanf("%s",reply);
		send(clientSocket, reply, strlen(reply), 0);
		printf("----------------------------------------------\n");
	}
	close(clientSocket);

	
	return 0;
}
