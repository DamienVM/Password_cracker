/* Definition de la structure node qui contient un pointeur char et un pointeur node*/
typedef struct node{
  struct node *next;
  char *mot;
} node_t;

/* Definition de la stucture list qui contient un pointeur vers une node qui est la première de la liste*/
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
