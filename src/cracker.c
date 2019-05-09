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
#include "reverse.c"
#include "sha256.c"
#include <sys/time.h>
/*Les fonctions supplémentatires se trouvent dans cracker_fct.c */
#include "cracker_fct.c"


char **ftrad;    /*Liste des fichiers*/
uint8_t **hash;  /*Tableau pour les pointeurs hash */
char **trad;     /*Tableau pour les pointeurs traduction */
pthread_mutex_t mutex_hash;
pthread_mutex_t mutex_trad;
sem_t hashempty;
sem_t hashfull;
sem_t tradempty;
sem_t tradfull;
int c = 0;  /*valeur qui indique si on analyse les consonnes */
int o = 0;  /*valeur qui indique si il y a un fichier de sortie */
int T = 1;  /* nombre de threads de calcul*/
int lecture_finie = 0;  /* valeur qui indique si la lecture est finie */
int count_mot; /*Compteur de mots a traduire*/
char *out;  /*Nom du fichier de sortie*/


/*
  Fonction pour le thread de lecture
 */
 
 
void *lecture(void *param)
{
  int nf = (int)*ftrad[0];
  count_mot =0;

  for(int i=1;i<nf+1;i++){                       /*boucle pour lire tout les fichiers*/
    int a = open(ftrad[i],O_RDONLY);
    if(a==-1){printf("impossible d'ouvrir le fichier %i\n",i ); }

    int stat = 1;
    while(stat  != 0){                           /*boucle pour lire tout les hash*/
      uint8_t* buff= malloc(32);
      stat = read(a,buff,32);
      if (stat==-1){
	    printf("impossible de lire le fichier %i \n", i);
	    exit(0);
      }
      else if(stat != 0){
	    int i = 0;
	    while(i == 0){                    /*boucle pour mette le hash dans le tableau */
	      sem_wait(&hashempty);
	      pthread_mutex_lock(&mutex_hash);
	      for(int j = 0; j < T+1 && i==0 ;j++){  /*boucle pour chercher une place*/
	        if(hash[j]==NULL){
	          hash[j]=buff;
	          i=1;
	        }
	      }
	      pthread_mutex_unlock(&mutex_hash);
	    }  
	    sem_post(&hashfull);
	    count_mot++;
      }
    }  
    close(a);
  }
  lecture_finie++;
  pthread_exit(NULL);
}


/*
  Fonction pour le thread de traduction
*/


void *traduction (void *param)
{
  int value;
  sem_getvalue(&hashfull,&value);
  uint8_t *buf;  /* buffer pour le hash*/
  while(value != 0 || lecture_finie != 1){                     /*Boucle principale qui agit tant que la lecture n'est pas finie ou que le tableau hash n'est pas vide */
    int m = 0;
    while(m == 0){                          /*Boucle pour copier un hash */
      sem_wait(&hashfull);	
      pthread_mutex_lock(&mutex_hash);
      for(int n = 0; n < T+1 && m == 0 ; n++){     /*Boucle pour chercher le hash a copier*/
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
    char *buf2 = malloc(16);/* buffer pour la traduction*/
    reversehash(buf,buf2,16);
    free(buf);  /*le pointeur du hash plus nécessaire*/
    
    int i = 0;
    while(i == 0){                        /*Boucle pour mettre le mot dans le tableau*/
      sem_wait(&tradempty);
      pthread_mutex_lock(&mutex_trad);
      for(int j = 0; j < T+1 && i==0 ; j++){     /*Boucle pour chercher une place*/
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
  pthread_exit(NULL);
}


/*
  Fonction pour le thread qui choisi les candidats
*/


void *candidat(void* param)
{
  list_t *list = malloc(sizeof(list_t)); /*Initialisation de la liste*/
  char *tra;   /*mot courrant analysé*/
  int nbr = 0; /*nombre d'occurence maximal de consonne ou voyelle */
  int co;      /*Nombre d'occucence du mot actuellement analysé */
  int counter = 0; /*Compteur de mots traduits*/
  while(counter < count_mot || lecture_finie != 1){        /*Boucle principale qui agit tant que le conteur de mots traduits est plus petit que le nombre de mots lu ou que la lecture n'est pas finie */
    int m = 0;
    while(m == 0){                      /*Boucle pour prendre un mot*/
      sem_wait(&tradfull);
      pthread_mutex_lock(&mutex_trad);
      for(int n = 0; n < T+1 && m == 0 ; n++){ /*Boucle pour chercher un mot dans le tableau*/
	    if(trad[n] != NULL){
	       tra = trad[n];
	       trad[n]=NULL;
	       m = 1;
	    }
      }
      pthread_mutex_unlock(&mutex_trad);
    }	
    sem_post(&tradempty);
    /*Voir si c'est un candidat*/
    co = count(tra,c);
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
    counter++;
  }
  /*Choix des candidats terminé*/
  if(o)
    {
      int a = open(out,O_RDWR | O_TRUNC);
      if(a==-1){printf("impossible d'ouvrir le fichier de sortie \n"); }
      struct node *n = list->first;
      for (int i = 0; i < list->size ; i++)
	{
	  write(a,n->mot,strlen(n->mot));
	  write(a,"\n",1);
	  n = n->next;
	}
      close(a);
    }   
  else{
    struct node *n = list->first;
    for (int i = 0; i < list->size ; i++){
      printf(n->mot);
      printf("\n");
      n = n->next;
    }
  }
  delete_list(list);
  pthread_exit(NULL);
}


/*
  Fonction principale
 */
 
 
int main(int argc,char *argv[]){
  struct timeval tv1,tv2;
  long long temps;
  gettimeofday(&tv1,NULL);
  int opt;
  int f = 1;
  while ((opt = getopt (argc, argv, "t:co:")) != -1){
    if(opt == 't'){
      T = atoi(optarg);
    }
    if(opt == 'c'){
      c = 1;
    }
    if(opt == 'o'){
      o = 1;
      out = optarg;
    }
    f = optind;
  }	
  
  int nf = argc-f; /*Le nombre de fichiers a traduire*/
  
  /* Fin de L'anayse des arguments */
  
  ftrad=  malloc(sizeof(char*)*(nf+1));
  ftrad[0] = (char*)&nf;
  
  for(int i = 1; i<nf+1 ;i++){
    ftrad[i]=argv[f+i-1];
  }
  
  hash = calloc((T+1),8);
  if(hash[0] != NULL){
    printf("erreur dans la creation du tableau de hash \n");
  }

  trad = calloc((T+1),8);
  if(trad[0]!= NULL){
    printf(" erreur dans la creation du tableau de traduction \n");
  }
  
  pthread_mutex_init(&mutex_hash,NULL);
  pthread_mutex_init(&mutex_trad,NULL);
  sem_init(&hashfull,0,0);
  sem_init(&hashempty,0,T+1);
  sem_init(&tradfull,0,0);
  sem_init(&tradempty,0,T+1);

  /* Etape de création des threads */
  
  pthread_t lect;
  int err=pthread_create(&lect,NULL,&lecture,NULL);
  if(err!=0){
    printf("impossible de creer le thread de lecture \n");
  }
  
  pthread_t threads[T];
  for(int i=0; i < T ; i++){
    if (pthread_create(&(threads[i]),NULL,&traduction,NULL) != 0){
      printf("impossible de creer le thread de traduction %i \n", i);
    }
  } 

  pthread_t selec;
  int errrr=pthread_create(&selec,NULL,&candidat,NULL);
  if(errrr!=0){
    printf("impossible de creer le thread de selection \n");
    }
  
   /* Etape de lancement des threads */
   
  pthread_join(lect,NULL);
  for(int j = 0 ; j < T ; j++){
    pthread_join(threads[j], NULL);
  }
  pthread_join(selec,NULL);

   /* Etape de destruction des threads */
   
  pthread_mutex_destroy(&mutex_hash);
  pthread_mutex_destroy(&mutex_trad);
  sem_destroy(&hashfull);
  sem_destroy(&hashempty);
  sem_destroy(&tradfull);
  sem_destroy(&tradempty);
  free(hash);
  free(trad);
  free(ftrad);

  gettimeofday(&tv2,NULL);
  temps=(tv2.tv_sec-tv1.tv_sec);
  printf("temps=%lld secondes\n",temps);



  return 1;
}
