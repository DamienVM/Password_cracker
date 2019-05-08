#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <semaphore.h>
#include <stdbool.h>
#include "cracker_ftc.c"

void main(void){
  char* cafe = "cafe";
  char* kayak = "kayak";
  char* fusee = "fusee";
  char* cqmd = "cqmd";

  printf("Test de la fonction 'count'\n");
  
  if(count(cafe,0) == 2)
    printf("\tReussi\n");
  else
    printf("\tErreur la fonction dit que %s a %i voyelle.s\n",cafe,count(cafe,0));
  
  if(count(kayak,0) == 3)
    printf("\tReussi\n");
  else
    printf("\tErreur la fonction dit que %s a %i voyelle.s\n",cafe,count(kayak,0));
  
  if(count(cqmd,0) == 0)
    printf("\tReussi\n");
  else
    printf("\tErreur la fonction dit que %s a %i voyelle.s\n",cafe,count(cqmd,0));
  
  if(count(cqmd,1) == 4)
    printf("\tReussi\n");
  else
    printf("\tErreur la fonction dit que %s a %i consonne.s\n",cafe,count(cqmd,1));
  
  if(count(fusee,1) == 2)
    printf("\tReussi\n");
  else
    printf("\tErreur la fonction dit que %s a %i consonne.s\n",cafe,count(fusee,1));

}

