/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

/*
 * port d'ordinateur pour envoyer et recevoir des messages
 */
#define PORT 8089

/* 
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */
int envoie_recois_message(int socketfd);
int envoie_nom_de_client(int socketfd);
int envoie_de_calcul(int socketfd);
int envoie_couleurs(int socketfd, char *pathname);
int envoie_balises(int socketfd);
int envoie_plot(int socketfd, char *pathname, int nb_colors);
int envoie_json(int socketfd, char *pathname);

#endif
