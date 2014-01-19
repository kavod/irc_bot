/************************************************
* Application Shiva (Bot IRC)			*
* Auteur : Boris Kavod				*
* Fonctions de connexions socket		*
************************************************/
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
#include "socket.h"

int my_sock_client(char Fip[],int port) // pour se connnect a un serv
{
        int s;
        unsigned int addr;
        struct hostent *hp;
        struct sockaddr_in sin;

        if (isalpha(Fip[0]))
                hp=gethostbyname(Fip);
        else
        { 
                addr = inet_addr(Fip);
                hp = gethostbyaddr((char *)&addr,4,AF_INET); 
        }

        sin.sin_addr.s_addr =   *((u_int32_t *) hp->h_addr);
        sin.sin_family = AF_INET; 
        sin.sin_port=htons(port);

        s = socket(PF_INET, SOCK_STREAM, 0);

        if (connect(s,(struct sockaddr *)&sin,sizeof(SOCKADDR_IN))==SOCKET_ERROR)
        {
        //	printf("%i\n",errno);
              my_error(__FILE__,__LINE__,0,"erreur de connect");
	}
        return s;
}

int my_sock_init()   // init pour winsock
{
	#ifdef WIN32
	WSADATA wsaData;
	WORD verreq;
	
	verreq=MAKEWORD(2,2);
	if(WSAStartup(verreq,&wsaData) != 0)
		return -1;
	#endif
	return 0;
}

