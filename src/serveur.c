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
#include <math.h>


#include "serveur.h"

/* ===== FONCTIONS =====*/
char **nouveauTableauString() {
    char** array = malloc(sizeof(char*) * 50);
    if (array == NULL) {
        exit(1);
    }
    for(int i = 0; i < 50; i++){
        array[i] = (char *)malloc(50 + 1);
        if (array[i] == NULL) {
            exit(1);
        }
    }

    return array;
}

void afficherMessageRecu(char* code, char** values) {
    printf("Message\n Code: %s\n Valeurs:", code);
    for (int i = 0; i < sizeof(values); ++i) {
        if (strlen(values[i]) > 0) {
            printf("%s", values[i]);
            if (i != sizeof(values) - 1) {
                printf(", ");
            }
        }
    }
    printf("\n");
}

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

/* ===== JSON ===== */
void encoder(char* data){
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

    do {
        valeurs[indice] = malloc(sizeof(char));
        strcpy(valeurs[indice],segment);
        indice++;
        segment = strtok(NULL,delimiter);
    } while(segment != NULL);

    strcat(strcpy(json,"{"),"\"code\": \"");
    strcat(json,commande);
    strcat(json,"\", \"valeurs\": [");
    for(int i = 0; valeurs[i] != NULL && i < nombre_valeurs; ++i) {
        strcat(json,"\"");
        strcat(json,valeurs[i]);
        strcat(json,"\"");

        if(valeurs[i+1] != NULL){
            strcat(json,", ");
        }
    }
    strcat(json,"]}");
    strcpy(data,json);

    for(int i = 0; i <= indice; ++i) {
        free(valeurs[indice]);
    }
}

char* decoder(char* message, char* data) {
    // lecture jusqu'a code
    strtok(message, ":");
    strtok(NULL, "\"");
    // récupération du code
    char* code = malloc(100);
    memcpy(code, "\0", 100);
    strcpy(code, strtok(NULL, "\""));
    strcat(code, ":");
    strcpy(data, code);
    strcat(data, " ");


    strtok(NULL, "[");
    char* token = strtok(NULL, "]");
    for (int i = 0; i <= strlen(token); ++i) {
        if (token[i] == '"') {
            ++i;
            while (token[i] != '"') {
                strncat(data, &token[i], 1);
                ++i;
            }
            strcat(data, " ");
        }
    }

    return code;
}



/* ===== SERVICES ====== */
int renvoie_message(int client_socket_fd, char *data) {

    encoder(data);
  
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

    encoder(data);

    int data_size = write (client_socket_fd, (void *) data, strlen(data));

    if (data_size < 0) {
        perror("erreur ecriture");
        return(EXIT_FAILURE);
    }
}

double somme(double* array, int length)
{
    double sum = *array;
    for (int i = 1; i < length; ++i)
    {
        sum += *(array + i);
    } 
    return sum;
}

double soustraction(double* array, int length)
{
    double soustract = *array;
    for (int i = 1; i < length; ++i)
    {
        soustract -= *(array + i);
    } 
    return soustract;
}

double multiplication(double* array, int length)
{
    double multiple = *array;
    for (int i = 1; i < length; ++i)
    {
        multiple *= *(array + i);
    } 
    return (double)(multiple);
}

double division(double* array, int length)
{
    double divide = *array;
    for (int i = 1; i < length; ++i)
    {
        if(*(array + i)==0)
       {
           perror("division par zero");
           return(EXIT_FAILURE);
       } 
        divide /= *(array + i);
    } 
    return (double)(divide);
}

double moyenne(double* array, int length)
{
    return somme(array,length)/length;
}


/*
 * renvoyer un resultat de calcul (*data) au client (client_socket_fd)
 */
int renvoie_calcul(int client_socket_fd, char *data) {
    // Récupération de l'opérateur et des nombres
    char* tmp = strtok(data, " ");
    char* operateur = strtok(NULL, " ");
    double *numbers = malloc(sizeof(double)); 

    int length = 0;
    do
    {
        char* nextCharacter = strtok(NULL, " ");
        if(!nextCharacter)
            break;
        else
        { 
            numbers = realloc(numbers,sizeof(double) * (length + 1));
            numbers[length] = atof(nextCharacter);
        }
        length++;
    }while(length<100);

    // calcul
    double resultat = 0;
    if (!strcmp(operateur, "+")) {
        resultat = somme(numbers,length);
    }
    else if (!strcmp(operateur, "-")) {
        resultat = soustraction(numbers,length);
    }
    else if (!strcmp(operateur, "*")) {
        resultat = multiplication(numbers,length);
    }
    else if (!strcmp(operateur, "/")) {
        resultat = division(numbers,length);
    }
    else if (!strcmp(operateur, "moyenne")) {
        resultat = moyenne(numbers,length);
    } 
    else if (!strcmp(operateur, "minimum")) {
        double minimum = numbers[0];
        for(int i = 1; i < length; ++i)
        {
            if(minimum > numbers[i])
                minimum = numbers[i]; 
        } 
        resultat = minimum;
    }
    else if (!strcmp(operateur, "maximum")) {
        double maximum = numbers[0];
        for(int i = 1; i < length; ++i)
        {
            if(maximum < numbers[i])
                maximum = numbers[i]; 
        } 
        resultat = maximum;
    }
    else if (!strcmp(operateur, "ecart-type")) {
        double moyenne_au_carre = 0;
        for (int i = 0; i < length; ++i)
        {
            moyenne_au_carre += numbers[i] * numbers[i];
        } 
        moyenne_au_carre = moyenne_au_carre/length;
        resultat = sqrt((moyenne_au_carre - moyenne(numbers,length) * moyenne(numbers,length)));
    }

    // préparation de la réponse
    char reponse[50];
    strcpy(reponse,"calcul: ");
    sprintf(tmp, "%f", resultat);
    strcat(reponse,tmp);
    encoder(reponse);

    // envoi de la réponse
    int data_size = write (client_socket_fd, (void *) reponse, strlen(reponse));

    if (data_size < 0) {
        perror("erreur ecriture");
        return(EXIT_FAILURE);
    }

    free(numbers);

    return 0;
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

/* ===== RECEPTION ===== */
/*
 * Accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie(int socketfd) {
    struct sockaddr_in client_addr;
    char message[1024];

    int client_addr_len = sizeof(client_addr);

    // nouvelle connection de client
    int client_socket_fd = accept(socketfd, (struct sockaddr *) &client_addr, &client_addr_len);
    if (client_socket_fd < 0 ) {
        perror("accept");
        return(EXIT_FAILURE);
    }

    // la réinitialisation de l'ensemble des données
    memset(message, 0, sizeof(message));

    //lecture de données envoyées par un client
    int message_size = read (client_socket_fd, (void *) message, sizeof(message));

    if (message_size < 0) {
    perror("erreur lecture");
        return(EXIT_FAILURE);
    }

    /*
    * extraire le code des données envoyées par le client.
    * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
    */
    printf("Données reçues: %s\n\n", message);
    char data[1024];
    memset(data, '\0', 1024);

    char* code = decoder(message, data);
    printf("Données décodées: %s\n", data);

    // Si le message commence par le mot: 'message:', etc...
    if (strcmp(code, "message:") == 0) {
        renvoie_message(client_socket_fd, data);
    }
    else if (strcmp(code, "nom:") == 0) {
        renvoie_nom(client_socket_fd, data);
    }
    else if (strcmp(code, "calcul:") == 0) {
        printf("Calcul:\n");
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

    return 0;
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
