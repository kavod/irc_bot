

/* Les includes communs a windows et linux */
#include <stdio.h>
#include <string.h>


/* includes de windows */
#ifdef WIN32
         #include <winsock.h>
         typedef unsigned int u_int32_t;    /* utilisé par linux mais non défini dans winsock.h */
#else 
	/* les includes de linux */
         #include <unistd.h>
         #include <sys/socket.h>
         #include <netinet/in.h>
         #include <netdb.h>
         #include <sys/types.h>
         #define SOCKET_ERROR (-1) /* défini dans winsock.h */
         typedef struct sockaddr_in SOCKADDR_IN; /* idem */
#endif

#include "extract.h"
#include "socket.h"






/* différentes commandes IRC */
#define IRC_USER         "USER %s %s %s :%s\n"
#define IRC_NICK         "NICK %s\n"    
#define IRC_MSG    	 "PRIVMSG %s :%s\n"
#define IRC_JOIN         "JOIN %s\n"
#define IRC_PART         "PART %s\n"
#define PING_CMD         "PING :%s\n"
#define PONG_CMD         "PONG :%s\n"
#define IRC_QUIT         "QUIT :%s\n"

/* structure définissant un utilisateur IRC : plus pratique */
typedef struct irc_user{
        char *nick;
        char *ident;
        char *name;
        char *host;
}irc_user;

/* quelques fonctions maison */

char *my_sock_recv_line(int sock);
int my_error(char *file,int line,int severity,char *texte); 
char *my_last_arg(char *chaine);        
char *strleft(char *chaine,int lft) ;
char hexatochar(char * hexa);

/* fonctions servant à se connecter à un server Radio */
int get_radio_title(char * utilisateur, int sock1);

/* fonction servant a se connecter a un server irc */
int irc_connect(char *serveur,int port,irc_user u1,char *chan);
int irc_shell(int sock1,char *master); // la fonction traitant les commandes du bot

char * config(FILE *f,char * param);
static char * addr_radio; 
static int port_radio;
static char * pass_radio; 
 


int main(int argc, char *argv[])
{
	irc_user bot;
	int retval=0;
	int mainsock; // le sock utilisé du debut a la fin
	
	char *chan;	// buffer pour recevoir le chan + le # ki va avec ;)
	char *str_port_radio;
	char *valconfig;
	/* Fichier de configuration */
	FILE * fileconfig;


	char car;
	
	
	printf("Arguments\t\t\t\t");
	if(argc < 6)     // si on a pas mis tous les arguments obligatoires
	{
		printf("USAGE: %s <serveur> <port> <nick> <chan> <propriétaire> [ident] [name]\n",argv[0]);
		return (-1);
	}
	if(argc == 6)       // si ya pas d ident et de name
	{
		bot.ident = "Kavod_bot";
		bot.name = "Miss Shiva";
	}
	if(argc == 7)    // si ya pas de name
	{
		bot.ident = (char *)malloc(strlen(argv[6]));
		bot.ident = argv[6];
		bot.name = "Miss Shiva";
	}
	if(argc == 8)    // si ya tout
	{
		bot.ident = (char *)malloc(strlen(argv[6]));
		bot.ident = argv[6]; 
		bot.name = (char *)malloc(strlen(argv[7]));
		bot.name = argv[7];
	}
	
	printf("OK\n");
	
	fileconfig = fopen("bot.conf", "rt");
	addr_radio = (char *)malloc(sizeof(char)*16);
	str_port_radio = (char *)malloc(sizeof(char)*8);
	pass_radio = (char *)malloc(sizeof(char)*32);
	printf("Chargement du fichier de configuration\t");
	fscanf(fileconfig,"addr_radio %s\n", addr_radio);
	fscanf(fileconfig,"port_radio %s\n", str_port_radio);
	fscanf(fileconfig,"pass_radio %s\n", pass_radio);
	port_radio = atoi(str_port_radio);

	//strcpy(addr_radio,config(fileconfig,"addr_radio"));
	//printf("addr_radio dans main : %s\n",addr_radio);
	printf("OK\n");
        fclose(fileconfig);
	printf("Etape 1\n");
	                
	bot.nick = (char *)malloc(strlen(argv[3])); // jaime bien les malloc (sinon ca segfault souvent sous nux)
	printf("Etape 2\n");
	bot.nick = argv[3];
	printf("Etape 3\n");
	bot.host = "kavod";       // sert a rien
	printf("Etape 4\n");
	chan =(char *)malloc(strlen(argv[4]) + 2);
	printf("Etape 5\n");
	sprintf(chan,"#%s",argv[4]);     // on prend le nom du chan et on rajoute le # devant
	printf("Etape 6\naddr_radio dans main : %s\n",addr_radio);
	

	while(retval != 1)
	{
		printf("Connexion...\t\t\t\t\n");
		mainsock = irc_connect(argv[1],atoi(argv[2]),bot,chan); // on connecte a irc et on renvoit une socket
		retval = irc_shell(mainsock,argv[5]); // le sock est reutilisé dans le shell
		printf("Echec\nRetentative\n\n");
	}
	return 0;
}

// FONCTION irc_connect
int irc_connect(char *serveur,int port,irc_user u1,char *chan)
{
	int sock1;
	int i;
	char *buf_send=(char *)malloc(sizeof(char)*50); // le buffer avec lekel on envoit les données
	char *buf_recv=(char *)malloc(sizeof(char));    // le buffer dans lequel sont stockées les données recues
	char *tmpbuf;
	
	#ifdef WIN32
	my_sock_init();    // fonction seulement pour win
	#endif
	
	sock1 = my_sock_client(serveur,port);   // on fait une connection cliente
	
	// on envoit la commande USER	
	sprintf(buf_send,IRC_USER,u1.ident,u1.nick,u1.nick,u1.name);
	if(send(sock1,buf_send,strlen(buf_send),0) == SOCKET_ERROR)
		my_error(__FILE__,__LINE__,0,"Send Error"); 
		
	// on envoit la commande NICK		
	sprintf(buf_send,IRC_NICK,u1.nick);
	if(send(sock1,buf_send,strlen(buf_send),0) == SOCKET_ERROR)
		my_error(__FILE__,__LINE__,0,"send error");

	buf_recv=(char *)my_sock_recv_line(sock1);
	if((tmpbuf = strstr(buf_recv,"PING")) != NULL) // si PING on repond
	{
		printf("PING ? PONG !\n");
		tmpbuf[1]='O';
		send(sock1,tmpbuf,strlen(tmpbuf),0);
	}
	
	for(i=0;i<1000;i++);     // c un temps d attente c tout :)
	sprintf(buf_send,"PRIVMSG nickserv :identify 40315\n"); 
	if(send(sock1,buf_send,strlen(buf_send),0) == SOCKET_ERROR)
		my_error(__FILE__,__LINE__,0,"send error");
	
	return sock1;
	sprintf(buf_send,IRC_JOIN,chan);             // on join le chan demandé
	if(send(sock1,buf_send,strlen(buf_send),0) == SOCKET_ERROR)
		my_error(__FILE__,__LINE__,0,"send error");
	
	return sock1;
}
    
int irc_shell(int sock2,char *master)
{
	int sock1 = sock2;
	int radiosock1;
	int i,n;
	int retval = 0;
	char *buf_send=(char *)malloc(sizeof(char)*50);
	char *buf_recv=(char *)malloc(sizeof(char));
	char *radio_buf_send=(char *)malloc(sizeof(char)*50); // le buffer avec lekel on envoit les données radio
	char *radio_buf_recv=(char *)malloc(sizeof(char));    // le buffer dans lequel sont stockées les données radio recues

	char *tmpbuf,*buf2;
	
	struct timeval tv; 
	fd_set rfds;
	
	tv.tv_sec=0;         // on va attendre 0seconde
	tv.tv_usec=0;       // et 0 µs :)
	printf("OK\n");
	while(retval == 0)
	{
		
		FD_ZERO(&rfds); // on met a zero le fd_set
		FD_SET(sock1,&rfds); // on ajoute sock1 kommme descripteur a l ensemble
		
		select(sock1 + 1,&rfds,NULL,NULL,&tv); // man select :]
		
		/* est ce qu on sock1 a subi un changement (arrivée de données par exemple) */
		if(FD_ISSET(sock1,&rfds))
		{
			buf_recv=(char *)my_sock_recv_line(sock1); // on recoit une ligne
			
			printf("%s",buf_recv);
			if((parser_get_cmd(buf_recv)) != NULL)
			{
				if((strcmp(parser_get_cmd(buf_recv),"PRIVMSG")) == 0) // si la commande est un PRIVMSG
				{
					printf("<%s> %s\r\n",parser_get_nick(buf_recv),parser_get_buf(buf_recv)); 

					// on affiche facon mirc :)	
		
					// si le nick de celui ki parle est celui ki controle le bot
					if((strcmp(parser_get_nick(buf_recv),master))== 0 || (strcmp(parser_get_dest(buf_recv),master)) == 0 || is_op(parser_get_dest(buf_recv)) == 1 || is_op(parser_get_nick(buf_recv)) == 1)
					{
						//printf("master détect\n");
						/* Fonctions réservées au master */
						if((tmpbuf = strstr(buf_recv,"!join")) != NULL)
						{
							if((buf2 = strstr(tmpbuf," ")) != NULL) // ca prend tout ce kya apres le !join
							{
								// strleft me sert ici a enlever l espace devant le chan
								printf("*** JOINING %s",strleft(buf2,1));
								sprintf(buf_send,IRC_JOIN,strleft(buf2,1));// cmd JOIN
								send(sock1,buf_send,strlen(buf_send),0); // on join le chan
							}
						}
						if((tmpbuf = strstr(buf_recv,"!part")) != NULL) // idem avec part
						{
							if((buf2 = strstr(tmpbuf," ")) != NULL)
							{
								printf("*** PARTING %s",strleft(buf2,1));
								sprintf(buf_send,IRC_PART,strleft(buf2,1));
								send(sock1,buf_send,strlen(buf_send),0);
							}
						}
						// quit qui en + de faire partir du serv fait sortir du while
						if((tmpbuf = strstr(buf_recv,"!quit")) != NULL)
						{
							if((buf2 = strstr(tmpbuf," ")) != NULL)
							{
								printf("*** QUIT(%s)",strleft(buf2,1));
								sprintf(buf_send,IRC_QUIT,strleft(buf2,1));
								send(sock1,buf_send,strlen(buf_send),0);
								retval = 1;
							}
						}
						if((tmpbuf = strstr(buf_recv,"!exit")) != NULL)
						{
							send(sock1,"QUIT\n",strlen("QUIT\n"),0);
							retval = 2;// avec ca le bot quit et revient
						}
						if((tmpbuf = strstr(buf_recv,"!say")) != NULL)
						{
							if((buf2 = strstr(tmpbuf," ")) != NULL)
							{
								printf("*** %s",strleft(buf2,1));
								sprintf(buf_send,IRC_MSG,"#ensemble",strleft(buf2,1));
								//            sprintf(buf_send,IRC_MSG,parser_get_dest(buf_recv),strleft(buf2,1));
								printf(IRC_MSG,parser_get_dest(buf_recv),strleft(buf2,1));
								send(sock1,buf_send,strlen(buf_send),0);
							}
						}
						// ex: !raw PRIVMSG #groar :lol <-- raw sert a envoyer des donneés au server
						if((tmpbuf = strstr(buf_recv,"!raw")) != NULL)
						{
							if((buf2 = strstr(tmpbuf," ")) != NULL)
							{
								printf("RAW :%s",buf2);
								sprintf(buf_send,strleft(buf2,1));
								send(sock1,buf_send,strlen(buf_send),0);
							}
						}
						if((tmpbuf = strstr(buf_recv,"!nick")) != NULL)  // change de nick
						{
							printf("*** je change de nick : %s",my_last_arg(tmpbuf));
							sprintf(buf_send,IRC_NICK,my_last_arg(tmpbuf));
							send(sock1,buf_send,strlen(buf_send),0);
						}
					} else 
					{
						if (strstr(parser_get_dest(buf_recv),"#") != NULL)
						{
							//printf("chan détect\n");
							/* Fonctions de listening du chan */
						} else
						{
							//printf("user détect\n");
							/* Fonctions accessibles aux utilisateurs */
							if ((tmpbuf = strstr(buf_recv,"!radio")) != NULL)
							{
								printf("radio détect\naddr_radio dans irc_shell : %s\n",addr_radio);
								get_radio_title(parser_get_nick(buf_recv),sock1);
							} else
							{					
								sprintf(buf_send,"PRIVMSG %s :Salut %s, donne moi quelques jours pour que j'apprenne à parler et je te répondrai\n",parser_get_nick(buf_recv),parser_get_nick(buf_recv));
								send(sock1,buf_send,strlen(buf_send),0);
							}
						}
					}
				} 
			} else
			{
				if((tmpbuf = strstr(buf_recv,"PING")) != NULL) // PING ? PONG !
				{
					tmpbuf[1]='O';
					send(sock1,tmpbuf,strlen(tmpbuf),0);
					printf("PING ? PONG !\n");
				}
			}
				
		}
		else
		{
        		
		}
	}
	#ifdef WIN32
	closesocket(sock1); // facon win
	#else
	close(sock1);   // facon nux
	#endif
	
	return retval;
}





// decale une char * vers la gauche : exemple : strleft("groar",1); = roar
char *strleft(char *chaine,int lft)
{
	int i = 0;
	char *buf = (char *)malloc(strlen(chaine) - lft);
	for(i = lft; i < strlen(chaine); i++)
	{
		buf[i - lft] = chaine[i];
	}
	return (char *)buf;
}

// avec ca on recoit ligne par ligne ce ke le serv envoie
char *my_sock_recv_line(int sock)
{
	int pos = 0;
	char c;
	int buf_len;
	char *buf_str;
	
	buf_str = (char *)malloc(10 * sizeof(char));
	buf_len = 10;
	
	c = '\0';
	
	while((c != '\n') && (c != '>') )
	{
		recv(sock,&c,sizeof(char),0);
		buf_str[pos] = c;
		pos++;
		if(pos == buf_len)
		{
			buf_len += 10;
			buf_str = (char *)realloc(buf_str,buf_len * sizeof(char));
		}
	}
	buf_str[pos] = '\0';
	
	return (char *)buf_str;
}



char *my_last_arg(char *chaine) // retourne le dernier mot d une chaine
{
	char* p;
	char* buffer,*temp;
	char* separateurs = { " " };
	
	buffer = strdup(chaine);
	
	p = strtok( buffer, separateurs );
	temp = (char *)malloc(strlen(p)+1);
	
	while((p = strtok( NULL, separateurs    )) != NULL )
	{
		free(temp);
		temp = (char *)malloc(strlen(p)+1);
		sprintf(temp,p);
	}
	
	return (char*)temp;
}
// erreur : c pourri comme error mais bon
int my_error(char *file,int line,int severity,char *texte)
{
	printf("%s:%d # %s\n",file,line,texte);
	return 0;
}




char hexatochar(char * hexa)
{
        int ascii;

        sscanf(hexa, "%x", &ascii);

        printf("%d", ascii);
} 


int get_radio_title(char * utilisateur, int sock1)
{
	char *buf_send=(char *)malloc(sizeof(char)*100); // le buffer avec lekel on envoit les données sur l'IRC
	char *buf_recv=(char *)malloc(sizeof(char));    // le buffer dans lequel sont stockées les données recues sur l'IRC
	char *radio_buf_send=(char *)malloc(sizeof(char)*100); // le buffer avec lekel on envoit les données sur la radio
	char *radio_buf_recv=(char *)malloc(sizeof(char));    // le buffer dans lequel sont stockées les données recues sur la radio
	char *tmpbuf;
	int radiosock1,i;
	
	#ifdef WIN32
	my_sock_init();    // fonction seulement pour win
	#endif
	printf("Connexion au serveur Radio\n%s\n",addr_radio);
	
	if ((radiosock1 = my_sock_client(addr_radio,port_radio)) < 0) // on connecte a la radio et on renvoit une socket
	{
		printf("*** Erreur connexion radio: %s");
		sprintf(buf_send,"PRIVMSG %s :Impossible de se connecter à la radio\n",utilisateur);
		send(sock1,buf_send,strlen(buf_send),0);
	} else
	{	
		char *radio_buf_send=(char *)malloc(sizeof(char)*100); // le buffer avec lekel on envoit les données
		char *radio_buf_recv=(char *)malloc(sizeof(char));    // le buffer dans lequel sont stockées les données recues
		sprintf(radio_buf_send,"GET /admin.cgi?pass=%s&mode=viewxml HTTP/1.1\nUser-Agent:Mozilla\n\n",pass_radio);
		if(send(radiosock1,radio_buf_send,strlen(radio_buf_send),0) == SOCKET_ERROR)
			my_error(__FILE__,__LINE__,0,"Send Error"); 
		radio_buf_recv=(char *)my_sock_recv_line(radiosock1);
		for (i=0;((tmpbuf = strstr(radio_buf_recv,"<SONGTITLE")) == NULL) && (i < 1000)  ; i++)// tant qu'on a pas notre <songtitle>
		{
			radio_buf_recv=(char *)my_sock_recv_line(radiosock1);
		}
		if (tmpbuf == NULL)
		{
			sprintf(buf_send,"Titre de la chanson introuvable\n",utilisateur);
			send(sock1,buf_send,strlen(buf_send),0);
			sprintf(buf_send,"PRIVMSG %s :Radio Kavod : http://kavod.net0.nerim.net:8000\n",utilisateur);
			send(sock1,buf_send,strlen(buf_send),0);
		} else
		{
			radio_buf_recv=(char *)my_sock_recv_line(radiosock1);
			sprintf(buf_send,"PRIVMSG %s :Sur Radio Kavod : 4",utilisateur);
			send(sock1,buf_send,strlen(buf_send),0);
			for (i=0 ; strcmp(radio_buf_recv,strstr(radio_buf_recv,"</SONGTITLE>")) != 0 ;i++)
			{
				if (radio_buf_recv[0]=='&' && radio_buf_recv[1]=='#' && radio_buf_recv[2]=='x' )
				{
					sprintf(radio_buf_send,"%c%c",radio_buf_recv[3],radio_buf_recv[4]);
					sprintf(buf_send,"%c",hexatochar(radio_buf_send));
					send(sock1,buf_send,strlen(buf_send),0);
					radio_buf_recv += 5;
					i += 5;									
				} else
				{
					sprintf(buf_send,"%c",radio_buf_recv[0]);
					send(sock1,buf_send,strlen(buf_send),0);
				}
				radio_buf_recv += 1;
			}
			send(sock1,"\n",strlen("\n"),0);
			sprintf(buf_send,"PRIVMSG %s :Radio Kavod : http://kavod.net0.nerim.net:8000\n",utilisateur);
			send(sock1,buf_send,strlen(buf_send),0);
			
		}
		#ifdef WIN32
		closesocket(radiosock1); // facon win
		#else
		close(radiosock1);   // facon nux
		#endif						
	}
	return 1;
}

char * config(FILE *f,char * param)
{
        char * motif,*value;
        int fch;
	
	sprintf(motif,"%s %%s\n",param);
	//printf("motif : %s\n",motif);
        fch = fscanf(f, motif, value);
        //printf("Param: %s\nValeur: %s\n", param, value);
        
        return value;
}

int is_op(char *username,int sock1)
{
	char * buf_send, * buf_recv, * user;
	buf_send=(char *)malloc(sizeof(char)*50);
	buf_recv=(char *)malloc(sizeof(char));
	sprintf(user,"@%s",username);
	sprintf(buf_send,"NAMES #ensemble\n");
	send(sock1,buf_send,strlen(buf_send),0);
	buf_recv=(char *)my_sock_recv_line(sock1);
	while (strcmp(buf_recv,strstr(buf_recv,"End of /NAMES list")) != 0)
	{
		if (strcmp(buf_recv,strstr(buf_recv,user)) == 0)
		{
			return 1;
		}
		buf_recv=(char *)my_sock_recv_line(sock1);
	}
	return 0;
}

