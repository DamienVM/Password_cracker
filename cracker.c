#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>

void *lecture(char *fichier)
{
	int a = open(fichier, O_RDONLY);
	uint32_t buffer;
	read(a,buffer,32);
}


int main(int argc,char *argv[])
{
	int N = 1; /*nombre de threads de calculs*/
	int c = 0;
	int nargs = 1;
	int o = 0;
	char *out;
	if(strcmp(argv[nargs],"-t") == 0)
	{
		N = atoi(argv[nargs+1]);
		nargs = nargs+2;
	}
	if(strcmp(argv[nargs],"-c") == 0)
	{
		c = 1;
		nargs++;
	}
	if(strcmp(argv[nargs],"-o") == 0)
	{
		o = 1;
		out = argv[nargs+1];
		nargs = nargs+2;
	}
	uint8_t *ptr = malloc(sizeof(*ptr)*(N+1));
	
	return 1;
}


