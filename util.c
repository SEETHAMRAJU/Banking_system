
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


int char_to_num(char *num, int len)
{
	int ans = 0;
	for(int i=0;i<len;i++)
		ans = ans*10 + (num[i]-'0');
	return ans;
}
/*
int main()
{
	char m[10]="12345";
	int h = char_to_num(m,strlen(m));
	printf("%d\n",h);
	return 0;

}*/
