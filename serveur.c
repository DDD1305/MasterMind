/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/

#include <curses.h>
#include <netinet/in.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fon.h" /* Primitives de la boite a outils */

#include "jeu.h"

#define SERVICE_DEFAUT "1111"

#define CONTINUE "CONTINUE"
#define WIN "WIN"

void serveur_appli(char *service); /* programme serveur */

/******************************************************************************/
/*---------------- programme serveur ------------------------------*/

int main(int argc, char *argv[]) {

  char *service = SERVICE_DEFAUT; /* numero de service par defaut */

  /* Permet de passer un nombre de parametre variable a l'executable */
  switch (argc) {
  case 1:
    printf("defaut service = %s\n", service);
    break;
  case 2:
    service = argv[1];
    break;

  default:
    printf("Usage:serveur service (nom ou port) \n");
    exit(1);
  }

  /* service est le service (ou numero de port) auquel sera affecte
  ce serveur*/

  serveur_appli(service);
}

/******************************************************************************/

typedef struct {
  int num_socketInit;
  int num_socketCom;
} socket_tcp;

socket_tcp client;

/*
 * Initialise la socket serveur :
 *  - création de la socket TCP
 *  - association à une adresse (bind)
 *  - mise en écoute (listen)
 *  - acceptation d'un client
 */
void init_TCP_com(char *service) {
  client.num_socketInit = h_socket(AF_INET, SOCK_STREAM);
  struct sockaddr_in *adr;
  adr_socket(service, NULL, SOCK_STREAM, &adr);
  h_bind(client.num_socketInit, adr);
  h_listen(client.num_socketInit, 32);
  struct sockaddr_in adr_client;
  client.num_socketCom = h_accept(client.num_socketInit, &adr_client);
}

/*
 * Reçoit le niveau de jeu envoyé par le client.
 * Le niveau est transmis en format réseau (big endian),
 * il faut donc le convertir avec ntohl.
 */
int send_msg(socket_tcp socket, char *msg) {
  int len = strlen(msg);
  int lenNetwork = ntohl(len);
  int write = h_writes(socket.num_socketCom, (char *)&lenNetwork,
                       sizeof(int)); // d'abord la taille du msg
  if (write != sizeof(int)) {
    return -1;
  }
  write = h_writes(socket.num_socketCom, msg, len);
  if (write != len) {
    return -1;
  }
  return write;
}

/*
 * Reçoit un message depuis le client :
 *  - lit la taille du message
 *  - convertit la taille (ntohl)
 *  - vérifie que le buffer est suffisant
 *  - lit le message
 *  - ajoute un '\0' pour obtenir une chaîne C valide
 *
 * Retour :
 *  - taille du message lu
 *  - -1 en cas d'erreur
 */
int receive_msg(socket_tcp socket, char *tampon, int taille) {
  int lenNetwork;
  int read = h_reads(socket.num_socketCom, (char *)&lenNetwork,
                     sizeof(int)); // On lit d'abord la taille
  if (read != sizeof(int)) {
    return -1;
  }
  int len = ntohl(lenNetwork);
  if (len >= taille) {
    printf("buffer tampon trop petit");
    return -1;
  }
  read = h_reads(socket.num_socketCom, tampon, len);
  if (read != len) {
    return -1;
  }
  // le réseau ne transmet pas le '\0',
  // on doit donc l'ajouter manuellement après réception
  tampon[len]='\0';
  return read; // retourne la taille du msg lu ou -1 si erreur.
}

int receive_level(socket_tcp socket) {
  int niveau_reseau;
  int reads =
      h_reads(socket.num_socketCom, (char *)&niveau_reseau, sizeof(int));
  /*
  if(reads ==0){
    printf("Fermeture client");
    return -1;
  }*/
  if (reads != sizeof(int)) {
    printf("n mauvais format\n");
    return -1;
  }
  

  level = ntohl(niveau_reseau);
  if (level <= 0 || level > 100) {
    printf("niveau invalide\n");
    return -1;
  }
  return 0; // pas d'erreur
}


/*
 * Boucle principale du serveur :
 *  - envoie une consigne au client
 *  - reçoit une proposition
 *  - calcule la correction
 *  - envoie la correction
 *  - envoie l'état du jeu (WIN / CONTINUE / LOSE)
 *
 * La boucle s'arrête lorsque la partie est terminée.
 */
void serveur_appli(char *service)

/* Procedure correspondant au traitement du serveur de votre application */

{
  init_TCP_com(service);

  // On récupère le niveau
  int read = receive_level(client);
  if (read == -1) {
    printf("Erreur récupération level");
    goto cleanup;
  }

  // On initialise le jeu.
  init(level);
  char *answer = NULL;
  char *try = malloc(level + 1); // pour avoir la chaine de taille n et \0
  if (try == NULL) {
    printf("erreur allocation try");
    goto cleanup;
  }
  while (!win) {
    int send = send_msg(client, "Entrer la proposition:");
    if (send == -1) {
      printf("Erreur à l'envoie : Entrer une proposition\n");
      goto cleanup;
    }
    int read = receive_msg(client, try, level + 1); // fct bloquante
    if (read == -1) {
      goto cleanup;
    }
    if(read ==0){
    printf("Fermeture client");
    goto cleanup;
    }
    answer = check(try, level);
    send = send_msg(client, answer);
    if (send == -1) {
      printf("Erreur à l'envoie la correction\n");
      goto cleanup;
    }
    free(answer);
    answer = NULL;

    if(win){
      send = send_msg(client, "WIN");
      if (send == -1) {
        printf("Erreur à l'envoie : %s\n", WIN);
        goto cleanup;
      }
    }else{
      send = send_msg(client, CONTINUE);
      if (send == -1) {
        printf("Erreur à l'envoie : %s\n", CONTINUE);
        goto cleanup;
      }
    }
  }

cleanup:
  /*
 * Libère les ressources :
 *  - mémoire allouée
 *  - fermeture des sockets
 */
  if (try != NULL) {
    free(try);
    try= NULL;
  }
  if (answer != NULL) {
    free(answer);
    answer = NULL;
  }
  h_close(client.num_socketCom);
  h_close(client.num_socketInit);
}

/******************************************************************************/
