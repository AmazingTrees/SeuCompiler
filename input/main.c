#include <stdio.h>
#include <string.h>
#include"test.h"

int main()
{
	int a = 5;
	int b = a + 3.5E3;
	a= 8;
	char s[] = "I love the world\n";
	for(int i=0; i<5; i++)
		printf("%s\n",s);
}