/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef __SERVER_H__
#define __SERVER_H__

#define PORT 8089

int recois_envoie(int socketfd);

int renvoie_message(int client_socket_fd, char *data);
int renvoie_nom(int client_socket_fd, char *data);
int renvoie_calcul(int client_socket_fd, char* data);
int renvoie_sauvegarde_couleurs(int client_socket_fd, char* data);
int renvoie_sauvegarde_balises(int client_socket_fd, char* data);
void plot(char *data);

#endif
