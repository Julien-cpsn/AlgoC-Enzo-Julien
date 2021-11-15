/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serveur.h"

void plot(char *data) {

  //Extraire le compteur et les couleurs RGB 
  FILE *p = popen("gnuplot -persist", "w");
  printf("Plot");
  int count = 1;
  int n;
  char *saveptr = NULL;
  char *str = data;

  char *token = strtok_r(str, " ", &saveptr);
  token = strtok_r(NULL, ",", &saveptr);
  str=NULL;

  int nb_colors = atoi(token);

  fprintf(p, "set xrange [-15:15]\n");
  fprintf(p, "set yrange [-15:15]\n");
  fprintf(p, "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "set title 'Top %d colors'\n", nb_colors);
  fprintf(p, "plot '-' with circles lc rgbcolor variable\n");
  while(1) {
    token = strtok_r(str, ",", &saveptr);

    if (token == NULL) {
      break;
    }
    str=NULL;
    if (count == 0) {
      n = atoi(token);
    }
    else {
      // 360° (cercle) / nb_colors
      fprintf(p, "0 0 10 %d %d 0x%s\n", (count-1)*360/nb_colors, count*360/nb_colors, token+1);
    }
    count++;
  }
  fprintf(p, "e\n");
  printf("Plot: FIN\n");
  pclose(p);
}

void encoderCommunication(char* data){
  char json[5000];
  char code[24], valeurs[500];
  char* commande = strtok(data," ");
  char* newValeurs = strtok(NULL," ");

    if(!strcmp(commande,"message:")){
      strcat(code,"message");
      strcat(valeurs,newValeurs);
    }
    else if(!strcmp(commande,"nom:")){
      strcat(code,"nom");
      strcat(valeurs,newValeurs);
    }
    else if(!strcmp(commande,"calcul:")){
      strcat(code,"calcul");
      strcat(valeurs,newValeurs);
    }

  strcat(strcpy(json,"{"),"\"code\" : \"");
  strcat(json,code);
  strcat(json,"\",\"valeurs\" : [ \"");
  strcat(json,valeurs);
  strcat(json,"\" ]}");
  strcpy(data,json);
}

void decoderCommunication(char* data){
  char* code = strtok(data, "\"");
  char* valeurs;
  char* newValeurs;
  char unparseData[1024];
  for(int i = 0 ; i < 3 ; ++i){
    code = strtok(NULL,"\"");
  }

  for(int i = 0 ; i < 2 ; ++i){
    valeurs = strtok(NULL,"\"");
  }

  if(!strcmp(code,"calcul")){
    newValeurs = strtok(NULL,"\"");
    newValeurs = strtok(NULL,"\"");
    strcpy(valeurs,newValeurs);
    newValeurs = strtok(NULL,"\"");
    newValeurs = strtok(NULL,"\"");
    strcat(valeurs," ");
    strcat(valeurs,newValeurs);
    newValeurs = strtok(NULL,"\"");
    newValeurs = strtok(NULL,"\"");
    strcat(valeurs," ");
    strcat(valeurs,newValeurs);
  }
  else if(!strcmp(code,"balises")){
    /*newValeurs = strtok(NULL,"[");
    newValeurs = strtok(NULL,"]");*/
  }
  else{
    for(int i = 0 ; i < 2 ; ++i){
      valeurs = strtok(NULL,"\"");
    }
  }

  strcat(strcpy(unparseData,code),": ");
  strcat(unparseData,valeurs);
  strcpy(data,unparseData);
}


/* ===== RENVOIS ====== */
/*
 * renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data) {

  encoderCommunication(data);
  
  printf("Data: %s",data);

  int data_size = write (client_socket_fd, (void *) data, strlen(data));
      
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

/*
 * renvoyer un nom (*data) au client (client_socket_fd)
 */
int renvoie_nom(int client_socket_fd, char *data) {

  encoderCommunication(data);

  int data_size = write (client_socket_fd, (void *) data, strlen(data));
      
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

/*
 * renvoyer un resultat de calcul (*data) au client (client_socket_fd)
 */
int renvoie_calcul(int client_socket_fd, char *data) {
  // Récupération de l'opérateur et des nombres
  char* tmp = strtok(data, " ");
  char* operateur = strtok(NULL, " ");
  double nombre1 = atof(strtok(NULL, " "));
  double nombre2 = atof(strtok(NULL, "\n"));

  // calcul
  double resultat = 0;
  if (!strcmp(operateur, "+")) {
    resultat = nombre1 + nombre2;
  }
  else if (!strcmp(operateur, "-")) {
    resultat = nombre1 - nombre2;
  }
  else if (!strcmp(operateur, "*")) {
    resultat = (double)(nombre1 * nombre2);
  }
  else if (!strcmp(operateur, "/")) {
    if (nombre2 == 0) {
      perror("division par zero");
      return(EXIT_FAILURE);
    }
    resultat = (double)(nombre1 / nombre2);
  }

  // préparation de la réponse
  char reponse[50];
  strcpy(reponse,"calcul: ");
  sprintf(tmp, "%f", resultat); 
  strcat(reponse,tmp);
  encoderCommunication(reponse);

  // envoi de la réponse
  int data_size = write (client_socket_fd, (void *) reponse, strlen(reponse));
      
  if (data_size < 0) {
    perror("erreur ecriture");
    return(EXIT_FAILURE);
  }
}

/*
 * sauvegarde les couleurs recues dans un fichier
 */
int renvoie_sauvegarde_couleurs(int client_socket_fd, char* data) {
  // Récupération des couleurs
  char* tmp = strtok(data, " ");
  char* couleurs = strtok(NULL, "\n");

  // Ecriture dans le fichier
  FILE* fichier = fopen("couleurs.txt", "w");
  fputs(couleurs, fichier);
}

/*
 * sauvegarde les balises recues dans un fichier
 */
int renvoie_sauvegarde_balises(int client_socket_fd, char* data) {
  // Récupération des balises
  char* tmp = strtok(data, " ");
  char* couleurs = strtok(NULL, "\n");

  // Ecriture dans le fichier
  FILE* fichier = fopen("balises.txt", "w");
  fputs(couleurs, fichier);
}

/* ===== RECEPTIONS ===== */
/*
 * accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie(int socketfd) {
  struct sockaddr_in client_addr;
  char data[1024];

  int client_addr_len = sizeof(client_addr);
 
  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *) &client_addr, &client_addr_len);
  if (client_socket_fd < 0 ) {
    perror("accept");
    return(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  //lecture de données envoyées par un client
  int data_size = read (client_socket_fd, (void *) data, sizeof(data));
      
  if (data_size < 0) {
    perror("erreur lecture");
    return(EXIT_FAILURE);
  }
  
  /*
   * extraire le code des données envoyées par le client. 
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  decoderCommunication(data);
  printf ("Message recu: %s\n", data);
  char code[10];
  sscanf(data, "%s", code);

  //Si le message commence par le mot: 'message:' 
  if (strcmp(code, "message:") == 0) {
    renvoie_message(client_socket_fd, data);
  }
  else if (strcmp(code, "nom:") == 0) {
    renvoie_nom(client_socket_fd, data);
  }
  else if (strcmp(code, "calcul:") == 0) {
    renvoie_calcul(client_socket_fd, data);
  }
  else if (strcmp(code, "couleurs:") == 0) {
    renvoie_sauvegarde_couleurs(client_socket_fd, data);
  }
  else if (strcmp(code, "balises:") == 0) {
    renvoie_sauvegarde_balises(client_socket_fd, data);
  }
  else if (strcmp(code, "plot:") == 0) {
    plot(data);
  }

  //fermer le socket 
  close(socketfd);
}

/* ===== MAIN ===== */
int main() {

  int socketfd;
  int bind_status;
  int client_addr_len;

  struct sockaddr_in server_addr, client_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if ( socketfd < 0 ) {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  //détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if (bind_status < 0 ) {
    perror("bind");
    return(EXIT_FAILURE);
  }
 
  // Écouter les messages envoyés par le client
  listen(socketfd, 10);

  //Lire et répondre au client
  recois_envoie(socketfd);

  return 0;
}
