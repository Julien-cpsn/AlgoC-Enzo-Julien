#include <stdbool.h>
#include <string.h>
#include <malloc.h>

bool validate(char* json) {
    // lecture jusqu'à code
    if(json[0] != '{' && json[strlen(json) - 1] != '}') {
        return false;
    }

    strtok(json, ":");
    strtok(NULL, "\"");
    // récupération du code
    char* code = malloc(100);
    memcpy(code, "\0", 100);
    strcpy(code, strtok(NULL, "\""));
    strcat(code, ":");


    strtok(NULL, "[");
    char* token = strtok(NULL, "]");

    if (strcmp(code, "calcul:") == 0) {
        for (int i = 0; i <= strlen(token); ++i) {
            if (token[i] == '"') {
                ++i;
                while (token[i] != '"') {
                    ++i;
                }
            }
            else if (token[i] == ',') {
                i = i + 2;
                while (i < strlen(token) && token[i] != ',') {
                    ++i;
                }
                --i;
            }
        }
    }
    else {
        for (int i = 0; i <= strlen(token); ++i) {
            if (token[i] == '"') {
                ++i;
                while (token[i] != '"') {
                    ++i;
                }
            }
        }
    }

    free(code);
    return true;
}