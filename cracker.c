#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>



/*
Fonction pour le thread de lecture
 */
void *lecture(void* ptr)
{
  char *param = (char*)ptr;
  int nf = (int)param[0]; 
  printf("il y a %i fichiers a lire\n",nf);
  int count =0;
  uint32_t* buff;
  for(int i=1;i<nf+1;i++){
    int a = open(&param[i],O_RDONLY);
    if(a==-1){
      printf("erreur1\n");
    }
    int stat = 1;
    while(stat!=0){
      stat = read(a,(void*) buff,sizeof(uint32_t*));
      count++;
      if (stat==-1){
	printf("erreur2\n");
	exit(0);
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
  int N = 1;     /*nombre de threads de calculs*/
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
  char* ftrad[nf+1];     /*Liste des fichiers dont le premier argument est un pointeur vers le nbr de fichiers*/ 
  printf("il y a %i fichiers à traiter\n",nf);
  char* cnf = (char*)&nf; 
  ftrad[0]= cnf;
  printf("il y a bien %i a traiter\n",(int)*ftrad[0]) ;
  for(int i = 1; i<nf+1 ;i++)
    {
      ftrad[i]=argv[nargs+i-1];
      printf("\tfichier %i copié %s \n",i,ftrad[i]);
    }
  
  printf("creation du tableau de buffer\n"); 
  uint8_t**ptr = malloc(sizeof(uint8_t)*(N+1)); /*Buffer pour les pointeurs hash */
  if(ptr[0]==NULL)
    {
      printf("\tfait\n");
    }




  /*
    Etape de création des threads
   */
  printf("\033[0;31mCréation des threads\033[00m\n");
  
  pthread_t lect;

  int err=pthread_create(&lect,NULL,&lecture,(void*)*ftrad);
  if(err!=0)
    {
      printf("erreur Lecture\n");
    }
  err = pthread_join(lect,NULL);
   
  free(ptr);
  err=printf("\033[0;31mFonction principale finie\033[00m\n");
  return 1;
}
