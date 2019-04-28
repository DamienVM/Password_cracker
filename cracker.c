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


char **ftrad;        /*Liste des fichiers dont le premier argument est un pointeur vers le nbr de fichiers*/
char **hash;         /*Tableau pour les pointeurs hash */
char **trad;          /*Tableau pour les pointeurs traduction */
pthread_mutex_t mutex_hash;
pthread_mutex_t mutex_trad;
sem_t hashempty;
sem_t hashfull;
sem_t tradempty;
sem_t tradfull;
int N =1;
int lecture_finie = 0;


/*
Fonction pour le thread de lecture
 */


void *lecture(void *param)
{
  int nf = (int)*ftrad[0];
  printf("il y a %i fichiers a lire\n",nf);
  int count =0;
  char* buff = malloc(32);

  for(int i=1;i<nf+1;i++){                          /*boucle pour lire tout les fichiers*/
    printf("%s\n",ftrad[i]);
    int a = open(ftrad[i],O_RDONLY);
    if(a==-1){printf("erreur1\n"); }

    int stat = 1;
    while(stat  != 0){                             /*boucle pour lire tout les hash*/
      stat = read(a,buff,32);
      if (stat==-1){
	printf("erreur2\n");
	printf("%\n ",strerror(errno));
	exit(0);
      }
      else if(stat != 0){
	for(int i =0; i==0;){                      /*boucle pour mette le hash dans le tableau */
	  sem_wait(&hashempty);
	  pthread_mutex_lock(&mutex_hash);
	  for(int j = 0; j < N+1 && i==0 ;j++){    /*boucle pour chercher une palce*/
	    if(hash[j]==NULL){
	      hash[j]=malloc(32);
	      strcpy(hash[j],buff);
	      printf("\t copié en zone %i\n",j);
	      i=1;
	    }
	  }
	  pthread_mutex_unlock(&mutex_hash);
	}
	sem_post(&hashfull);
	count++;
      }
    }
    close(a);
  }
  printf("il y a %i mots\n",count);
  printf("fin de lecture\n");
  lecture_finie = 1;
  pthread_exit(NULL);
}


/*
  Fonction pour le thread de traduction
*/


void *traduction (void *param)
{
  int sval;
  sem_getvalue(&hashfull,&sval);
  char *buf;  /* buffer pour le hash*/
  bool is_translate; 
  while(true)
    {
      char *buf2 = malloc(16); /* buffer pour la traduction*/
      for(int m =0; m==0;){
	sem_wait(&hashfull);
	pthread_mutex_lock(&mutex_hash);
	for(int n = 0; n < N+1 && m == 0 ; n++){
	  if(hash[n] != NULL)
	    {
	      buf = hash[n];
	      free(hash[n]);
	      hash[n]=NULL;
	      printf("pris en zone %i\n",n);
	      m = 1;
	    }
	}
	pthread_mutex_unlock(&mutex_hash);
      }
      sem_post(&hashempty);
      is_translate = reversehash(buf,buf2,16);
      printf("la traduction est :%s:\n",buf2);

      for(int i =0; i==0;){
	sem_wait(&tradempty);
	pthread_mutex_lock(&mutex_trad);
	for(int j = 0; j < N+1 && i==0 ; j++){
	  if(trad[j]==NULL){
	    trad[j]=buf2;
	    printf("copié2 en zone %i\n",j);
	    i=1;
	  }
	}
	pthread_mutex_unlock(&mutex_trad);
      }
      sem_post(&tradfull);
    }
  
}


/*
Fonction principale
 */


int main(int argc,char *argv[])
{
  printf("\033[0;34mFonction principale commencée\033[00m\n");
  int opt;
  int f = 1;
  int c = 0;
  int o = 0;
  char *out;

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
  printf("il y a %i threads\n" ,N);


  /*
    Fin de L'anayse des arguments
   */


  printf("\033[0;31mArivée à la prochaine étape\033[00m\n");
  printf("il y a %i fichiers à traiter\n",nf);

  ftrad=  malloc(sizeof(char*)*(nf+1));
  ftrad[0] = (char*)&nf;

  printf("il y a bien %i fichiers a traiter\n",(int)*ftrad[0]) ;
  for(int i = 1; i<nf+1 ;i++)
    {
      ftrad[i]=argv[f+i-1];
      printf("\tfichier %i copié %s\n",i,ftrad[i]);
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


  printf("\033[0;31mCréation des threads\033[00m\n");
  
  pthread_t lect;
  int err=pthread_create(&lect,NULL,&lecture,NULL);
  if(err!=0)
    {
      printf("erreur Lecture\n");
    }
  
  pthread_t traduc;
  int errr=pthread_create(&traduc,NULL,&traduction,NULL);
  if(errr!=0)
    {
      printf("erreur Traduction\n");
    }
  err = pthread_join(lect,NULL);
  errr = pthread_join(traduc,NULL);

  pthread_mutex_destroy(&mutex_hash);
  free(hash);
  free(ftrad);
  printf("\033[0;31mFonction principale finie\033[00m\n");
  return 1;
}
