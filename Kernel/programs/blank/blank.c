#include "kernel.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "memory.h"

int main(int argc, char **argv)
{
	printf("My age is %i\n", 23);
	char words[] = "hello how are you";

	const char* token = strtok(words, " ");
	while(token) {
		printf("%s\n", token);
		token = strtok(NULL, " ");
	}

	while(1) {}
	return 0;
}