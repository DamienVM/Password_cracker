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
#include "reverse.h"
#include "reverse.c"
#include "sha256.h"
#include "sha256.c"
#include <sys/time.h>


char **ftrad;        /*Liste des fichiers dont le premier argument est un pointeur vers le nbr de fichiers*/
uint8_t **hash;         /*Tableau pour les pointeurs hash */
char **trad;          /*Tableau pour les pointeurs traduction */
pthread_mutex_t mutex_hash;
pthread_mutex_t mutex_trad;
sem_t hashempty;
sem_t hashfull;
sem_t tradempty;
sem_t tradfull;
int N = 1; /* nombre de threads de calcul*/
int M = 0; /*nombre de threads ayant fini l'étape traduction */
int W = 0; /*nombre de threads ayant fini l'étape lecture */
int c = 0;
int o = 0;
char *out;
int lecture_finie = 0;

typedef struct node{
  struct node *next;
  char *mot;
} node_t;

typedef struct list{
  struct node *first;
  int size;
} list_t;


/*
  Fonction pour initialiser une node  contennant un mot pointé par "val"
 */


node_t* init_node(char* val){
  node_t *n;
  n = malloc(sizeof(node_t));
  n->mot = val;
  n->next = NULL;
  return n;
}


/*
  Fonction pour ajouter à la liste pointée par "list" une node contennant un mot pointé par "val"
 */


int add_node(list_t *list, char* val){
  node_t *n = init_node(val);
  if(list->first==NULL){
    list->first = n;
    list->size=1;
    return 1;
  }
  node_t *p = list->first;
  list->first=n;
  n->next=p;
  list->size=list->size+1;
  return 1;
}


/*
  Fonction qui vide la liste (le pointeur vers la liste est conservé)
*/


void empty_list(list_t *list){
  node_t *c = list->first;
  list->first = NULL;
  node_t *n;
  while(c!=NULL){
    n = c->next;
    free(c->mot);
    free(c);
    c=n;
  }
}


/*
  Fonction qui suppprime la liste et son contenu
*/


void delete_list(list_t *list){
  empty_list(list);
  free(list->first);
  free(list);
}


/*
  Fonction qui, pour le mot pointé par "mot", compte le nombre d'occurence de voyelles ou de consonnes en fonction du paramètre c

*/


int count(char *mot, int c){
  int nbr = 0;
  for(int i = 0 ; i < strlen(mot) ; i++){
    if(mot[i]=='\0'){
      return nbr;
    }
    if(c == 0){
      if(mot[i]=='a' || mot[i]=='e' || mot[i]=='i' || mot[i]=='o' || mot[i]=='u' ||  mot[i]=='y'){
        nbr++;}
    }
    if(c == 1){
      if(mot[i]!='a' && mot[i]!='e' && mot[i]!='i' && mot[i]!='o' && mot[i]!='u' &&  mot[i]!='y'){
        nbr++;}
    }  
  }
  return nbr;
}


/*
  Fonction pour le thread de lecture
 */


void *lecture(void *param)
{
  int nf = (int)*ftrad[0];
  printf("il y a %i fichiers a lire\n",nf);
  int co =0;

  for(int i=1;i<nf+1;i++){                          /*boucle pour lire tout les fichiers*/
    printf("%s\n",ftrad[i]);
    int a = open(ftrad[i],O_RDONLY);
    if(a==-1){printf("impossible d'ouvrir le fichier %i \n, i"); }

    int stat = 1;
    while(stat  != 0){                              /*boucle pour lire tout les hash*/
      uint8_t* buff= malloc(32);
      /*printf("%i",buff);*/
      stat = read(a,buff,32);
      if (stat==-1){
	printf("impossible de lire le fichier %i \n, i");
	printf("%\n ",strerror(errno));
	exit(0);
      }
      else if(stat != 0){
	for(int i =0; i==0;){                      /*boucle pour mette le hash dans le tableau */
	  sem_wait(&hashempty);
	  pthread_mutex_lock(&mutex_hash);
	  for(int j = 0; j < N+1 && i==0 ;j++){    /*boucle pour chercher une place*/
	    if(hash[j]==NULL){
	      hash[j]=buff;
	      i=1;
	    }
	  }
	  pthread_mutex_unlock(&mutex_hash);
	}
	sem_post(&hashfull);
	co++;
      }
    }
    close(a);
  }
  printf("il y a %i mots\n",co);
  printf("fin de lecture\n");
  W++;
  pthread_exit(NULL);
}


/*
  Fonction pour le thread de traduction
*/


void *traduction (void *param)
{
  int value;
  sem_getvalue(&hashfull,&value);
  char *buf;  /* buffer pour le hash*/
  bool is_translate; 
  while(W != 1 || value != 0)
  {
      char *buf2 = malloc(16); /* buffer pour la traduction*/
      for(int m =0; m==0;){
	sem_wait(&hashfull);
	pthread_mutex_lock(&mutex_hash);
	for(int n = 0; n < N+1 && m == 0 ; n++){
	  if(hash[n] != NULL)
	    {
	      buf = hash[n];
	      hash[n]=NULL;
	      m = 1;
	    }
	}
	pthread_mutex_unlock(&mutex_hash);
      }
      sem_post(&hashempty);
      is_translate = reversehash(buf,buf2,16);
      free(buf);

      for(int i =0; i==0;){
	sem_wait(&tradempty);
	pthread_mutex_lock(&mutex_trad);
	for(int j = 0; j < N+1 && i==0 ; j++){
	  if(trad[j]==NULL){
	    trad[j]=buf2;
	    i=1;
	  }
	}
	pthread_mutex_unlock(&mutex_trad);
      }
      sem_post(&tradfull);
      sem_getvalue(&hashfull,&value);
  }
  printf("fin de traduction\n");
  M++;
  pthread_exit(NULL);
}

/*
  Fonction pour le thread qui choisi les candidats
*/


void *candidat(void* param)
{
  int value;
  sem_getvalue(&tradfull,&value);
  list_t *list = malloc(sizeof(list_t));
  list->first;
  list->size;
  char *tra;
  int nbr = 0;
  while(M != N || value != 0)
    {
      for(int m =0; m==0;){
	sem_wait(&tradfull);
	pthread_mutex_lock(&mutex_trad);
	for(int n = 0; n < N+1 && m == 0 ; n++){
	  if(trad[n] != NULL){
	    tra = trad[n];
	    trad[n]=NULL;
	    m = 1;
	  }
	}
	pthread_mutex_unlock(&mutex_trad);
      }
      sem_post(&tradempty);
      int co = count(tra,c);
      if(co == nbr){
	add_node(list,tra);
      }
      else if(co > nbr){
	nbr = co;
	empty_list(list);
	add_node(list,tra);
      }
      else{
	free(tra);
      }
      sem_getvalue(&tradfull,&value);
    }
  if(o)
    {
      int a = open(out,O_RDWR | O_TRUNC);
      if(a==-1){printf("impossible d'ouvrir le fichier de sortie"); }
      struct node *n = list->first;
      for (int i = 0; i < list->size ; i++)
	{
	  write(a,n->mot,strlen(n->mot));
	  write(a,"\n",1);
	  n = n->next;
	}
      close(a);
    }   
  else
    {
    struct node *n = list->first;
    for (int i = 0; i < list->size ; i++)
      {
	printf(n->mot);
	printf("\n");
	n = n->next;
      }
    }
  delete_list(list);
  printf("fin de la sélection\n");
  pthread_exit(NULL);
}


/*
  Fonction principale
 */


int main(int argc,char *argv[])
{
  struct timeval tv1,tv2;
  long long temps;
  gettimeofday(&tv1,NULL);
  int opt;
  int f = 1;

  while ((opt = getopt (argc, argv, "t:co:")) != -1)
  {
	if(opt == 't')
	{
		N = atoi(optarg);
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

  int nf = argc-f;


  /*
    Fin de L'anayse des arguments
   */


  ftrad=  malloc(sizeof(char*)*(nf+1));
  ftrad[0] = (char*)&nf;

  for(int i = 1; i<nf+1 ;i++)
    {
      ftrad[i]=argv[f+i-1];
    }
  
  printf("creation du tableau de hash\n"); 
  hash = calloc((N+1),8);
  if(hash[0]==NULL)
    {
      printf("\tfait\n");
    }

  printf("creation du tableau de trad\n"); 
  trad = calloc((N+1),8);
  if(trad[0]==NULL)
    {
      printf("\tfait\n");
    }

  pthread_mutex_init(&mutex_hash,NULL);
  pthread_mutex_init(&mutex_trad,NULL);
  sem_init(&hashfull,0,0);
  sem_init(&hashempty,0,N+1);
  sem_init(&tradfull,0,0);
  sem_init(&tradempty,0,N+1);


  /*
    Etape de création des threads
   */

  
  pthread_t lect;
  int err=pthread_create(&lect,NULL,&lecture,NULL);
  if(err!=0)
    {
      printf("erreur Lecture\n");
    }
  pthread_t threads[N];
  for(int i=0; i < N ; i++)
  {
    if (pthread_create(&(threads[i]),NULL,&traduction,NULL) != 0)
        {
          fprintf(stderr, "error: Cannot create thread  %d\n", i);
          break;
        }
  } 
  pthread_t selec;
  int errrr=pthread_create(&selec,NULL,&candidat,NULL);
  if(errrr!=0)
    {
      printf("erreur Selection\n");
    }


   /*
    Etape de lancement des threads
   */


  err = pthread_join(lect,NULL);
  for(int j = 0 ; j < N ; j++)
  {
     if (pthread_join(threads[j], NULL) != 0)
        {
          fprintf(stderr, "error: Cannot join thread  %d\n", j);
        }
  }
  errrr = pthread_join(selec,NULL);

  pthread_mutex_destroy(&mutex_hash);
  pthread_mutex_destroy(&mutex_trad);
  sem_destroy(&hashfull);
  sem_destroy(&hashempty);
  sem_destroy(&tradfull);
  sem_destroy(&tradempty);
  free(hash);
  free(ftrad);
  free(trad);


  gettimeofday(&tv2,NULL);
  temps=(tv2.tv_sec-tv1.tv_sec);
  printf("temps=%lld secondes\n",temps);



  printf("\033[0;31mFonction principale finie\033[00m\n");
  return 1;
}