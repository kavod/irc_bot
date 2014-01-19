/************************************************
* Application Shiva (Bot IRC)			*
* Auteur : Boris Kavod				*
* Fonctions d'interprétation de messages IRC	*
************************************************/
#ifndef _extract_h
#define _extract_h

/* Extraction de l'ident */
char *parser_get_ident(char *data);

/* Extraction du DNS */
char *parser_get_host(char *data);

/* Extraction de la commande */
char *parser_get_cmd(char *data);

/* Extraction de la destination du message */
char *parser_get_dest(char *data);

/* Extraction du message PRIVMSG */
char *parser_get_buf(char *data);

/* Extraction du nickname */
char *parser_get_nick(char *data);

#endif
