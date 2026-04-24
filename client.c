/******************************************************************************/
/*			Application: ... */
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/

#include <curses.h> /* Primitives de gestion d'ecran */
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fon.h" /* primitives de la boite a outils */

#include "jeu.h"

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

#define TAILLE_MSG 256

int levelClient;

void client_appli(char *serveur, char *service);

/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[]) {

  char *serveur = SERVEUR_DEFAUT; /* serveur par defaut */
  char *service =
      SERVICE_DEFAUT; /* numero de service par defaut (no de port) */

  /* Permet de passer un nombre de parametre variable a l'executable */
  switch (argc) {
  case 1: /* arguments par defaut */
    printf("serveur par defaut: %s\n", serveur);
    printf("service par defaut: %s\n", service);
    break;
  case 2: /* serveur renseigne  */
    serveur = argv[1];
    printf("service par defaut: %s\n", service);
    break;
  case 3: /* serveur, service renseignes */
    serveur = argv[1];
    service = argv[2];
    break;
  default:
    printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
    exit(1);
  }

  /* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
  /* service le numero de port sur le serveur correspondant au  */
  /* service desire par le client */

  client_appli(serveur, service);
}

int socketClient;

int init_TCP_com(char *serveur, char *service) {
  int socket = h_socket(AF_INET, SOCK_STREAM);
  struct sockaddr_in *p_adr_socket;
  adr_socket(service, serveur, SOCK_STREAM, &p_adr_socket);
  h_connect(socket, p_adr_socket);
  return socket;
}

int ask_level(int socket) {
  int niveau;
  printf("Entrer le niveau : ");
  scanf("%d", &niveau);
  int niveau_reseau = htonl(niveau);
  int write = h_writes(socket, (char *)&niveau_reseau, sizeof(int));
  if (write != sizeof(int)) {
    return -1;
  }
  levelClient= niveau;
  return niveau;
}

int send_msg(char *msg) {
  int len = strlen(msg);
  int lenNetwork = htonl(len);
  int write = h_writes(socketClient, (char *)&lenNetwork,
                       sizeof(int)); // d'abord la taille du msg
  if (write != sizeof(int)) {
    return -1;
  }
  write = h_writes(socketClient, msg, len);
  if (write != len) {
    return -1;
  }
  return write;
}

int receive_msg(char *tampon, int taille) {
  int lenNetwork;
  int read = h_reads(socketClient, (char *)&lenNetwork,
                     sizeof(int)); // On lit d'abord la taille
  if (read != sizeof(int)) {
    return -1;
  }
  int len = htonl(lenNetwork);
  if (len > taille) {
    printf("buffer tampon trop petit\n");
    return -1;
  }
  read = h_reads(socketClient, tampon, len);
  if (read != len) {
    return -1;
  }
  tampon[len] = '\0';
  return read; // retourne la taille du msg lu ou -1 si erreur.
}

/*****************************************************************************/
void client_appli(char *serveur, char *service)

/* procedure correspondant au traitement du client de votre application */

{
  socketClient = init_TCP_com(serveur, service);

  if (ask_level(socketClient) == -1) {
    printf("Erreur à la récupération du niveau\n");
    goto cleanup;
  }

  char* tampon_consigne = malloc(TAILLE_MSG);
  if(tampon_consigne == NULL){
    printf("Erreur allocation tampon_consigne\n");
    goto cleanup;
  }

  int taille_buff = levelClient + 1;
  char *tampon_correction = malloc(taille_buff);
  if (tampon_correction == NULL) {
    printf("Erreur allocation tampon_correction\n");
    goto cleanup;
  }
  char *tampon_write = malloc(taille_buff);
  if (tampon_write == NULL) {
    printf("Erreur à l'allocation tampon_write\n");
    goto cleanup;
  }
  
  int running =1;
  while (running) {
    // 1. Lire l'invitation du serveur ("Entrer la proposition" - 21 octets)
    int read = receive_msg(tampon_consigne, TAILLE_MSG);
    if (read == -1) {
      printf("Erreur réception de msg A\n");
      goto cleanup;
    }
    printf("%.*s\n", read, tampon_consigne);

    // 2. Saisir la proposition (attention : l'utilisateur doit taper tout
    // attaché, ex: "RVBB")
    while (1) {
      scanf("%s", tampon_write);
      if (strlen(tampon_write) == (size_t) levelClient) {
        break;
      }
      printf("Il faut %i couleurs\nEntrer votre proposition:\n", levelClient);
    }

    // 3. Envoyer UNIQUEMENT la taille attendue par le serveur (niveau)
    send_msg(tampon_write);

    // 4. Lire la réponse du serveur (qui fait exactement 'niveau' octets)
    read = receive_msg(tampon_correction, taille_buff);
    if (read == -1) {
      printf("Erreur réception de la correction\n");
      goto cleanup;
    }
    printf("Résultat : %.*s\n\n", read, tampon_correction);

    read = receive_msg(tampon_consigne, TAILLE_MSG);
    if (read == -1) {
      printf("Erreur réception de msg\n");
      goto cleanup;
    }
    if (read > 0 && strcmp(tampon_consigne, "WIN") == 0) {
      running=0;
    }
  }

  
cleanup:

if(tampon_correction != NULL){
  free(tampon_correction);
}
if(tampon_write != NULL){
  free(tampon_write);
}

h_close(socketClient);
if (!win) {
    printf("Bravo, vous avez trouvé la bonne combinaison !\n");
}
}

/*****************************************************************************/
