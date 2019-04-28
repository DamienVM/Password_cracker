#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

void *lecture(char *fichier)
{
	int N = 1;
	for(int i = 0 ; i < N ; i++)
	{
		int a = open(fichier, O_RDONLY);
		char buffer[32];
		int b = read(a,&buffer,32);
		while(b > 0)
		{
			b = read(a,&buffer,32);
		}
		close(a);
	}
}


int main(int argc,char *argv[])
{
	int opt;
	int f = 1;
	int N = 1; /*nombre de threads de calculs*/
	int c = 0;
	int o = 0;
	char *out;

	while ((opt = getopt (argc, argv, "t:co:")) != -1)
	{
		if(opt == 't')
		{
			N = *optarg;
		}
		if(opt == 'c')
		{
			c = 1;
		}
		if(opt == 'o')
		{
			o = 1;
			out = optarg;
		}
		f = optind;
	}	

	char *fichier = argv[f];
	uint8_t *ptr = malloc(sizeof(*ptr)*(N+1));
	return 1;
}


