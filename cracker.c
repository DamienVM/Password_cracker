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

char **ftrad;        /*Liste des fichiers dont le premier argument est un pointeur vers le nbr de fichiers*/
char **hash;         /*Tableau pour les pointeurs hash */
char **trad;          /*Tableau pour les pointeurs traduction */
pthread_mutex_t mutex_hash;
pthread_mutex_t mutex_trad;
sem_t empty;
sem_t full;
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

  for(int i=1;i<nf+1;i++){
    printf("%s\n",ftrad[i]);
    int a = open(ftrad[i],O_RDONLY);
    if(a==-1){printf("erreur1\n"); }

    int stat = 1;
    while(stat  != 0){
      stat = read(a,buff,32);
      /*printf("%064x\n",*buff);*/
      if (stat==-1){
	printf("erreur2\n");
	printf("%\n ",strerror(errno));
	exit(0);
      }
      else if(stat != 0){
	for(int i =0; i==0;){
	  sem_wait(&empty);
	  pthread_mutex_lock(&mutex_hash);
	  for(int j = 0; j < N+1 && i==0 ;j++){
	    if(hash[j]==NULL){
	      hash[j]=buff;
	      printf("\t copié en zone %i\n",j);
	      printf("le hash est %i",*buff); 
	      i=1;
	    }
	  }
	  pthread_mutex_unlock(&mutex_hash);
	}
	sem_post(&full);
	count++;
	/*printf("\t %i %064hhx \n",count,buff);*/
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
  sem_getvalue(&full,&sval);
  char *buf;
  while(lecture_finie == 0 && sval != 0)
  {
	sem_wait(&full);
	pthread_mutex_lock(&mutex_hash);
	for(int i = 0; i<N+1 ; i++)
	{
	  if(hash[i] != NULL)
	  {
	     buf = hash[i];
	  }
	}
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
  printf("il y a %i threads" ,N);




  /*
    Fin de L'anayse des arguments
   */
  printf("\033[0;31mArivée à la prochaine étape\033[00m\n");
  printf("il y a %i fichiers à traiter\n",nf);

  ftrad=  malloc(sizeof(char*)*(nf+1));
  ftrad[0] = (char*)&nf;

  printf("il y a bien %i a traiter\n",(int)*ftrad[0]) ;
  for(int i = 1; i<nf+1 ;i++)
    {
      ftrad[i]=argv[f+i-1];
      printf("\tfichier %i copié %s\n",i,ftrad[i]);
    }
  
  printf("creation du tableau de buffer\n"); 
  hash = calloc((N+1),sizeof(char*));
  if(hash[0]==NULL)
    {
      printf("\tfait\n");
    }
  pthread_mutex_init(&mutex_hash,NULL);
  




  /*
    Etape de création des threads
   */
  printf("\033[0;31mCréation des threads\033[00m\n");
  
  pthread_t lect;
  sem_init(&full,0,0);
  sem_init(&empty,0,N+1);

  int err=pthread_create(&lect,NULL,&lecture,NULL);
  if(err!=0)
    {
      printf("erreur Lecture\n");
    }
  err = pthread_join(lect,NULL);
  
  pthread_mutex_destroy(&mutex_hash);
  free(hash);
  free(ftrad);
  printf("\033[0;31mFonction principale finie\033[00m\n");
  return 1;
}
