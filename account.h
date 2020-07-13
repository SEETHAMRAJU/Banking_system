#ifndef ACCOUNT_H
#define ACCOUNT_H



struct Account{
	int ac_id, balance, transactions;
	char type;
	int arr[2];
};


//Get the latest account number 
int getLast(char *file_name);
//Create account 

/*char **/
void delete_account(int loc, int fd);

void modify_account(char converse[], int len, int fd, char temp[], int loc);

void search_account(int k, int fd, char *g);

void create_account(int ac_id, int balance, char type, int *arr);

void print_account(struct Account *acc);

//struct Account * search_account_by_id(int id);



#endif
