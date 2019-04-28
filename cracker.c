#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

char **ftrad;        /*Liste des fichiers dont le premier argument est un pointeur vers le nbr de fichiers*/
char **hash;         /*Buffer pour les pointeurs hash */
pthread_mutex_t mutex_hash;
int N =1;
/*
Fonction pour le thread de lecture
 */
void *lecture(void* param)
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
	  pthread_mutex_lock(&mutex_hash);
	  for(int j = 0; j < N+1 && i==0 ;j++){
	    if(hash[j]==NULL){
	      hash[j]=buff;
	      printf("\t copié en zone %i\n",j);
	      i=1;
	    }
	    else if(j==N){
	      i=1;
	    }
	  }
	  
	  pthread_mutex_unlock(&mutex_hash);
	}
	
	count++;
	/*printf("\t %i %064hhx \n",count,buff);*/
      }
    }
    close(a);
  }
  printf("il y a %i mots\n",count);
  printf("fin de lecture\n");
  pthread_exit(NULL);
}

/*
Fonction principale
 */
int main(int argc,char *argv[])
{
  printf("\033[0;34mFonction principale commencée\033[00m\n");
  int c = 0;     /*Consonne ativé*/
  int nargs = 1; /*Compteur d'arguments*/
  int o = 0;     /*Fichier de sortie ou pas*/
  char *out;     /*Fichier de sortie*/
  printf("Initialisation terminée\n");

  if(strcmp(argv[nargs],"-t") == 0)
    {
      N = atoi(argv[nargs+1]);
      nargs = nargs+2;
    }
  printf("le Nombre de thread est %i\n", N);
  if(strcmp(argv[nargs],"-c") == 0)
    {
      c = 1;
      nargs++;
    }
  printf("Les consonnes sont a:%i\n", c);
  if(strcmp(argv[nargs],"-o") == 0)
    {
      o = 1;
      out = argv[nargs+1];
      nargs = nargs+2;
    }
  printf("Il y a un fichier de sortie:%i\n", o);




  /*
    Fin de L'anayse des arguments
   */
  printf("\033[0;31mArivée à la prochaine étape\033[00m\n");

  int nf = argc-nargs; /*Nombre de fichiers en argument*/
  printf("il y a %i fichiers à traiter\n",nf);

  ftrad=  malloc(sizeof(char*)*(nf+1));
  ftrad[0] = (char*)&nf;

  printf("il y a bien %i a traiter\n",(int)*ftrad[0]) ;
  for(int i = 1; i<nf+1 ;i++)
    {
      ftrad[i]=argv[nargs+i-1];
      printf("\tfichier %i copié %s\n",i,ftrad[i]);
    }
  
  printf("creation du tableau de buffer\n"); 
  hash = calloc((N+1),sizeof(uint8_t));
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
