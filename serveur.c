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

#include<stdio.h>
#include <curses.h>

#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

void serveur_appli (char *service);   /* programme serveur */


/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/
	
	serveur_appli(service);
}


/******************************************************************************/	
void serveur_appli(char *service)

/* Procedure correspondant au traitemnt du serveur de votre application */

{
	int sock_ecoute = h_socket(AF_INET,SOCK_STREAM);
	struct sockaddr_in* p_adr_socket;
	adr_socket(service,NULL,SOCK_STREAM, &p_adr_socket);
	h_bind(sock_ecoute,p_adr_socket);
	h_listen(sock_ecoute, 2);
	int sock_com = h_accept(sock_ecoute,p_adr_socket);
	int taille_buff = 4096;
	char* tampon = malloc(taille_buff);
	h_reads(sock_com,tampon,taille_buff);
	printf("Message reçu : %s\n", tampon);
	printf("Donnez une réponse : \n");
	scanf("%s",tampon);
	h_writes(sock_com, tampon, taille_buff);
	
	h_close(sock_com);
	h_close(sock_ecoute);
	free(tampon);

}

/******************************************************************************/	

