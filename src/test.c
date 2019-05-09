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
#include "cracker_fct.c"

int main(void){
  char* cafe = malloc(sizeof(char)*16);
  strcpy(cafe,"cafe");
  char* fusee = malloc(sizeof(char)*16);
  strcpy(fusee,"fusee");
  char* kayak = "kayak";
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

  printf("Test de la fonction 'init_node'\n");
  node_t* node = init_node(fusee);
  node_t* n = init_node(cqmd);
  if(strcmp(node->mot,"fusee")==0)
    printf("\tReussi\n");
  else
    printf("\tErreur"); 

  if(strcmp(n->mot,"cqmd")==0)
    printf("\tReussi\n");
  else
    printf("\tErreur\n");

  free(node);
  free(n);

   printf("Test de la fonction 'add_node'\n");

   list_t* list = malloc(sizeof(list_t));
   list->first = NULL;
   add_node(list,fusee);
   add_node(list,cafe);
   
   if(strcmp(list->first->mot,"cafe")==0)
     printf("\tReussi\n");
   else
     printf("\tErreur\n");

   if(strcmp(list->first->next->mot,"fusee")==0)
     printf("\tReussi\n");
   else
     printf("\tErreur\n");

   printf("test de la fonction 'empty_list'\n");
   empty_list(list);
   if(list->first==NULL)
     printf("\tReussi\n");
   else
     printf("\tErreur\n");

   printf("Fin des Test");


   return 0;
}

