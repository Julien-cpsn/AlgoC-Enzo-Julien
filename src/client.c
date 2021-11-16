/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#include "client.h"
#include "bmp.h"

/* ===== FONCTIONS ===== */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */


void encoderCommunication(char* data){ //encode en JSON pour la communication client/serveur
  char json[5000];
  int nombre_valeurs = 500;
  char* valeurs[nombre_valeurs];
  char* delimiters[] = {"\0"," ",", "};
  char* delimiter;
  char* commande = strtok(data,":");
  char* token = strtok(NULL,"\0");

  if(!strcmp(commande,"message")){
    delimiter = delimiters[0];
  }
  else if(!strcmp(commande,"nom")){
    delimiter = delimiters[0];
  }
  else if(!strcmp(commande,"calcul")){
    delimiter = delimiters[1];
  }
  else if(!strcmp(commande,"couleurs")){
    delimiter = delimiters[2];
  }
  else if(!strcmp(commande,"balises")){
    delimiter = delimiters[2];
  }

  char* segment = strtok(token,delimiter);
  int indice = 0;

  do{
    valeurs[indice] = malloc(sizeof(char));
    strcpy(valeurs[indice],segment);
    indice++;
    segment = strtok(NULL,delimiter);
  }while(segment != NULL);

  strcat(strcpy(json,"{"),"\"code\": \"");
  strcat(json,commande);
  strcat(json,"\",\"valeurs\": [");
  for(int i = 0; valeurs[i] != NULL && i < nombre_valeurs; ++i){
    strcat(json,"\"");
    strcat(json,valeurs[i]);
    strcat(json,"\"");
    if(valeurs[i+1] != NULL){
      strcat(json,", ");
    }
  }
  strcat(json,"]}");
  strcpy(data,json);

  for(int i = 0; i <= indice; ++i){
    free(valeurs[indice]);
  }
}

void decoderCommunication(char* data){ //decode en JSON pour la communication client/serveur
  char* code = strtok(data, "\"");
  char* valeurs;
  char unparseData[1024];
  for(int i = 0 ; i < 7 ; ++i){
    if(i < 3)
      code = strtok(NULL,"\"");
    else
      valeurs = strtok(NULL, "\"");
  }

  strcat(strcpy(unparseData,code),": ");
  strcat(unparseData,valeurs);
  strcpy(data,unparseData);
}




int envoie_recois_message(int socketfd) {
 
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));


  // Demandez à l'utilisateur d'entrer un message
  char message[100];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, 1024, stdin);
  strcpy(data, "message: ");
  strcat(data, message);

  encoderCommunication(data);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if ( read_status < 0 ) {
    perror("erreur lecture");
    return -1;
  }

  //decoderCommunication(data);

  printf("Message recu: %s\n", data);
 
  return 0;
}

void analyse(char *pathname, char *data, int nb_colors) {
  //compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  char temp_string[nb_colors];
  if (cc->size <= nb_colors) {
      sprintf(temp_string, "%d,", cc->size);
  }
  else {
      sprintf(temp_string, "%d,", nb_colors);
  }
  strcat(data, temp_string);

  //choisir n (<=30) couleurs
  for (count = 1; count < nb_colors + 1 && cc->size - count > 0; count++) {
    if(cc->compte_bit ==  BITS32) {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size-count].c.rouge,cc->cc.cc32[cc->size-count].c.vert,cc->cc.cc32[cc->size-count].c.bleu);
    }
    if(cc->compte_bit ==  BITS24) {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size-count].c.rouge,cc->cc.cc32[cc->size-count].c.vert,cc->cc.cc32[cc->size-count].c.bleu);
    }
    strcat(data, temp_string);
  }

  //enlever le dernier virgule
  data[strlen(data)-1] = '\0';
}

int envoie_nom_de_client(int socketfd) {
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));


  // Demandez à l'utilisateur d'entrer son nom
  char message[100];
  printf("Votre nom (max 1000 caracteres): ");
  fgets(message, 1024, stdin);
  strcpy(data, "nom: ");
  strcat(data, message);

  encoderCommunication(data);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if ( read_status < 0 ) {
    perror("erreur lecture");
    return -1;
  }

  decoderCommunication(data);

  printf("Nom recu: %s\n", data);
 
  return 0;
}

int envoie_de_calcul(int socketfd) {
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));


  // Demandez à l'utilisateur d'entrer son nom
  char message[100];
  printf("Votre calcul (max 1000 caracteres): ");
  fgets(message, 1024, stdin);
  strcpy(data, "calcul: ");
  strcat(data, message);

  encoderCommunication(data);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if ( read_status < 0 ) {
    perror("erreur lecture");
    return -1;
  }

  decoderCommunication(data);

  printf("Resultat recu: %s\n", data);
 
  return 0;
}

int envoie_couleurs(int socketfd, char *pathname) {
  char data[1024];
  memset(data, 0, sizeof(data));
  strcpy(data, "couleurs: ");
  analyse(pathname, data, 10); // récupère les couleurs de l'image

  encoderCommunication(data);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int envoie_balises(int socketfd) {
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));


  // Demandez à l'utilisateur d'entrer son nombre de balises et ses balises
  char balises[1000];
  printf("Vos balises (max 1000 caracteres): ");
  fgets(balises, 1024, stdin);
  sprintf(data, "balises: %s", balises);

  encoderCommunication(data);
  
  int write_status = write(socketfd, data, strlen(data));
  if ( write_status < 0 ) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }
}

int envoie_plot(int socketfd, char *pathname, int nb_colors) {
    char data[1024];
    memset(data, 0, sizeof(data));
    strcpy(data, "plot: ");
    analyse(pathname, data, nb_colors); // récupère les couleurs de l'image

    int write_status = write(socketfd, data, strlen(data));
    if ( write_status < 0 ) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    return 0;
}

char* read_until_delim(FILE* file, char delim) {
    char* buffer = malloc(100);
    for (int i = 0; 1; ++i) {
        buffer[i] = fgetc(file);

        if (buffer[i] == delim) {
            buffer[i] = '\0';
            return buffer;
        }
    }
}

int envoie_json(int socketfd, char *pathname) {
    char buffer[1000];
    FILE* json = fopen(pathname, "r");

    for (int i = 0; 1; ++i) {
        buffer[i] = fgetc(json);

        if (buffer[i] == EOF) {
            break;
        }
        else if (buffer[i] == '{') {
            fgetc(json);
            char* line = read_until_delim(json, '}');

            // lecture jusqu'a code
            strtok(line, ":");
            strtok(NULL, "\"");
            // récupération du code
            char* code = strtok(NULL, "\"");
            printf("code: %s\n", code);

            // lecture des valeurs
            char* values[100];

            strtok(NULL, "[");
            char* token = strtok(NULL, "]");
            for (int j = 0; j <= strlen(token); ++j) {
                if (token[j] == '"') {
                    ++j;
                    while (token[j] != '"') { // TODO À débuguer
                        strncat(values[sizeof(values)/sizeof(values[0]) - 1], &token[j], 1);
                        ++j;
                    }
                    printf("test: %s", values[sizeof(values)/sizeof(values[0]) - 1]);
                    break;
                }
            }
            printf("\n");
        }
    }

    char data[1024];
    memset(data, 0, sizeof(data));
    strcpy(data, "json: ");

    int write_status = write(socketfd, data, strlen(data));
    if ( write_status < 0 ) {
        perror("erreur ecriture");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/* ===== MAIN ===== */
int main(int argc, char **argv) {
  int socketfd;
  int bind_status;

  struct sockaddr_in server_addr, client_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if ( socketfd < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  //détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  //demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if ( connect_status < 0 ) {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }


  if (!strcmp(argv[1], "message")) {
    envoie_recois_message(socketfd);
  }
  else if (!strcmp(argv[1], "nom")) {
    envoie_nom_de_client(socketfd);
  }
  else if (!strcmp(argv[1], "calcul")) {
    envoie_de_calcul(socketfd);
  }
  else if (!strcmp(argv[1], "couleurs")) {
    envoie_couleurs(socketfd, argv[2]); // chemin de l'image en deuxième argument
  }
  else if (!strcmp(argv[1], "balises")) {
    envoie_balises(socketfd);
  }
  else if (!strcmp(argv[1], "plot")) {
      if (atoi(argv[3]) > 30) {
          perror("Nombre de couleurs doit etre inferieur a 30");
          exit(EXIT_FAILURE);
      }
      envoie_plot(socketfd, argv[2], atoi(argv[3])); // chemin de l'image en deuxième argument et nombre de couleurs pour le troisième argument
  }
  else if (!strcmp(argv[1], "json")) {
      envoie_json(socketfd, argv[2]); // chemin du fichier "json" en deuxième argument
  }
  else {
    perror("Mauvais argument");
    exit(EXIT_FAILURE);
  }

  close(socketfd);
}

#pragma clang diagnostic pop