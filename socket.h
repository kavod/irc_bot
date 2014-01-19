/************************************************
* Application Shiva (Bot IRC)			*
* Auteur : Boris Kavod				*
* Fonctions de connexions socket		*
************************************************/

#ifndef _socket_h
#define _socket_h

/* Connexion au serveur Fip par le port port */
int my_sock_client(char Fip[],int port);

/* Initialisation de socket (pour windows) */
int my_sock_init();

#endif
