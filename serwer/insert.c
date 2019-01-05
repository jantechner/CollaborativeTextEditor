#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void insertCharacter(char s[20][1000], char c, int line, int pos) {
    char tmp;
    int i = pos;

    if (c == '\n') {
        char temp[1000], in[1000];

        if (pos == 0) {
            strcpy(in, s[line]);
            strcpy(s[line], "\n");
        } else {
            if (s[line][pos] == '\n') {
                strcpy(in, "\n");
            } else {
                do {
                    tmp = s[line][i];
                    s[line][i++] = c;
                    c = tmp;
                } while (c);
                s[line][i] = c;

                char *tok, *newstr1, *newstr2;
                tok = strtok(s[line], "\n");
                newstr1 = malloc(strlen(tok) + 2);
                strcpy(newstr1, tok);
                strcat(newstr1, "\n");

                tok = strtok(NULL, "\n");
                newstr2 = malloc(strlen(tok) + 2);
                strcpy(newstr2, tok);
                strcat(newstr2, "\n");
                
                strcpy(s[line], newstr1);
                strcpy(in, newstr2);

                free(newstr1);
                free(newstr2);
            }
        }
        int j = line;
        while (j<19) {
            j++;
            strcpy(temp, s[j]);
            strcpy(s[j], in);
            strcpy(in, temp);
        }
    } else {
        do {
            tmp = s[line][i];
            s[line][i++] = c;
            c = tmp;
        } while (c);
        s[line][i] = c;
    }
}

void deleteCharacter(char s[20][1000], int line, int pos) {

    int i, len = (int) strlen(s[line]);
    if (pos > len) pos = len;
    i = pos;

    if (s[line][i] == '\n') { //usuwanie znaku nowej linii i przesuwanie pozostałych linii
        s[line][i] = '\0';
        strcpy(s[line], strcat(s[line], s[line+1]));
        int j = line;
        while (j < 19) { 
            j++; 
            strcpy(s[j], s[j+1]); 
        };
        strcpy(s[j], "\n");
    } else {
        do {
            s[line][i] = s[line][i+1];
            i++;
        } while(s[line][i+1]);
        s[line][i] = '\0';
    }
}

void printText(char text[20][1000]){
    printf("TEXT AFTER MODIFICATION\n");
    for (int i = 0; i < 20; i++) {
        printf("%s", text[i]);
    }
}

int main(int argc, char const *argv[])
{
    char wyraz[20][1000];

    for (int i = 0; i<20; i++){
        strcpy(wyraz[i], "\n");
    }
    strcpy(wyraz[0], "janek\n");
    strcpy(wyraz[1], "gosia\n");

    // insertCharacter(wyraz, '1', 0, 5);
    // insertCharacter(wyraz, '2', 1, 0);
    // insertCharacter(wyraz, '3', 2, 0);
    // insertCharacter(wyraz, '4', 3, 0);
    // insertCharacter(wyraz, '5', 4, 0);
    // insertCharacter(wyraz, '6', 5, 0);
    // insertCharacter(wyraz, '7', 6, 0);
    // insertCharacter(wyraz, '8', 7, 0);
    // insertCharacter(wyraz, '9', 8, 0);
    // insertCharacter(wyraz, '1', 9, 0);
    // insertCharacter(wyraz, '2', 10, 0);
    // insertCharacter(wyraz, '3', 11, 0);
    // insertCharacter(wyraz, '4', 12, 0);
    // insertCharacter(wyraz, '5', 13, 0);
    // insertCharacter(wyraz, '6', 14, 0);
    // insertCharacter(wyraz, '7', 15, 0);
    // insertCharacter(wyraz, '8', 16, 0);
    // insertCharacter(wyraz, '9', 17, 0);

    // deleteCharacter(wyraz, 2, 0);
    // deleteCharacter(wyraz, 9, 6);
    // deleteCharacter(wyraz, 9, 6);

    // insertCharacter(wyraz, '\n', 0, 3);
    // insertCharacter(wyraz, '\n', 0, 3);
    // insertCharacter(wyraz, '\n', 0, 2);
    // insertCharacter(wyraz, '\n', 0, 0);

    printText(wyraz);

    // insertCharacter(wyraz, '4', 10);
    // insertCharacter(wyraz, '5', 10);
    // insertCharacter(wyraz, '6', 10);
    // insertCharacter(wyraz, '7', 10);
    // insertCharacter(wyraz, '8', 10);

    return 0;
}
