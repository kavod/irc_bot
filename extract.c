/************************************************
* Application Shiva (Bot IRC)			*
* Auteur : Boris Kavod				*
* Fonctions d'interprétation de messages IRC	*
************************************************/
#include <stdio.h>
#include <string.h>
#include "extract.h"

char *parser_get_ident(char *data)
{   // meme systeme : man strtok
	char *buf;
	char *delimit = "~@" ;
	char *buf1 = strdup(data);
	buf = strtok(buf1, ":!");
	buf = strtok(NULL, delimit);
	return (char *)buf;
}

char *parser_get_host(char *data)
{
	char *buf;
	char *delimit = " " ;
	char *buf1 = strdup(data);
	buf = strtok(buf1, ":!");
	buf = strtok(NULL, "~@");
	buf = strtok(NULL, delimit);
	return (char *)buf;
}

char *parser_get_cmd(char *data)
{
	char *buf;
	char *delimit = " " ;
	char *buf1 = strdup(data);
	buf = strtok(buf1, ":!");
	buf = strtok(NULL, "~@");    
	buf = strtok(NULL, " ");
	buf = strtok(NULL, delimit);
	return (char *)buf;
}

char *parser_get_dest(char *data)
{
        char *buf;
        char *delimit = ":" ;
        char *buf1 = strdup(data);
        buf = strtok(buf1, ":!");
        buf = strtok(NULL, "~@");    
        buf = strtok(NULL, " ");
        buf = strtok(NULL, " ");
        buf = strtok(NULL, delimit);
        return (char *)buf;
}

char *parser_get_buf(char *data)
{
        char *buf;
        char *delimit = "\n" ;
        char *buf1 = strdup(data);
        buf = strtok(buf1, ":!");
        buf = strtok(NULL, "~@");    
        buf = strtok(NULL, " ");
        buf = strtok(NULL, " ");
        buf = strtok(NULL, ":");
        buf = strtok(NULL, delimit);
        return (char *)buf;
}

char *parser_get_nick(char *data)
{
	char *buf=(char *)malloc(100*sizeof(char));
	char *delimit = ":!" ;
	char *buf1 = strdup(data);
	buf = strtok(buf1, delimit); // on prend le char * entre le : et le !
	return (char *)buf;      // c le nick

}

