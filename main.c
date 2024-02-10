#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#define lim 50 //taille de buffer de caractères , vous pouvez le changer , il doit ètre supérieur à 8 , si il est changé , vous devez recreer un nouveau fichier
#define b 4 //changez ce nombre si vous voulez une autre amplitude dans le bloc T!OF
#define MaxLength 20 //nombre de noms et prenoms ,  si vous voulez la modifier , vous devez aussi modifier la liste prédifinie dans le module Chargement_Aleatoir_NOM_PRENOM

//la donnée sera formulée comme ceci : taille[3] , clé[3] , eff[1] , info[length] (Max_Length = lim - 7)



//-----------------------PARTIE MACHINE ABSTRAITE -------------------------------

/*-------------------------- REMARQUE -------------------------------------------
LES MODULES QUI TERMINENT AVEC : ..._T C'EST DE T/OVC || ...._L C'EST DE L/OVC */

typedef struct ptit_enr{ //enregistrement d'une seule donnée de T!OF / Indexe
  int cle ; //la donnée est un entier
  bool eff ; //si eff = 1 , la donnée est supprimée et n'existe pas
  int i , j ;
}Data ;

typedef struct Tenreg_T{ //enregistrement d'un seul bloc T/OF
 Data Tab[b] ; //se divise en b données de haut
 int nbr ; //et le nombre de données existantes avec eff = 0
}enr_T ;

typedef struct Tenreg_L{ //enregistrement d'un seul bloc de L/OVC ou bien T/OVC (le champ Next n'est pas utilisée dans T/OVC)
char espace_donnee[lim] ;//c'est l'espace de clés , le data , les tailles ....
int Next_Block ; //ceci est la fondation pour un fichier Liste , cet entier enregistre la taille de saut de pointeur de SEEK_SET vers le prochain Block .
}enr_L ;

typedef struct Entet_T{ // l'entète T/OF
 int nbr_data ;//nbr de donnée existante avec eff = 0
 int nbr_bloc ;
 int nbr_eff ;//nbr de donnée sub_existante avec eff = 1 (logiquement supprimée)
}Ent_T;

typedef struct Entet_L{ // l'entète de L/OVC
 int Tete ;//contient la taille de saut de SEEK_SET vers le premier Bloc
 int posi_libr ;//la première position libre dans le dernier Bloc
 int nbr_don ;//nbr de données inséré non supprimé
 int nbr_eff ;//nbr de caractères sub_existante avec eff = 1 (logiquement supprimée)
 int nbr_Block ;//nombre de blocks existantes
}Ent_L;

int Entete_T(char *nomf , int n){ //machine abstraitre pour déterminer les éléments de l'entète T/OF
Ent_T e ;
FILE *f = fopen(nomf, "rb");
fread(&e , sizeof(e) , 1 , f);
fclose(f) ;

 switch (n){

  case 1: return e.nbr_bloc ;
  case 2: return e.nbr_data ;
  case 3: return e.nbr_eff ;

  default :
  printf("il y a une erreur dans l'entete.\n");
  return 0 ;
 }
}

int Entete_L(char *nomf , int n){ //machine abstraitre pour déterminer les éléments de l'entète L!OVC / T!OVC
Ent_L e ;
FILE *f = fopen(nomf, "rb");
fread(&e , sizeof(e) , 1 , f);
fclose(f) ;

 switch (n){

  case 1: return e.Tete ;
  case 2: return e.posi_libr ;
  case 3: return e.nbr_don ;
  case 4: return e.nbr_eff ;
  case 5: return e.nbr_Block ;

  default :
  printf("il y a une erreur dans l'entete.\n");
  return 0 ;
 }

}

void aff_entete_T(char *nomf , int n , int x){ //machine abstraitre pour affecter des valeurs sur les éléments de l'entète T!OF
Ent_T e ;

FILE *f = fopen(nomf , "rb+"); //rb+ pour la modification
fread(&e , sizeof(e) , 1 , f); //lire l'entète
fseek(f , 0 , SEEK_SET); //puisque on a fait fread , on va retourner le pointeur au début

 switch (n){

  case 1:
   e.nbr_bloc = x ; //modifier le nombre de bloc
   fwrite(&e , sizeof(e) , 1 , f);
   fclose(f) ;
   return ;
  case 2:
   e.nbr_data = x ; //modifier le nombre de datas
   fwrite(&e , sizeof(e) , 1 , f);
   fclose(f) ;
  return ;
  case 3:
   e.nbr_eff = x ; //modifier le nombre de datas suprimés ou bien vide
   fwrite(&e , sizeof(e) , 1 , f);
   fclose(f) ;
  return ;

  default :
   printf("il y a une erreur dans l'aff_entete.\n");
   fclose(f) ;
   return ;
 }

}

void aff_entete_L(char *nomf , int n , int x){ //machine abstraitre pour affecter des valeurs sur les éléments de l'entète L!OVC/T!OVC
Ent_L e ;

FILE *f = fopen(nomf , "rb+"); //rb+ pour la modification
fread(&e , sizeof(e) , 1 , f); //lire l'entète
fseek(f , 0 , SEEK_SET); //puisque on a fait fread , on va retourner le pointeur au début

 switch (n){

  case 1:
   e.Tete = x ;
   fwrite(&e , sizeof(e) , 1 , f);
   fclose(f) ;
   return ;
  case 2:
   e.posi_libr = x ;
   fwrite(&e , sizeof(e) , 1 , f);
   fclose(f) ;
  return ;
  case 3:
   e.nbr_don = x ;
   fwrite(&e , sizeof(e) , 1 , f);
   fclose(f) ;
  return ;
  case 4:
   e.nbr_eff = x ; //modifier le nombre de datas suprimés ou bien vide
   fwrite(&e , sizeof(e) , 1 , f);
   fclose(f) ;
  return ;
  case 5:
    e.nbr_Block = x ;
   fwrite(&e , sizeof(e) , 1 , f);
   fclose(f) ;
  return ;

  default :
   printf("il y a une erreur dans l'aff_entete.\n");
   fclose(f) ;
   return ;
 }

}

void fcpy(char *nomf1 , char *nomf2){//copier de fichier 1 vers 2 pour L/OVC

Ent_L t ;
enr_L Buff ;
int p , nbr_block = Entete_L(nomf1 , 5) ;
FILE *copy = fopen( nomf1 , "rb" ) , *paste = fopen( nomf2 , "wb" );
if(copy == NULL) return ;//you can't copy a 'nothing' , or can you ???

fread(&t , sizeof(t) , 1 , copy) ;
fwrite(&t , sizeof(t) , 1 , paste) ;

if( nbr_block == 0) return ;//there's nothing to copy...

p = Entete_L(nomf1 , 1) ; //p s'initialise à tète

 for(int i = 1 ; i <= nbr_block ; i++){ //ou bien While(p != -1) , comme NULL dans les listes
  fseek(copy , p , SEEK_SET) ;
  fseek(paste , p , SEEK_SET) ;//Diriger les pointeurs des fichiers en sautant de taille p

  fread(&Buff , sizeof(Buff) , 1 , copy) ;
  fwrite(&Buff , sizeof(Buff) , 1 , paste) ;//copie coller

  p = Buff.Next_Block ; //mis à jour pour trouver le prochain Block
 }

 fclose(copy);
 fclose(paste);
}

int LireDire_T(char *nomf , int i , enr_L *Buff ){//(T/OVC) prendre le ième Block de fichier dans le Buffer et retourne le pointeur vers ce Block

int p ;
FILE *f = fopen(nomf , "rb") ;
 if(f == NULL || i <= 0) return sizeof(Ent_L) ;//si le fichier est vide , on se positionne au premier bloque , efficace aux initialisations

p = sizeof(Ent_L) + (i - 1)*sizeof(*Buff) ;
fseek(f , p , SEEK_SET) ; //sauter vers ;e ième bloque

fread(Buff , sizeof(*Buff) , 1 , f) ;//lire le bloque

 fclose(f) ;
 return p ;
}

int LireDire_L(char *nomf , int i , enr_L *Buff , bool Next){//(L/OVC) couteux comme machine , prendre le ième Block de fichier dans le Buffer et retourne le pointeur vers ce Block

int p = Entete_L(nomf , 1) ;

 if(Entete_L(nomf , 5) == 0) return p ;

FILE *f = fopen(nomf , "rb") ;

 if(Next == 1){ //si c'est juste aller au suivant
  p = Buff->Next_Block ;
  fseek(f , Buff->Next_Block , SEEK_SET);
  fread(Buff , sizeof(*Buff) , 1 , f); //lire le Block directement
  fclose(f) ;
  return p ;
 }

fseek(f , Entete_L(nomf , 1) , SEEK_SET) ; //sauter vers premier block pointé par Tète
fread(Buff , sizeof(*Buff) , 1 , f); //lire le premier Block

 for(int k = 1 ; k < i ; k++){//une boucle pour aller jusqu'a ieme Block
  p = Buff->Next_Block ;
  fseek(f , Buff->Next_Block , SEEK_SET) ; //seek to next Block
  fread(Buff , sizeof(*Buff) , 1 , f); //lire le (k+1)eme Block
 } // notre Buffer est maintenant prét

 fclose(f) ;
 return p ;
}

int Alloc_Bloc(char *nomf){//calculer le pointeur pour le prochain bloc non utilisée

enr_L Buff ;
int p ;

if(Entete_L(nomf , 3) == 0) return Entete_L(nomf , 1) ;//cas spécial : fichier vide , donc la tète sera le pointeur à initialiser

p = LireDire_L(nomf , Entete_L(nomf , 5) , &Buff , 0) ; //p pointe maitenant vers le dernier Block

p = p + sizeof(Buff) ;

 return p ;
}

void EcrireDire_T(char *nomf , int i , enr_L Buff_write){ //(T/OVC)ecrire dans un ième bloque le buffer Buff

int p ;

FILE *f = fopen(nomf , "rb+") ;
 if(f == NULL || i < 0) return ;

 p = sizeof(Ent_L) + (i - 1)*sizeof(Buff_write) ;
 fseek(f , p , SEEK_SET) ;//positionner à ce Block
 fwrite(&Buff_write , sizeof(Buff_write) , 1 , f); //ecriture

 fclose(f) ;

}

void EcrireDire_L(char *nomf , int i , enr_L Buff_write){ //(L/OVC)ecrire dans un ième bloque le buffer

enr_L Buff_receiv ;
int p = LireDire_L(nomf , i , &Buff_receiv , 0) ; //aller au ième block

FILE *f = fopen(nomf , "rb+") ;
 if(f == NULL) return ;

 fseek(f , p , SEEK_SET) ;//positionner à ce Block
 fwrite(&Buff_write , sizeof(Buff_write) , 1 , f); //ecriture

 fclose(f) ;

}

void Ecrire_chaine_T(char *nomf , int *i ,  int *j , char *chaine){ //(T/OVC)écrire dans le ième Block de position j la chaine et faire des modifiactions si nécessaire de l'entaite dans TOVC

enr_L Buff ;
int taille_chaine = strlen(chaine) ;

LireDire_T(nomf , *i , &Buff) ;//Buffer prend le ième Block

 for(int m = 0 ; m <= taille_chaine-1 ; m++){ //pour chaque itération on écrit un caractère dans le Buffer
  Buff.espace_donnee[*j] = chaine[m] ;
  *j = *j + 1;
   if(*j > lim-1){ //si on est arrivée au fin du Buffer , on va au prochain Block en écrivant les dernières modifications
    EcrireDire_T(nomf , *i , Buff) ;//écriture
    *i = *i + 1 ;
    LireDire_T(nomf , *i , &Buff) ; //lire le suivant block
    *j = 0 ;
   }
 }

//dernière écriture
if(*j != 0) EcrireDire_T(nomf , *i , Buff) ; //ecriture
 if(*i > Entete_L(nomf , 5) || (*i == Entete_L(nomf , 5) && *j >= Entete_L(nomf , 2))){ //il a eut une écriture dans la fin de fichier
  aff_entete_L(nomf , 5 , *i) ;
  aff_entete_L(nomf , 2 , *j) ;
 }

}

void Ecrire_chaine_L(char *nomf , int *i ,  int *j , char *chaine){ //(L/OVC)écrire dans le ième Block de position j la chaine et faire des modifiactions si nécessaire de l'entaite

enr_L Buff ;
int taille_chaine = strlen(chaine) , nbr_bloc = Entete_L(nomf , 5) , last_posi = Entete_L(nomf , 2);

LireDire_L(nomf , *i , &Buff , 0) ;//Buffer prend le ième Block
if(*i >= nbr_bloc) Buff.Next_Block = -1 ;

 for(int m = 0 ; m <= taille_chaine-1 ; m++){ //pour chaque itération on écrit un caractère dans le Buffer
  Buff.espace_donnee[*j] = chaine[m] ;
  *j = *j + 1;
   if(*j > lim-1){ //si on est arrivée au fin du Buffer , on va au prochain Block en écrivant les dernières modifications
    if(Buff.Next_Block == -1) Buff.Next_Block = Alloc_Bloc(nomf) ;//si c'est le dernier bloque on va allouer un nouveau pour les prochaines écritures
    EcrireDire_L(nomf , *i , Buff) ;//écriture
    *i = *i + 1 ;
    LireDire_L(nomf , *i , &Buff , 1) ; //lire le suivant block
    if(*i > nbr_bloc) Buff.Next_Block = -1 ; //si il y a des nouveaux , l'initialisation de NULL
    *j = 0 ;
   }
 }

//dernière écriture
if(*j != 0) EcrireDire_L(nomf , *i , Buff) ; //ecriture
 if(*i > nbr_bloc || (*i == nbr_bloc && *j >= last_posi )){ //il a eut une écriture dans la fin de fichier
  aff_entete_L(nomf , 5 , *i) ;
  aff_entete_L(nomf , 2 , *j) ;
 }

}

void Extraire_chaine(char *nomf , int taille_chaine , int *i , int *j , char *chaine){//(L/OVC)extraire une chaine dans le fichier d'une taille donnée dans le bloque i du position j
  /*Inspired by algorithm of my collegues*/
//i , j se mis à jour aprés le dernier caractère extrait
enr_L Buff ;
 if( Entete_L(nomf , 3) == 0) return ;

LireDire_L(nomf , *i , &Buff , 0) ;//Buffer prend le ième Block

 for(int m = 0 ; m <= taille_chaine-1 ; m++){ //pour chaque itération on écrit un caractère
  chaine[m] = Buff.espace_donnee[*j] ;
  *j = *j + 1;
   if(*j > lim-1){ //si on est arrivée au fin du Buffer , on va au prochain Block
    *i = *i + 1 ;
    LireDire_L(nomf , *i , &Buff , 1) ;
    *j = 0 ;
   }
 }

}

void Dechiffrement(int x , char *nomb){//déchiffrer un nombre entier de 3 chiffres pour le rendre du tableau en CODE_ASCII
int p = x , last = 2 ;
int reste ;

if(p < 0){
  nomb[0] = '-' ;
  last = 1 ;
  p = -p ;
}

 for(int i = 0 ; i<=last ; i++){
  reste = p % 10 ;//prendre l'unité
  reste = reste + 48 ; //codeASCII de 0 est 48 , reste est maintenant le CODE ASCII de l'unité
  nomb[2-i] = reste ; //ecriture de la droite vers gauche
  p = p/10 ; //pour aller au prochain chiffre
 }
}

void chiffrement(char *nomb , int *x){//chiffrer une chaine de 3 chiffres en CODE_ASCII à un nombre entier
int p = 0 ,car , first = 0 ;
if(nomb[0] == '-') first = 1 ;

 for(int i = first ; i <= 2 ; i++){
  car = nomb[i] ;//prendre une unité ASCII
  car = car - 48 ; //codeASCII de 0 est 48 , car est maintenant le chiffre de l'unité
  p = p + car ; //ecriture de chiffre
  p = p*10 ; //multiplier pour aller au prochain chiffre
 }
 p = p / 10 ;
 if(nomb[0] == '-') p = -p ;
 *x = p ;
 return ;
}

void Random_String(int length , char *ch){ //générer une chaine aleatoire de taille length

int x , y ;

  for(int i = 0 ; i<length ; i++){
  x = rand() ; //x is randomised
  y = rand() ; //y is randomised
  y = y % 2 ; //y est soit 1 ou 0
  if(y == 0) x = x%26 + 65 ; //x est soit une lettre miniscule aleatoire
  else x = x%26 + 97 ;//ou bien majiscule
   ch[i] = x ;
 }

 ch[length] = '\0' ; //ch maitenant est une vrai chaine
}


//------------------------------------- PARTIE MANIPULATION DU FICHIER --------------------------------------------------------


void Lect_L_NO_VC(char *nomf){ //Lire un L/OVC ou T/OVC en divisant les parties avec "||" (vous pouvez l'enlevez en ligne 450)

enr_L Buff ;//Buffer
int Stop , state = 0 , posi_sauv = 0 , length = 0 , posi = 0 ; //si state = 0 : on est dans taille , 1 pour clé , 2 pour eff , 3 pour la donnée
char taille[3] , eff = '0' ;

FILE *f = fopen(nomf , "rb") ;
 if(f == NULL) return ;
if(Entete_L(nomf , 1) != 0 ) fseek(f , Entete_L(nomf , 1) , SEEK_SET) ; //sauter vers premier block pointé par Tète si c'est une liste
else fseek(f , sizeof(Ent_L) , SEEK_SET) ; //sinon c'est un tableau T/OVC

 for(int i = 1 ; i <= Entete_L(nomf , 5) ; i++){ //un simple parcourt des blocs
  fread(&Buff , sizeof(Buff) , 1 , f);//prendre le Block dans le Buffer
  printf("\nBloc %d : ",i);
    if(i == Entete_L(nomf , 5) && Entete_L(nomf , 2) != 0) Stop = Entete_L(nomf , 2) ; //si on est dans le dernier Bloc et la dernière position est au milieu , l'amplitude de dernière boucle change
    else Stop = lim ;//sinon on lit tous le Buffer

    for(int j = 0 ; j <= Stop - 1 ; j++){ //on va lire char par char

     if(state == 0){ //si on est dans la partie du taille
      taille[posi] = Buff.espace_donnee[j] ; //on l'enregistre dans une chaine pour aprés calculer le length
      posi++ ;
     }

     if(state == 2) eff = Buff.espace_donnee[j] ; //on veut savoir si on va lire la donnée ou pas avec eff

     if(state == 3 && eff == '1') printf(" ");//la donnée se montre effacé , on écrit un espace
     else printf("%c",Buff.espace_donnee[j]);//dans les autres cas , on écrit simplement

     posi_sauv++; //posi_sauv montre combien on a écrit dans un mod state

     if((posi_sauv == 3 && (state == 0 || state == 1)) || (posi_sauv == 1 && state == 2) || (posi_sauv == length && state == 3)){ //si on termine d'écrire une partie (taille , clé , eff , data)
      //on a terminé d'écrire une partie
      if(state == 0){ //si on a écrite toute la chaine taille
       chiffrement(taille , &length) ;//length va avoir la taille de donnée arrivant
       posi = 0 ; //rénitialisation de position de taille[]
      }

      printf(" || ");//diviser les parties
      posi_sauv = 0 ;
      state++ ;
      state = state % 4 ;//state ne doit pas dépasser 4 , il doit toujours boucler les parties taille , clé , eff , Data
     }

    }
    if(Entete_L(nomf , 1) != 0 ) fseek(f , Buff.Next_Block , SEEK_SET);//aller vers prochain Block si c'était une Liste
   }

 fclose(f) ;
 return ;
}

void Lect_T_NO_F(char *nomf){ //lire un T/of

enr_T Buff ;//Buffer

FILE *f = fopen(nomf , "rb") ;
 if(f == NULL) return ;
fseek(f , sizeof(Ent_T) , SEEK_SET) ; //sauter l'entète , trés important!!

 for(int k = 1 ; k <= Entete_T(nomf , 1) ; k++){ //un simple parcourt entre les blocs
  fread(&Buff , sizeof(Buff) , 1 , f);
  printf("\nBloc %d : ",k);
   for(int l = 0 ; l <= Buff.nbr - 1 ; l++) printf("cle:%d i:%d j:%d eff:%d||",Buff.Tab[l].cle , Buff.Tab[l].i , Buff.Tab[l].j , Buff.Tab[l].eff );
 }

 fclose(f) ;
 return ;
}

void Chargement_L_NO_VC(char *nomf , int tai){ //Créer un L/OVC de nombre de datas connus (tai)

printf("\nSi Vous voulez Ajouter des noms et prenoms Manuelement , Vous pouvez faire ecrire la donnee comme ceci : \n");
printf("Info_Nom_Prenom \t En ajoutant '_' , c est le critere de separation du champ Info dans ce programme\n");
printf("**Ajouter un nom ou un prenom est optionel**\n") ;

int Tete , i = 1 , j = 0 , cle ; //last_posi pour savoir où on est dans le Buffer pour chaque écriture , ref pourr des simplifier les calculs dans la boucle
char Data[lim - 7] , cle_Buff[3] , length[3] , ch[lim] ; //la donnée et la clé
FILE *f = fopen(nomf , "wb");
fclose(f) ;
 if(f == NULL) return ;

 Tete = sizeof(Ent_L) ; //initialiser la tète , le premier saut pour aller au premier Block , you can put anything , surtout verfyi berk belli rake sure marake 7a ttouchi l'entète sinon yro7 Koullesh
 aff_entete_L(nomf , 1 , Tete);//tete de fichier Liste
 aff_entete_L(nomf , 2 , 0);//initialiser la dernière position faisable à 0
 aff_entete_L(nomf , 3 , tai) ;//nombre de données existantes , initialisation
 aff_entete_L(nomf , 4 , 0) ;//nbr supprimé (=0 en création)
 aff_entete_L(nomf , 5 , 1); //nbr de bloques

 for(int k = 1 ; k <= tai ; k++){///chaque boucle il y aura une donnée prise
  printf("\n**Donnee la cle sous forme de XXX (chiffres): ");
  scanf("%d",&cle) ;
  Dechiffrement(cle , cle_Buff) ;
  printf("Ecrivez votre Data (Maximum %d - 7 = %d caracteres): ", lim , lim - 7 );
  scanf("%s",Data);
  Dechiffrement(strlen(Data) , length) ; ///Length maitenant a la taille de donnée sous forme d'une chaine

  strncpy(ch , length , 3) ;//copier la taille
  strncpy(ch + 3, cle_Buff , 3) ;//cle
  strcpy(ch + 6 , "0") ;//eff
  strncpy(ch + 7, Data , strlen(Data)) ;//info
  ch[strlen(Data) + 7] = '\0' ;///ceci pour qu'on peut la rendre une vrai chaine C (son strlen est calculable par exemple)

  Ecrire_chaine_L(nomf , &i , &j  , ch) ;///ecriture

 }

}

void Chargement_Aleatoir_CLE_ORDONNEE(char *nomf , int tai){ //Créer et générer des données aléatoire dans un L!OVC avec clées oordonnee (Hors TP , mais mieux que rien) , les infos ici sont plus longue

int Tete , cle_cpt = 0 , random_length , cpt_existe = 0 , i = 1 , j = 0 ; ///cpt_existe pour calculer le nombre de données existantes
char Data[lim-7] , cle[3] , length[3] , random_eff , ch[lim] ; //la donnée et la clé , eff aleatoire
srand(time(NULL)) ; //initialisation pour génération aleatoire

FILE *f = fopen(nomf , "wb");
 if(f == NULL) return ;
fclose(f) ;

 Tete = sizeof(Ent_L) ; //initialiser la tète , le premier saut pour aller au premier Block , you can put anything , surtout verfyi berk belli rake sure marake 7a ttouchi l'entète sinon yro7 Koullesh
 aff_entete_L(nomf , 1 , Tete);//tete de fichier Liste
 aff_entete_L(nomf , 2 , 0);//initialiser la dernière position faisable à 0
 aff_entete_L(nomf , 5 , 1); //nbr de bloques

 for(int k=1 ; k<=tai ; k++){
  random_length = rand() % (lim - 8) + 1 ; ///taille de chaine aleatoire
  printf("Length will be %d  ",random_length);
  Dechiffrement(random_length , length) ;
  Dechiffrement(cle_cpt , cle) ; ///cle prete


  random_eff = rand() % 2 + 48 ; ///soit '0' ou bien '1' , le champ eff est pret
  printf("eff will be :%c   ",random_eff) ;
  if(random_eff == '0') cpt_existe++ ;

  Random_String(random_length , Data) ; ///champ info est pret
  printf("the string will be :%s\n",Data) ;

  strncpy(ch , length , 3) ;
  strncpy(ch + 3, cle , 3) ;
  strncpy(ch + 6 , &random_eff , 1) ;
  strncpy(ch + 7, Data , strlen(Data)) ; ///Copier tous vers une chaine ch
  ch[strlen(Data) + 7] = '\0' ;///mème commentaire du module d'avant

  Ecrire_chaine_L(nomf , &i , &j  , ch) ;

  cle_cpt ++ ;
 }

 aff_entete_L(nomf , 3 , cpt_existe) ;
 aff_entete_L(nomf , 4 , tai - cpt_existe) ;
}

void Chargement_Aleatoir_NOM_PRENOM(char *nomf , int tai){ //Créer et générer des données aléatoire dans un L!OVC avec des noms et prenoms

int Tete , random_length , cpt_existe = 0 , i = 1 , j = 0 , randome_X , High_amp = 500 , Low_amp = 498 ; ///Low amp et High amp pour créer des clés non ordonnees aleatoires
char Data[lim-7] , cle[4] , length[3] , random_eff , randomName[15] , randomSurName[15] , ch[lim] ; //la donnée et la clé , eff aleatoire

///voici cette liste prédifinie , Modifiez la mais respectez le define de MaxLength de Top de programme
char* Tab_Names[20] = {"Tati" , "Hamroune" , "Nadia" , "Adam" , "Chennoun" , "Hebba" , "AitAmrane" , "AitAllawa" , "AitAhmad" , "Hamadesh" , "Khelouat" , "Samiya" , "NourlHouda" , "Hadia" , "Messaoude" , "Amina" , "AitMoussa" , "Kamelia" , "Roukaya" , "Malika"};
char* Tab_SurNames[20] = {"Youcef" , "Ali" , "ElAlia" , "Mohammed" , "Luma" , "Ines" , "Rachid" , "Ahmad" , "Sahrawi" , "Maya" , "Benchrif" , "Gaya" , "Meriem" , "Imane" , "Amal" , "Zeyneb" , "Karima" , "Orakrice" , "Chiraz" , "Nahida"} ;

srand(time(NULL)) ; //initialisation pour génération aleatoire

FILE *f = fopen(nomf , "wb");//ecraser le fichier
fclose(f) ;
 if(f == NULL) return ;

 Tete = sizeof(Ent_L) ; //initialiser la tète , le premier saut pour aller au premier Block , you can put anything , surtout verfyi berk belli rake sure marake 7a ttouchi l'entète sinon yro7 Koullesh
 aff_entete_L(nomf , 1 , Tete);//tete de fichier Liste
 aff_entete_L(nomf , 2 , 0);//initialiser la dernière position faisable à 0
 aff_entete_L(nomf , 5 , 1); //nbr de bloques

 for(int k = 1 ; k <= tai ; k++){
  random_length = rand() % (lim - 35) + 1 ; ///taille de info aleatoire , lim - 7 - 10x2 - 2x'_' = lim - 29 donc pour avoir une info assez respectant la taille , 35
  randome_X = rand() % 20 ;
  strcpy(randomName , Tab_Names[randome_X]);///choisir en random un nom
  randome_X = rand() % 20 ;
  strcpy(randomSurName , Tab_SurNames[randome_X]);///choisir en random un prenom

  if(k % 2 == 0){ ///Creation du clee aleatoires non ordonnees , soit on prend l'interval du high amp , sinon low amp , malheureusement , on peut juste créer 500 clés uniques avec , aprés c'est une repetition
   randome_X = rand() % 2 + High_amp ;
   High_amp = High_amp + 2 ;
  } else {
    randome_X = rand() % 2 + Low_amp ;
    Low_amp = Low_amp - 2 ;
  }

  Dechiffrement(randome_X , cle);
  cle[3] = '\0' ;
  printf("Key : %s  ",cle);

  printf("Info Length: %d  ",random_length);
  printf("Nom et Prenom: %s  %s  ",randomName , randomSurName) ;

  random_eff = rand() % 2 + 48 ; //soit '0' ou bien '1' , le champ eff est pret
  printf("eff :%c   ",random_eff) ;
  if(random_eff == '0') cpt_existe++ ;

  Random_String(random_length , Data) ; //champ info est pret
  printf("Info:%s\n",Data) ;
  random_length = random_length + strlen(randomName) + strlen(randomSurName) + 2 ;//la vrai taille c'est info+nom+prenom+2 pour '_' deux fois
  Dechiffrement(random_length , length) ; //taille d info prète


  strncpy(ch , length , 3) ;//champ taille
  strncpy(ch + 3, cle , 3) ;//cle
  strncpy(ch + 6 , &random_eff , 1) ;//eff
  strncpy(ch + 7, Data , strlen(Data)) ;//info
  strcpy(ch + 7 + strlen(Data) , "_") ;//une séparation "_"
  strcpy(ch + 8 + strlen(Data) , randomName);//le Nom
  strcpy(ch + 8 + strlen(Data) + strlen(randomName) , "_") ;//une séparation "_"
  strcpy(ch + 9 + strlen(Data) + strlen(randomName) , randomSurName);//le Prenom

  ch[random_length + 7] = '\0' ;

  Ecrire_chaine_L(nomf , &i , &j  , ch) ;

 }

 aff_entete_L(nomf , 3 , cpt_existe) ;
 aff_entete_L(nomf , 4 , tai - cpt_existe) ;
}

bool Recherche_L_NO_VC(char *nomf , char *cle , int *taille_chaine , bool *trouv_cle , int *i , int *j ){//rechercher une chaine dont la clé est connu et savoir sa taille , sa position , de son premier caractère de Data

int i_sauv , j_sauv , tai =  Entete_L(nomf , 3) + Entete_L(nomf , 4) ;

char ch[lim] , cle_Buff[3] , taille_Buff[3] , eff ; //pour lire les propriétés du donnée
*j = 0 ; //initialisation
*i = 1 ;

  for(int k = 1 ; k <= tai ; k++){ //while mal7e9tsh la fin
   //pour chaque boucle , on extrait et on saute , tout est fait dans la fonction extraire_chaine
   i_sauv = *i ;
   j_sauv = *j ;
   Extraire_chaine(nomf , 3 , i , j , taille_Buff );
   Extraire_chaine(nomf , 3 , i , j , cle_Buff );
   Extraire_chaine(nomf , 1 , i , j , &eff );
   chiffrement(taille_Buff , taille_chaine);
    if(strncmp(cle , cle_Buff , 3) == 0 && eff == '0'){ //simple comparaison
     *trouv_cle = true ;
     *i = i_sauv ;
     *j = j_sauv ;
     return true ; //on a trouvé la donnée avec toute les paramètre mis à jour
    } else if(strncmp(cle , cle_Buff , 3) == 0 && eff == '1'){ //si on a trouvé la clé mais  la donnée est effacée
     *trouv_cle = true ;
     *i = i_sauv ;
     *j = j_sauv ;
     return false ; //on n'a pas trouvé la donnée avec toute les paramètre mis à jour
    }

   Extraire_chaine(nomf , *taille_chaine , i , j , ch ); //sinon on saute la chaine
  }
 *trouv_cle = false ;
 return false ; //si on est arrivé à la fin , alors on a pas trouvé la donnée
}

void Insertion_L_NO_VC(char *nomf , char *cle , char *Data ){///Simple insertion + Reecriture du donnee si l utilisateur a inseree la meme cle

int i , j , t ;
bool trouv_cle ;
char taille_chaine[4]  ;
Dechiffrement(strlen(Data) , taille_chaine) ;//rendre l'entier une chaine
taille_chaine[3] = '\0' ;

if(Recherche_L_NO_VC(nomf , cle , &t ,&trouv_cle , &i ,&j) == true) return ; //element existe déja
//sinon , on fait les 4 écritures du données ou bien si la clé existe enlever le eff
if(trouv_cle == false){ //si la clé n'existait pas on insère la nouvelle donnée

  Ecrire_chaine_L(nomf , &i , &j  , taille_chaine);
  Ecrire_chaine_L(nomf , &i , &j  , cle) ;
  Ecrire_chaine_L(nomf , &i , &j , "0");
  Ecrire_chaine_L(nomf , &i , &j  , Data) ;

} else  { //si la clé existe , la réinsèretion se fait en priorité et la nouvelle donnée ne va pas apparaitre

 Extraire_chaine(nomf , 3 , &i , &j , taille_chaine) ;
 Extraire_chaine(nomf , 3 , &i , &j , cle) ;
 Ecrire_chaine_L(nomf , &i , &j , "0") ;
}

   aff_entete_L(nomf , 3 , Entete_L(nomf , 3) + 1) ;
}

void Insertion_T_NO_F(char *nomf , Data info){ ///Insérer une donnée dans T!OF
enr_T Buff ;
int a = Entete_T(nomf , 1) , c = Entete_T(nomf , 2) ;
bool new_block = false ;

FILE *f = fopen(nomf , "rb+");
if(f == NULL) return ;

fseek(f , sizeof(Ent_T) + (a - 1)*sizeof(Buff) , SEEK_SET) ;///allez à aieme bloque
fread(&Buff , sizeof(Buff) , 1 , f) ;
if(c == 0) Buff.nbr = 0 ;

 if(Buff.nbr == b){///Soit bloque plein
  Buff.Tab[0] = info ;
  Buff.nbr = 1 ;
  new_block = true ;
  fseek(f , sizeof(Ent_T) + a*sizeof(Buff) , SEEK_SET) ;
 } else { ///ou bien il existe une place
  Buff.Tab[Buff.nbr] = info ;
  Buff.nbr++;
  fseek(f , - (long) sizeof(Buff) , SEEK_CUR) ;
 }

 fwrite(&Buff , sizeof(Buff) , 1 , f);
 fclose(f) ;

 if(new_block) aff_entete_T(nomf , 1 , a + 1);
 aff_entete_T(nomf , 2 , c + 1) ;


}

void Suppression_Physisque_L_NO_VC(char *nomf , char *cle){//suppression physique

bool trouv_cle , stop = false ;
int k , i_posi , i_j , j , ta , posi , Length , nbr_bloc = Entete_L(nomf , 5) , last_posi = Entete_L(nomf , 2) ;// posi est la première position du caractère à décaler prochainement
char Data[lim] ;

if(Recherche_L_NO_VC(nomf , cle , &ta , &trouv_cle , &i_j , &j) == false) return ; //on sort de programme si la donnée n'existe pas

//here starts the true suppression
i_posi = i_j ; //on initialse le bloque de posi

posi = j + ta + 7 ; //on se positionnent aprés la donnée , le premier caractère à décaler
if(posi >= lim){
 posi = posi - lim ;//bien sur si ça déborde on retourne au premier puis terminer au prochain bloque
 i_posi++ ;
}

/*Avec ceci , le vide commence à j dans le Bloque i , le premier caractère à décaler est à posi
 L'idée sera juste à extraire chaine de la mème taille et la déposer dans le vide et refaire cela jusqu'a la fin */

 if( i_j == nbr_bloc && posi == last_posi ) stop = true ; //cas particulier , aucune donnée à décalera

 while( !stop ){//tant qu'on est pas dans le dernier Block

  k = posi ; //sauvegarder la position
  Extraire_chaine(nomf , ta + 7 , &i_posi , &posi , Data) ;


   if( i_posi > nbr_bloc || ( i_posi == nbr_bloc &&  posi >= last_posi ) ){ //le vide est arrivé à la fin

    Length = last_posi - k ; //c'est le nombre de caractères restantes
    if(Length <= 0 ) Length = Length + lim ;
    stop = true ;

   } else Length = ta + 7 ;

     Data[Length] = '\0' ;
   Ecrire_chaine_L(nomf , &i_j , &j , Data) ;//on écrit la donnée
 }

aff_entete_L(nomf , 2 , last_posi - ta - 7) ;

 if(Entete_L(nomf , 2) <= 0){ //le bloque est vide
 if(Entete_L(nomf , 2) != 0) aff_entete_L(nomf , 2 , Entete_L(nomf , 2) + lim ) ;
 aff_entete_L(nomf , 5 , Entete_L(nomf , 5) - 1 ) ; //nombre de bloque diminue
 }

aff_entete_L(nomf , 3 , Entete_L(nomf , 3) - 1 ) ; //on décrémente nombre de info insérés

}

void Suppression_logique_L_NO_VC(char *nomf , char *cle){ // et logique

int i , j , ta ;
char ch[lim] ;
bool trouv_cle ;

 if(Recherche_L_NO_VC(nomf , cle , &ta , &trouv_cle , &i , &j) == false) return ; //on sort de programme si la donnée n'existe pas

Extraire_chaine(nomf , 6 , &i , &j , ch); //extraire : taille , cle et se positionner à eff
Ecrire_chaine_L(nomf , &i , &j , "1") ;

aff_entete_L(nomf , 3 , Entete_L(nomf , 3) - 1) ;
aff_entete_L(nomf , 4 , Entete_L(nomf , 4) + 1) ;


}


//------------------------------------- PARTIE MODULES TP1 + 1/2TP2--------------------------------------------------------------------


void Decoupage_L_NO_VC(char *nomf_L_O , char *nomf_T , char *nomf_L_N , Data *Tab_Indexe){ ///Extraire de L!OVC un T!OF et T!OVC et charger la table d indexe

int tai =  Entete_L(nomf_L_O , 3) + Entete_L(nomf_L_O , 4) ;///donnee supprimee logiquement + non supprimee = nbr_donnee

 FILE *f_lo = fopen(nomf_L_O , "rb") ;//lecture
 FILE *f_t = fopen(nomf_T , "wb") ;//ecrasement
 FILE *f_ln = fopen(nomf_L_N , "wb") ;//ecrasement

  fclose(f_ln) ;
  fclose(f_lo) ;
  fclose(f_t) ;///pas besoin de ces pointeurs , c'est juste pour l'initialisaion , ils peuvent ètre supprimé normalement , mais ana berk kheft

 aff_entete_L(nomf_L_N , 1 , 0);//tete de fichier T/OVC , inutile pour ce cas
 aff_entete_L(nomf_L_N , 2 , 0);//initialiser la dernière position faisable à 0
 aff_entete_L(nomf_L_N , 3 , Entete_L(nomf_L_O , 3)) ;//nombre de données existantes , initialisation
 aff_entete_L(nomf_L_N , 4 , 0) ;//nbr supprimé (=0 en création)
 aff_entete_L(nomf_L_N , 5 , 0); //nbr de bloques

 aff_entete_T(nomf_T , 1 , 1) ;///nombre de bloque , 0 peut poser des problemes
 aff_entete_T(nomf_T , 2 , 0) ;
 aff_entete_T(nomf_T , 3 , 0) ;

Data info ;

char ch_d[lim - 7] , cle_Buff[4] , taille_Buff[4] , eff , ch[lim] ; //pour lire les propriétés du donnée
int l = 0 , k = 1 , length , m = 1 , n = 0 ;

  for(int z = 0 ; z < tai ; z++) { //while mal7e9tsh la fin
   info.i = k ; //sauvegarder le i et j
   info.j = l ;

   Extraire_chaine(nomf_L_O , 3 , &k , &l , taille_Buff );
   Extraire_chaine(nomf_L_O , 3 , &k , &l , cle_Buff );
   Extraire_chaine(nomf_L_O , 1 , &k , &l , &eff );
   chiffrement(taille_Buff , &length);

   cle_Buff[3] = '\0' ;
   taille_Buff[3] = '\0' ;

   chiffrement(cle_Buff , &info.cle) ;
   if(eff == '0') info.eff = false ;
   else info.eff = true ;

   //(----------------- TP2 ------------------)
   Tab_Indexe[z] = info ; ///charger l element aux table d'indexe primaire
   ///-----------------------------------

   Insertion_T_NO_F(nomf_T , info) ;//en parallèle on le sauvegarde dans le fichier T/OF

  Extraire_chaine(nomf_L_O , length , &k , &l , ch_d );
  ch_d[length] = '\0' ;

  printf("%s %s %c %s   ",taille_Buff , cle_Buff , eff , ch_d) ;//affichage
    if(eff == '0'){
    printf("WRITING!!\n") ;

    strncpy(ch , taille_Buff , 3) ;
    strncpy(ch + 3, cle_Buff , 3) ;
    strcpy(ch + 6 , "0") ;
    strncpy(ch + 7 , ch_d , length) ;

    ch[length + 7] = '\0' ;
    Ecrire_chaine_T(nomf_L_N , &m , &n  , ch) ;///Ecriture en T/OVC

    } else printf("NO WRITING !!\n") ;
  }
}


//------------------------------------- PARTIE MODULES TP2 ----------------------------------------------------------------------------------

void Recherch_Dicho_ind(Data *Tab_index , int key , int left , int right , int *posi){//recherche dichotomique dans tableau d'indexe dans la borne [left .. right]
int inf = left , sup = right ;
bool trouv = false ;

int j = 0 ;

 while(inf <= sup && !trouv){
  j = (int) (sup + inf)/2 ;

   if(Tab_index[j].cle == key) trouv = true ;
   else{
    if(key > Tab_index[j].cle ) inf = j + 1 ;
    else sup = j - 1 ;
   }
 }

 if(!trouv) j = inf ;
 *posi = j ;
}

void Fusion(Data *Tab_Indexe , int left, int mid, int right , int champ) { //Faire trier deux parties du tableau bornée avec [left..right] à un milieu mid en utilisiant deux tableaux , tous ça pour un seul champ

 if(champ > 4){ //il y a pas plus de 4 champs
  printf("Il y a une erreur dans Votre Fusion...\n") ;
  return ;
 }

 int i = 0 , j = 0 , k = left , n1 = mid - left + 1 , n2 = right - mid; //n1 pour la fin du tableau L et le meme n2 pour R
 Data L[n1], R[n2]; ///Ces deux sont le coeur de programme

/// Copier les deux morceaux Vers L et R
for(int y = 0; y < n1; y++) L[y] = Tab_Indexe[left + y];
for(int z = 0; z < n2; z++) R[z] = Tab_Indexe[mid + 1 + z];

///Mais on va comparer selon le champ , avec ça on a un tri Fusion generalisée
 switch(champ){
  case 1:///On va Fusionner Ces Deux tableaux en un pour avoir Tableau triée d'aprés le champ
      ///..cle
  while (i < n1 && j < n2) {
   if (L[i].cle <= R[j].cle) {
     Tab_Indexe[k] = L[i];
     i++;
    } else {
     Tab_Indexe[k] = R[j];
     j++;
    }
     k++;
    }

   while (i < n1) { //Copier le reste
   Tab_Indexe[k] = L[i];
   i++;
   k++;
   }
  while (j < n2) {
  Tab_Indexe[k] = R[j];
  j++;
  k++;
  }

  break ;
  case 2:/// ..eff
  while (i < n1 && j < n2) {
   if (L[i].eff <= R[j].eff) {
     Tab_Indexe[k] = L[i];
     i++;
    } else {
     Tab_Indexe[k] = R[j];
     j++;
    }
     k++;
    }

   while (i < n1) { //Copier le reste
   Tab_Indexe[k] = L[i];
   i++;
   k++;
   }
  while (j < n2) {
  Tab_Indexe[k] = R[j];
  j++;
  k++;
  }

  break ;
  case 3:/// i
  while (i < n1 && j < n2) {
   if (L[i].i <= R[j].i) {
     Tab_Indexe[k] = L[i];
     i++;
    } else {
     Tab_Indexe[k] = R[j];
     j++;
    }
     k++;
    }

   while (i < n1) { //Copier le reste
   Tab_Indexe[k] = L[i];
   i++;
   k++;
   }
  while (j < n2) {
  Tab_Indexe[k] = R[j];
  j++;
  k++;
  }

  break ;
  case 4:/// j
  while (i < n1 && j < n2) {
   if (L[i].j <= R[j].j ) {
     Tab_Indexe[k] = L[i];
     i++;
    } else {
     Tab_Indexe[k] = R[j];
     j++;
    }
     k++;
    }

   while (i < n1) { //Copier le reste
   Tab_Indexe[k] = L[i];
   i++;
   k++;
   }
  while (j < n2) {
  Tab_Indexe[k] = R[j];
  j++;
  k++;
  }

  break ;
 }


}

void Tri_Fusion(Data *Tab_Indexe , int Bgauche , int Bdroite , int champ) {///champs seront : 1 pour cle , 2 pour eff , 3 pour i , 4 pour j
    if (Bgauche < Bdroite) {
        int mid = Bgauche + (Bdroite - Bgauche) / 2;

        // Trier les deux parties (Recursive)
        Tri_Fusion(Tab_Indexe , Bgauche, mid , champ);
        Tri_Fusion(Tab_Indexe , mid + 1, Bdroite , champ);
        // Fusionner les deux parties
        Fusion(Tab_Indexe, Bgauche, mid, Bdroite , champ);
    }
}

void Reorganisation_Indexe(char *nomf , Data *Tab_Indexe){///enlever tous les éléments supprimés logiquement en utilisant un indexe

char ch[lim] , taille_Buff[3] ; //pour lire les propriétés du donnée , 4ieme caractère pour \0
int length , i1 , j1 , i2 = 1 , j2 = 0 , tai =  Entete_L(nomf , 3) + Entete_L(nomf , 4) ; //tai = nbr totale de donnee supprimee logiquent et non supprimee

FILE *f2 = fopen("brouillant.bin" , "wb") ; //écraser tous le contenu de Brouillant pour le réétuliser
fclose(f2) ;
//initialiser l'entète de Brouillant
//Brouillant va ètre le fichier résultat , puis on va le copier vers fichier principal , c'est juste un fichier local
aff_entete_L("brouillant.bin" , 1 , sizeof(Ent_L)) ; //on met positionne la location de premier Block pour qu'il n'intercepte pas avec l'entète
aff_entete_L("brouillant.bin" , 2 , 0) ;
aff_entete_L("brouillant.bin" , 3 , Entete_L(nomf , 3)) ;
aff_entete_L("brouillant.bin" , 4 , 0) ;
aff_entete_L("brouillant.bin" , 5 , 0) ;

Tri_Fusion(Tab_Indexe , 0 , tai - 1 , 2);//tri par eff dans la table de fusion sur tous l interval
tai = Entete_L(nomf , 3) ; //tai = nbr donnee non supprimee logiquement
Tri_Fusion(Tab_Indexe , 0 , tai - 1 , 3 );//On tri par rapport à i sur l'interval du [0 ; nbr_don_non_suprim - 1] = {j / Indexe[j].eff = 0}

 for(int k = 0 ; k < tai ; k++){ ///un parcourt Dans juste L'interval non effacee , Plus performant

  i1 = Tab_Indexe[k].i ;
  j1 = Tab_Indexe[k].j ;

   //On lit tous les propriétés de l'info

   Extraire_chaine(nomf , 3 , &i1 , &j1 , taille_Buff );
   chiffrement(taille_Buff , &length);
   i1 = Tab_Indexe[k].i ;
   j1 = Tab_Indexe[k].j ;

   Extraire_chaine(nomf , length + 7 , &i1 , &j1 , ch );//extraction totale
   ch[length + 7] = '\0' ;
   Ecrire_chaine_L("brouillant.bin" ,&i2 , &j2 , ch) ;//ecriture directe , car on sait qu on a triee notre tableau

 }

 fcpy("brouillant.bin" , nomf) ; //copier le résultat vers le fichier principale
}

//----------------------------------------------- PARTIE BONUS ---------------------------------------------------------------

typedef struct List{ //Liste chainee de noms ou bien prenom
int Key ;
struct List *nextList ;
}*maill ;

typedef struct Tsecond{ //enregistrement du tete de la liste
char Name[20] ;
maill teteList ;
maill Dernier_Maill ; //pour pointer au dernier element du liste
}Data_Name ; //exemple : Data_Name[20] est un tableau tq chaque position porte un nom et pointe vers une liste des cles

maill Next(maill p){
return p->nextList ;
}

void allouer(maill *p){
    *p = malloc(sizeof(struct List));
    (*p)->nextList = NULL ;
}

void aff_adr(maill p , maill q){
p->nextList = q ;
}

void Initialisation_Tables_Sec(Data_Name *Nom , Data_Name *Prenom ){ //initialiser les pointeurs à NULL pour savoir aprés les etats des positions

 for(int i = 0 ; i < MaxLength ; i++){
  Nom[i].Dernier_Maill = NULL ;
  Nom[i].teteList = NULL ;
  Prenom[i].Dernier_Maill = NULL ;
  Prenom[i].teteList = NULL ;
 }
}

bool find_Name(Data_Name *Nom , char *name , int *i){ ///recherche du nom si il existe dans le tableau , pas la liste

*i = 0 ;

 while(Nom[*i].Dernier_Maill != NULL && *i < MaxLength){ ///simple parcourt du tableau
  if(strcmp(Nom[*i].Name , name) == 0) return true ;
  *i = *i + 1 ;
 }

 return false ;///si on est là , on a pas trouvee le nom
}

void Charger_Tables_Secondaires(char *nomf , Data_Name *Nom , Data_Name *Prenom , Data *Tab_Indexe){///Chergement de Table secondaires

maill p , q ;
Initialisation_Tables_Sec(Nom , Prenom) ;

char ch[lim] , taille_Buff[3] , nom[20] , prenom[20] ; //pour lire les propriétés du donnée
int length , alpha , beta , i1 , j1 , tai =  Entete_L(nomf , 3) + Entete_L(nomf , 4) ; //tai = nbr totale de donnee supprimee logiquent et non supprimee
bool Name_Found = false , Prenom_Found = false ;

Tri_Fusion(Tab_Indexe , 0 , tai - 1 , 2);//tri par eff dans la table de fusion sur tous l interval
tai = Entete_L(nomf , 3) ; //tai = nbr donnee non supprimee logiquement
Tri_Fusion(Tab_Indexe , 0 , tai - 1 , 3 );//On tri par rapport à i sur l'interval du [0 ; nbr_don_non_suprim - 1] = {j / Indexe[j].eff = 0}

 for(int k = 0 ; k < tai ; k++){ //un parcourt Dans juste L'interval non effacee

  i1 = Tab_Indexe[k].i ;
  j1 = Tab_Indexe[k].j ;

   //On lit tous les propriétés de l'info

   Extraire_chaine(nomf , 3 , &i1 , &j1 , taille_Buff );
   chiffrement(taille_Buff , &length);
   i1 = Tab_Indexe[k].i ;
   j1 = Tab_Indexe[k].j ;

   Extraire_chaine(nomf , length + 7 , &i1 , &j1 , ch );//extraction totale
   ch[length + 7] = '\0' ;

   //PARTIE CHARGEMENT AUX TABLEAU SECONDAIRES
   alpha = 7 ; //on commence a la position 7 pour commencer s il ya un nom et prenom par le Critère d'arret '_' tiret de 8
   Name_Found = false ;
   Prenom_Found = false ;

    while(alpha < length + 6 && (!Name_Found || !Prenom_Found)){ //parcourt de la chaine
     if(ch[alpha] == '_'){
      alpha++ ;
      if(Name_Found){ //si on a deja trouve le Nom , on va inserer un prenom
        beta = 0 ;
        for(int z = 0 ; ch[z + alpha] != '_' && z + alpha < length + 7 ; z++){
         prenom[z] = ch[z + alpha];
         beta++ ;
        }
        alpha = alpha + beta ;
        if(beta != 0) Prenom_Found = true ; //on a ecrit au moins une lettre , donc le champ de nom a existee
        prenom[beta] = '\0' ;

      } else { //sinon c'est un nom
        beta = 0 ;
        for(int z = 0 ; ch[z + alpha] != '_' && z + alpha < length + 7 ; z++){
         nom[z] = ch[z + alpha];
         beta++ ;
        }
        alpha = alpha + beta ;
        if(beta != 0) Name_Found = true ; //on a ecrit au moins une lettre , donc le champ de nom a existee
        nom[beta] = '\0' ;
      }
     } else alpha++ ;

    }

    if(Name_Found){//on a trouve un nom
     if(find_Name(Nom , nom , &alpha)){ //ce nom existe déja dans le tableau
       p = Nom[alpha].Dernier_Maill ; //aller a la fin de liste de ce nom
       allouer(&q) ;//creer un maillon
       q->Key = Tab_Indexe[k].cle ; //affecter la nouvelle cle
       aff_adr(p , q) ; //lier
       Nom[alpha].Dernier_Maill = q ; //la nouvelle dernière maillon
       printf("Name %s:Inserting %d \n" ,Nom[alpha].Name , Tab_Indexe[k].cle);

      } else { //ce nom est nouveau , on l insere avec sa première cle
      strcpy(Nom[alpha].Name , nom); //copier la chaine
      allouer(&p) ; //creer un maillon
      Nom[alpha].Dernier_Maill = p ;
      Nom[alpha].teteList = p ;
      p->Key = Tab_Indexe[k].cle ; //affecter la nouvelle cle
      printf("New Name:%s , inserting %d\n",Nom[alpha].Name , Tab_Indexe[k].cle);
      }
    }

    if(Prenom_Found){ //on a trouve un prenom
      if(find_Name(Prenom , prenom , &alpha)){ //ce nom existe déja dans le tableau
       p = Prenom[alpha].Dernier_Maill ; //aller a la fin de liste de ce nom
       allouer(&q) ;//creer un maillon
       q->Key = Tab_Indexe[k].cle ; //affecter la nouvelle cle
       aff_adr(p , q) ; //lier
       Prenom[alpha].Dernier_Maill = q ; //la nouvelle dernière maillon
       printf("Prename %s:Inserting %d \n" ,Prenom[alpha].Name , Tab_Indexe[k].cle);
      } else { //ce nom est nouveau , on l insere avec sa première cle
      strcpy(Prenom[alpha].Name , prenom); //copier la chaine
      allouer(&p) ; //creer un maillon
      Prenom[alpha].Dernier_Maill = p ;
      Prenom[alpha].teteList = p ;
      p->Key = Tab_Indexe[k].cle ; //affecter la nouvelle cle
      printf("New PreName:%s , inserting %d\n",Prenom[alpha].Name , Tab_Indexe[k].cle);
      }
    }
 }

}

void Lect_Secondary_Tab(Data_Name *Nom ){///Lire un Table D indexe secondaire

maill p ;
int i = 0 , cpt = 0 ;

 while(Nom[i].teteList != NULL && i < MaxLength){
  printf("%s --> ",Nom[i].Name);
  p = Nom[i].teteList ;
   while(p != NULL){
    printf("%d -> ",p->Key);
    p = Next(p) ;
    cpt++ ;
   }
   printf("N\n") ;
   i++ ;
 }
 printf("Wow , Il avait eut %d insertions dans ce tableau O_O\n",cpt);
}

void Recherche_Name_Prename(char *nomf , char *nom , char *prenom , Data_Name *Nom , Data_Name *Prenom , Data *Tab_Indexe){ ///faire recherche de nom et prenom dans un fichier en utilisant les indexes

int cpt = 0 , tai =  Entete_L(nomf , 3) + Entete_L(nomf , 4) , i = 0 , j = 0 , posi ;
bool Found = false ;
maill p , q ;

Tri_Fusion(Tab_Indexe , 0 , tai - 1 , 2);//tri par eff dans la table de fusion sur tous l interval
tai = Entete_L(nomf , 3) ; //tai = nbr donnee non supprimee logiquement
Tri_Fusion(Tab_Indexe , 0 , tai - 1 , 1 );//On tri par rapport à cle sur l'interval pour preparer la recherche dichotomique

 while(Nom[i].Dernier_Maill != NULL && i < MaxLength){ //recherche du nom
  if(strcmp(Nom[i].Name , nom) == 0){
   Found = true ;
   break ; //sortir du boucle
  }
  i++ ;
 }
 if(!Found){
  printf("Ce nom n existe pas dans le fichier...\n");
  return ;
 }
 Found = false ;

 while(Prenom[j].Dernier_Maill != NULL && j < MaxLength){ //recherche du prenom
  if(strcmp(Prenom[j].Name , prenom) == 0){
   Found = true ;
   break ; //sortir du boucle
  }
  j++ ;
 }
 if(!Found){
  printf("Ce nom n existe pas dans le fichier...\n");
  return ;
 }

//On a trouvee nos deux Listes , maitenant il suffit du faire un intersection pour trouver la clé qui leur unis

Found = false ;
p = Nom[i].teteList ;

 while(p != NULL ){//A chaque cle dans le nom
  q = Prenom[j].teteList ;
   while(q != NULL && !Found){//on parcourt la liste du prenom , un ptit peu couteut , O(n*n)
    if(q->Key == p->Key) Found = true ;
     else q = Next(q) ;
   }

   if(Found){
     //Sinon On a trouvee la cle
      Recherch_Dicho_ind(Tab_Indexe , q->Key , 0 , tai - 1 , &posi) ;
     if(cpt == 0)printf("\nvotre Nom et Prenom est dans : \n");
     printf(" - Cle : %d. la %dieme position logique de %dieme bloc .\n",q->Key , Tab_Indexe[posi].j , Tab_Indexe[posi].i) ;
     Found = false ;
     cpt++ ;
   }
   p = Next(p) ;
 }

printf("%d resultats trouvee ..\n", cpt);

}

bool Recherche_L_NO_VC_Indexe(char *nomf ,int key , Data *Tab_Indexe , int *i , int *j){ ///Recherche plus performante avec Table primaire

int tai =  Entete_L(nomf , 3) + Entete_L(nomf , 4) , posi ;

Tri_Fusion(Tab_Indexe , 0 , tai - 1 , 1);//tri par cle dans la table de fusion sur tous l interval pour une recherche dichotomique

 Recherch_Dicho_ind(Tab_Indexe , key , 0 , tai - 1 , &posi) ;
 if(posi >= tai){
    printf("votre cle n existe pas");
    return false ;
 }

 if(Tab_Indexe[posi].cle != key){
    printf("votre cle n existe pas");
    return false ;
 }

  *i = Tab_Indexe[posi].i ;
  *j = Tab_Indexe[posi].j ;
  printf("\nvotre Nom et Prenom est dans la %dieme position logique de %dieme bloc .",Tab_Indexe[posi].j , Tab_Indexe[posi].i) ;
  return true ;
}

void MENU(){ ///a menu
printf("-----------------------------  Menu  -----------------------------\n");
printf("----------------------------- PART : I -----------------------------\n");
printf("1)- Creer un fichier binaire manuelement L/OVC .\n");
printf("2)- Creer un fichier avec des donnees Aleatoires et clees ordonnes .\n");
printf("3)- Creer un fichier avec des donnees/clees Aleatoires + Nom/Prenom.\n");
printf("4)- Lire le fichier.\n") ;
printf("5)- Lire le Contenue de l entete.\n") ;
printf("6)- Inserer un donnee .\n") ;
printf("7)- Recherche Sequentielle d une donnee.\n") ;
printf("8)- Suppression physique d une donnee .\n") ;
printf("9)- Suppression logique d une donnee .\n") ;
printf("10)- Decouper le Fichier + Chargement du Table D indexe primaire.\n") ;
printf("11)- Lire le Fichier T/OF .\n") ;
printf("12)- Lire le Fichier T/OVC .\n") ;
printf("--------------------------------------------------------------\n\n");
printf("----------------------------- PART : II -----------------------------\n");
printf("13)- Afficher le Table D indexe primaire.\n");
printf("14)- Reorganisation du fichier primaire .\n");
printf("15)- Recherche dichotomiquement la cle avec Table indexe principale.\n");
printf("\n16)- Charger Tableaux secondaires de noms et prenoms .\n");
printf("17)- Afficher les tableaux d indexes secondaires.\n");
printf("18)- Recherche avec Tableaux secondaires le nom et prenom .\n");
printf("--------------------------------------------------------------\n\n");
printf("0)- Exit.\n");
printf("--------------------------------------------------------------\n\n");
}

int main()
{

int tai , state = 1 , choi , don ;
int i , j ;
Data Indexe[3000] ; //changez ceci si vous voulez charger plus de 3000 donnees
Data_Name Names[50] , Prenames[50] ;

bool trouv_cle , Decoup_pret = false ;
bool secondary = false ;
char cle_Buff[4] , Data[lim - 7] , nom[20] , prenom[20] ;

 while(state){
   system("cls") ;
   printf("voici votre menu : \n\n");
   MENU() ;

     printf("votre choix : ");
  scanf("%d",&choi);
     while( choi < 0 || choi > 18 || ( choi > 3 && Entete_L("file.bin" , 3) == 0) || ((choi >= 11 && choi <= 16) && Decoup_pret == false) || ((choi >= 17 && choi <= 18) && secondary == false) ){
      if(Entete_L("file.bin" , 3) == 0) printf("ERROR01: Votre fichier est vide !! le seul choix restant est 1 ou 2 ou 3 pour créer nouvelles donnees");
       else if((choi >= 11 && choi <= 16) && Decoup_pret == false) printf("\nERROR02: vous n avez pas encore fait le decoupage pour faire ces etapes") ;
       else if((choi >= 17 && choi <= 18) && secondary == false) printf("\nERROR03: vous n avez pas encore fait chargement de tableaux secondaires pour faire cette etape") ;
       else printf("\nErreur00 : choix non valable ");
      printf("\nReecesissez : ") ;
      scanf("%d",&choi);
     } //ce simple while est pour donner un choix correcte

   switch (choi){

    case 0:///Exit
     state = 0 ; //pour sortir de la grande boucle
    break ;

    case 1:///création de fichier L/OVC manuelement
     system("cls");
     if(lim >= 10){
      printf("!!! Attention , puisque la taille de cle est de 3 chiffres , on est limitee d avoir 1000 donnee uniques , puis il y aura des clees parasites.\nBien sur vous pouvez inserer plus , mais quelques modules basiques ne pourront pas fonctionner 100/100 correctement\n");
      printf("donnez le nombre de donnee qui va etre recu dans ce fichier: ");
      scanf("%d",&tai);

     Chargement_L_NO_VC("file.bin" , tai) ;
     printf("Vos donnee ont ete bien sauvegarde") ;
     } else printf("Il Semble Que Votre Fichier a un espace tres petit pour cette generation , vous pouvez la modifier sur les premires lignes du programmes (ce choi est faisable pour plus de 10 cractères au moins chaque bloque)\n");

     printf("\n\n Press <ENTER> to Continue...");
     Decoup_pret = false ;
     secondary = false ;

     fflush(stdin) ;
     getchar() ;
    break ;

     case 2:///création de fichier L/OVC en random et clées ordonnes

     system("cls");

     if(lim >= 10){
      printf("!!! Attention , puisque la taille de cle est de 3 chiffres , on est limitee d avoir 1000 donnee uniques , puis il y aura des clees parasites.\nBien sur vous pouvez inserer plus , mais quelques modules basiques ne pourront pas fonctionner 100/100 correctement\n");
      printf("donnez le nombre de donnee qui vont se generee en random dans ce fichier: ");
      scanf("%d",&tai);

     Chargement_Aleatoir_CLE_ORDONNEE("file.bin" , tai) ;
     printf("Vos donnee ont ete bien sauvegarde") ;
     } else printf("Il Semble Que Votre Fichier a un espace tres petit pour cette generation , vous pouvez la modifier sur les premires lignes du programmes (ce choi est faisable pour plus de 10 cractères au moins chaque bloque)\n");

     printf("\n\n Press <ENTER> to Continue...");
     Decoup_pret = false ;
     secondary = false ;

     fflush(stdin);
     getchar() ;
    break ;

    case 3:///création de fichier L/OVC en random et clées aleatoires avec Noms et prenoms

     system("cls");
     if(lim >= 35){
      printf("!!! Attention , puisque la taille de cle est de 3 chiffres , on est limitee d avoir 1000 donnee uniques , puis il y aura des clees parasites.\nBien sur vous pouvez inserer plus , mais quelques modules basiques ne pourront pas fonctionner 100/100 correctement\n");
      printf("donnez le nombre de donnee qui vont se generee en random dans ce fichier: ");
      scanf("%d",&tai);

     Chargement_Aleatoir_NOM_PRENOM("file.bin" , tai) ;
     printf("Vos donnee ont ete bien sauvegarde") ;
     } else printf("Il Semble Que Votre Fichier a un espace tres petit pour cette generation , vous pouvez la modifier sur les premires lignes du programmes (ce choi est faisable pour plus de 35 cractères au moins chaque bloque)\n");

     printf("\n\n Press <ENTER> to Continue...");
     Decoup_pret = false ;
     secondary = false ;

     fflush(stdin);
     getchar() ;
    break ;

    case 4:///Lecture L/OVC

     system("cls");

     Lect_L_NO_VC("file.bin");
     printf("\n\nVoici une structure d une seule donnee : Length || Key || Eff || Info_Name_SurName") ;
     printf("\nSi '_' n existe pas : cette info n a pas de nom ou de prenom.");
     printf("\nLength decrit toute la taille de : info+_+Name+_+Surname") ;
     printf("\n Si vous trouverez ||   || , cette info est supprimée logiquement (eff = 1)") ;
     printf("\n\n Press <ENTER> to Continue...");
     fflush(stdin);
     getchar() ;
    break ;

    case 5:///Lecture de l'entète

     system("cls");
     printf("**Emplacement du Block Tete (unitee est l'octet) : %d\n",Entete_L("file.bin" , 1));
     printf("**Derniere position libre : %d\n",Entete_L("file.bin" , 2));
     printf("**Nombre de donnees insere non supprime : %d\n",Entete_L("file.bin" , 3));
     printf("**Nombre de donnees supprimes logiquement : %d\n",Entete_L("file.bin" , 4));
     printf("**Nombre de Blocks : %d\n",Entete_L("file.bin" , 5));

     printf("\n\n Press <ENTER> to Continue...");
     fflush(stdin);
     getchar() ;
    break ;

    case 6:///Insertion

     system("cls");
     printf("\n**Donnee la cle sous forme de XXX (chiffres): ");
     scanf("%d",&don) ;
     Dechiffrement(don , cle_Buff) ;
     cle_Buff[3] = '\0' ;
     printf("Ecrivez votre Data (Maximum %d - 7 = %d caracteres): ",lim , lim-7);
     scanf("%s",Data);

     if(Recherche_L_NO_VC("file.bin" , cle_Buff , &don ,&trouv_cle , &i , &j)) printf("\nla cle existe deja pour une autre donnee non supprimee") ;
     else if(trouv_cle == false) printf("\nLa donnee a ete bien inseree ! ");
     else  printf("\n- Interruption dans Votre insertion : Cle de meme valeur trouve dans votre fichier , sa donnee precedente va etre reinseree .\n") ;

     Insertion_L_NO_VC("file.bin" , cle_Buff , Data) ;

     Decoup_pret = false ;
     printf("\n\n Press <ENTER> to Continue...");
     fflush(stdin);
     getchar() ;
    break ;

    case 7:///Recherche séquentielle

     system("cls");
     printf("quelle est la cle du donnee pour rechercher dans file.bin: ");
     scanf("%d",&don) ;
     Dechiffrement(don , cle_Buff) ;
     cle_Buff[3] = '\0' ;;

     if(Recherche_L_NO_VC("file.bin" , cle_Buff , &don ,&trouv_cle, &i , &j)) printf("\nvotre donnee est dans la %dieme position logique(champ de l info) de %dieme bloc tel que la taille de data est : %d \n" , j+1 , i , don);
     else if(trouv_cle) printf("cette donnee est supprimee logiquement dans Bloque : %d  position : %d", i , j) ;
     else printf("\nelement pas trouvee...");

     printf("\n\n Press <ENTER> to Continue...");
     fflush(stdin);
     getchar() ;
    break ;

    case 8:///Suppression physique

     system("cls");
     printf("ecris la cle pour que la donnee soit supprimee : ");
     scanf("%d",&don) ;
     Dechiffrement(don , cle_Buff) ;
     cle_Buff[3] = '\0' ;

     if(!Recherche_L_NO_VC("file.bin" , cle_Buff , &don ,&trouv_cle, &i , &j)) printf("\nla donnee n existe pas ...") ;
     else printf("\nLa donnee a ete bien supprimee ! ");

     Suppression_Physisque_L_NO_VC("file.bin" , cle_Buff) ;

     Decoup_pret = false ;
     secondary = false ;
     printf("\n\n Press <ENTER> to Continue...");
     fflush(stdin);
     getchar() ;
    break ;

     case 9:///Suppression logique

     system("cls");
     printf("ecris la cle pour que la donnee soit supprimee : ");
     scanf("%d",&don) ;
     Dechiffrement(don , cle_Buff) ;
     cle_Buff[3] = '\0' ;

     if(!Recherche_L_NO_VC("file.bin" , cle_Buff , &don ,&trouv_cle, &i , &j)) printf("\nla donnee n existe pas ...") ;
     else printf("\nLa donnee a ete bien supprimee ! ");

     Suppression_logique_L_NO_VC("file.bin" , cle_Buff) ;

     Decoup_pret = false ;
     secondary = false ;
     printf("\n\n Press <ENTER> to Continue...");
     fflush(stdin);
     getchar() ;
    break ;

    case 10:///Découpage

     system("cls");
     Decoupage_L_NO_VC("file.bin" , "fileT.bin" , "fileL.bin" , Indexe) ;

     Decoup_pret = true ;
     printf("Votre fichier a ete decoupee \n") ;
     printf("\n\n Press <ENTER> to Continue...") ;
     fflush(stdin);
     getchar() ;
    break ;

    case 11:///Lire le T/OF
     system("cls");

     Lect_T_NO_F("fileT.bin") ;
     printf("\n\n Press <ENTER> to Continue...") ;
     fflush(stdin);
     getchar() ;
    break ;

    case 12:///Lire le T/ovc

     system("cls");
     Lect_L_NO_VC("fileL.bin") ;

     printf("\n\n Press <ENTER> to Continue...") ;
     fflush(stdin);
     getchar() ;
    break ;

    case 13:///Lire le Tableau d indexes
     system("cls");
     don = Entete_L("file.bin" , 3) + Entete_L("file.bin" , 4) ;
     printf("Vous Avez quelques options de tri dans le tableau avant l affichage : \n\n");
     printf("1)-Trier les cles .\n");
     printf("2)-Trier les effacements .\n");
     printf("3)-Trier les i .\n");
     printf("4)-Trier les j .\n");
     printf("Else)- Faire aucun Tri .\n");
     printf("Choix : ");
     scanf("%d",&choi) ;
      switch(choi){
      case 1: Tri_Fusion(Indexe , 0 , don - 1 , 1) ;
      break ;
      case 2: Tri_Fusion(Indexe , 0 , don - 1 , 2) ;
      break ;
      case 3: Tri_Fusion(Indexe , 0 , don - 1 , 3) ;
      break ;
      case 4: Tri_Fusion(Indexe , 0 , don - 1 , 4) ;
      break ;
      default :
      break ;
      }

     system("cls");
     printf("---------- ECRITURE DE TABLE D INDEXE -------------\n\n");
     don = Entete_L("file.bin" , 3) + Entete_L("file.bin" , 4) ;
     for(int i = 0 ; i < don ; i++) printf("%d)- C:%d  Eff:%d  I:%d  J:%d\n",i, Indexe[i].cle , Indexe[i].eff , Indexe[i].i , Indexe[i].j ) ;

     printf("\n\nAstuce : Pour reinitialiser un tri et remettre le tableau par defaut , vous pouvez refaire le decoupage. ") ;
     printf("\n\n Press <ENTER> to Continue...") ;
     fflush(stdin);
     getchar() ;
    break ;

    case 14:///Reorganisation

     system("cls");
     printf("Loading...");
     Reorganisation_Indexe("file.bin" , Indexe);

     system("cls");
     printf("Votre fichier a ete bien reorganisee...");
     Decoup_pret = false ;
     secondary = false ;
     printf("\n\n Press <ENTER> to Continue...") ;
     fflush(stdin);
     getchar() ;
    break ;

    case 15:///Recherche Dichotomique
     system("cls");
     printf("La cle a rechercher : ");
     scanf("%d",&don) ;
     Recherche_L_NO_VC_Indexe("file.bin" , don , Indexe , &i , &j );

     printf("\n\n Press <ENTER> to Continue...") ;
     fflush(stdin);
     getchar() ;
    break ;

    case 16:///Chargement du table secondaire

     system("cls");
     Charger_Tables_Secondaires("file.bin" , Names , Prenames , Indexe);

     printf("Le chargement a ete bien effectuee");
     secondary = true ;
     printf("\n\n Press <ENTER> to Continue...") ;
     fflush(stdin);
     getchar() ;
    break ;

    case 17:///Lecture du table secondaire

     system("cls");
     printf("\n---------------- Lecture du Tableau des noms ---------------------\n\n");
     if(Names[0].Dernier_Maill == NULL) printf("Le Tableau des noms est vide ...\n");
     else Lect_Secondary_Tab(Names) ;

     printf("\n\n---------------- Lecture du Tableau des prenoms ---------------------\n\n");
     if(Prenames[0].Dernier_Maill == NULL) printf("Le Tableau des prenoms est vide ...\n");
     else Lect_Secondary_Tab(Prenames) ;

     printf("\n\n Press <ENTER> to Continue...") ;
     fflush(stdin);
     getchar() ;
    break ;

    case 18:///Recherche nom et prenom

     system("cls");
     printf("Le Nom: ");
     scanf("%s",nom) ;
     printf("Le Prenom: ");
     scanf("%s",prenom) ;
     Recherche_Name_Prename("file.bin" , nom , prenom , Names , Prenames , Indexe );
     printf("\n\n Press <ENTER> to Continue...") ;
     fflush(stdin);
     getchar() ;
    break ;

   }
 }

system("cls");
printf("Thank you for using our Program , see you next time ! ^_^ \n");

return 0;
}

