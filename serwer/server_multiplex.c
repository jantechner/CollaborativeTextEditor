#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>

#define LINES 20
#define CHARS 10
#define READ_BUFFER 1024
#define WRITE_BUFFER 1024
#define MAX_CLIENTS_NUMBER 10

void insertCharacter(char s[LINES][CHARS], char c, int line, int pos) {
    char tmp;
    int i = pos;

    if (c == '\n') {
        char temp[CHARS], in[CHARS];

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
        while (j<LINES-1) {
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

void deleteCharacter(char s[LINES][CHARS], int line, int pos) {

    int i, len = (int) strlen(s[line]);
    if (pos > len) pos = len;
    i = pos;

    if (s[line][i] == '\n') { //usuwanie znaku nowej linii i przesuwanie pozostałych linii
        s[line][i] = '\0';
        strcpy(s[line], strcat(s[line], s[line+1]));
        int j = line;
        while (j < LINES -1) { 
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

void printText(char text[LINES][CHARS]){
    printf("TEXT AFTER MODIFICATION\n");
    for (int i = 0; i < LINES; i++) {
        printf("%s", text[i]);
    }
}

struct file {
    int index;
    char name[100];
    char content[LINES][CHARS];
    int connectedClients[100];
};

int prepareFilenames (struct file files[], int size, char titles[3000], char mode) {
    int tsize = 0;
    memset(titles, 0, 3000);

    char str[2];
    str[0] = mode;
    strcpy(titles, str);
    tsize++;

    for(int i = 0; i < size; i++) {
        tsize += strlen(files[i].name);
        if (strcmp(files[i].name, "") != 0) {
            tsize += 1;
            strcpy(titles, strcat(titles, "#"));
            strcpy(titles, strcat(titles, files[i].name));
        }
    }
    return tsize;
}

void addEOFsign(char* message, int* size) {
    strcpy(message, strcat(message, "<<<!EOF!>>>"));
    *size += 11;
}

void addNewFile(struct file files[], int size, char filename[100]) {
    for(int i = 1; i < size; i++) {
        if (strcmp(files[i].name, "") == 0) {
            strcpy(files[i].name, filename);
            break;
        }
    }
}

void openFileForClient(struct file files[], int size, char filename[100], int clientFd, int clientFile[100]) {
    int index;
    for (int i = 0; i<size; i++) {
        if(strcmp(files[i].name, filename) == 0) index = i;
    }
    clientFile[clientFd] = index;
}

int prepareFileContent (struct file f, char content[3000], char mode) {

    int tsize = 0;
    memset(content, 0, 3000);

    char str[3];
    str[0] = mode;
    str[1] = '#';
    strcpy(content, str);
    tsize += 2;

    char index[3];
    sprintf(index,"%d", f.index);

    strcpy(content, strcat(content, index));
    tsize += strlen(index);

    strcpy(content, strcat(content, "#"));
    tsize++;

    for(int i = 0; i < LINES; i++) {
        strcpy(content, strcat(content, f.content[i]));
        tsize += strlen(f.content[i]);
    }
    // printf("%s\n", content);
    return tsize;
}

int main(int argc, char ** argv){
    
    int fd_server, fd_client, fdmax, fd_active, on = 1;
    struct sockaddr_in client_addr, server_addr;
    socklen_t client_addr_size = sizeof(client_addr);
	static struct timeval timeout;
    fd_set connected, wmask, rmask, newly_connected, open_files_list, update_files_list, open_file;

    struct file files[30];
    int receivedSize, sentSize, line, position, realSentSize, clientFile[100] = {-1};
    char receivedMsg[100], titles[3000], action, filename[100], *token, character, content[3000];

    //create & bind socket
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(1234);
	server_addr.sin_addr.s_addr = inet_addr("192.168.111.128");
	fd_server = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on));   
	bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr));
	listen(fd_server, MAX_CLIENTS_NUMBER);

    //clear file descriptors sets
    FD_ZERO(&connected);
    FD_ZERO(&rmask);
    FD_ZERO(&wmask);
    FD_ZERO(&newly_connected);
    FD_ZERO(&open_files_list);
    FD_ZERO(&update_files_list);
    FD_ZERO(&open_file);

    fdmax = fd_server;


    
	
    //prepare files 
    for (int i = 0; i<30; i++) {
        files[i].index = i;
        strcpy(files[i].name, "");
        for (int j = 0; j<LINES; j++) 
            strcpy(files[i].content[j], "\n");
    }
    strcpy(files[0].name, "file1");
    strcpy(files[0].content[0], "janek\n");
    strcpy(files[0].content[1], "gosia\n");
    strcpy(files[0].content[2], "adam\n");

    for (int i = 0; i < 100; i++) { clientFile[i] = -1; }



	while(1) {
        
        //manage clients file descriptors
        for(int fd = fd_server + 1; fd <= fdmax; fd++) {

            if (FD_ISSET(fd, &newly_connected)) { FD_SET(fd, &connected); FD_SET(fd, &open_files_list); }

            if (FD_ISSET(fd, &open_files_list)) { 
                printf("Add client %d to open_files_list set - allow writing to client\n", fd);
                FD_SET(fd, &wmask);
            }

            if (FD_ISSET(fd, &update_files_list)) { 
                printf("Add client %d to update_files_list set - all clients writing\n", fd);
                wmask = connected;
                update_files_list = connected;
            }

            if (FD_ISSET(fd, &open_file)) { 
                printf("Client %d allowed to open file\n", fd);
                FD_SET(fd, &wmask);
            }

            if (FD_ISSET(fd, &connected)) {
                printf("Add client %d to read set\n", fd);
                FD_SET(fd, &rmask);
            } 
        }
        FD_SET(fd_server, &rmask);
        FD_ZERO(&newly_connected);


        // select read-from ready clients 
        timeout.tv_sec = 15; timeout.tv_usec = 0;
        printf("Selecting ready file descriptors...\n");
        if( (fd_active = select(fdmax+1, &rmask, &wmask, (fd_set*) NULL, &timeout)) == 0 ) {
            printf("Server timed out...\n"); continue;
        }
        printf("Selection completed\n");


        // accept new clients 
        if (FD_ISSET(fd_server, &rmask)) {
            printf("Accepting new clients...\n");
            fd_active -= 1;
            fd_client = accept(fd_server, (struct sockaddr*) &client_addr, &client_addr_size);
            printf("New connection: %s, fd: %d\n", inet_ntoa( (struct in_addr) client_addr.sin_addr), (int) fd_client);
            FD_SET(fd_client, &newly_connected);
            if(fd_client > fdmax) fdmax = fd_client; // update max file descriptor
        }

        // iterate active clients
        for (int fd = fd_server + 1; fd <= fdmax && fd_active > 0; fd++) {
            

        	if (FD_ISSET(fd, &rmask)) {
        		fd_active -= 1;
                memset(receivedMsg, 0, 100);
        		receivedSize = read(fd, receivedMsg, READ_BUFFER);
                // printf("%d %s\n", receivedSize, receivedMsg);
                if(strlen(receivedMsg) != 0) {
                    
                    token = strtok(receivedMsg, "#");
                    action = *(token);

                    token = strtok(NULL, "#");
                    
                    if (action == 'o' || action == 'a' || action == 'c') {

                        strcpy(filename, token);     // open, add, close file
                        printf("Command from %d - %c %s\n", fd, action, filename);

                        if (action == 'a') { 
                            addNewFile(files, (int) sizeof(files)/sizeof(files[0]), filename);
                            FD_SET(fd, &update_files_list); 
                        }
                        else if (action == 'o') {
                            openFileForClient(files, (int) sizeof(files)/sizeof(files[0]), filename, fd, clientFile);
                            FD_SET(fd, &open_file);
                        }
                        else if (action == 'c') {
                            clientFile[fd] = -1;
                            FD_SET(fd, &update_files_list);
                        }


                    } else if (action == 'd' || action == 'i') {
                        line = (int) strtol(token, (char **) NULL, 10);
                        token = strtok(NULL, "#");
                        position = (int) strtol(token, (char **) NULL, 10);
                        token = strtok(NULL, "#");
                        character = *(token);
                        printf("Command from %d - %c %d %d %c\n", fd, action, line, position, character);
                    } else if (action == 'r') {
                        printf("Command from %d - %c\n", fd, action);
                        FD_CLR(fd, &connected);
                        FD_CLR(fd, &open_files_list);
                        FD_CLR(fd, &update_files_list);
                        FD_CLR(fd, &open_file);
                        FD_CLR(fd, &rmask);
                        FD_CLR(fd, &wmask);
                        clientFile[fd] = -1;
                        close(fd);
                        // update max file descriptor
                        if(fd == fdmax)
                            while(fdmax > fd_server && !FD_ISSET(fdmax, &connected))
                                fdmax -= 1;
                    }
                }
				FD_CLR(fd, &rmask);
        	} 
            else if (FD_ISSET(fd, &wmask)) {
                fd_active -= 1;
                // printf("%d ready to write to\n", (int) fd);

                //     char result[] = "mYZPHYejYxcKYk3U7ctACnU7yfwQbY7sFYNWVWyqHvFmT40tNyI3oP3MMejRWD0Djf4V2Wqccz06traUmn74TltzMb3ITPTLDJavMmiwFCVrP3ZM40zdOKKQgBDoNkc2V9N6WSvX0H4wFKH4e6jmoRNJZ08wjsdzXcPGEKWztfCrLiZvfp63PX0BkuwUNvrkKqZqkC2oDpjhFRCSKA1WHPM03xDYkrWqxMl9Cp2hiqNoxDWZDkTm5trz9WbhXOBBpGMU9DN5TJGH2PUfchEoPI609mmuNRhIEnvxQoyYEQzhWdQzCyhGaMu2aAEoPvGwgtxxjAhiFuxxs0n5B9vEgRfCl3MHcQbn52stvPMcRQKieJ0u0UU88FTyCTOUyVDlZFRzsL9BkVFhSFCHmjh4Hw8MLelyNE39o0rTOkRCm6NKcVkShsgAiRcYhWRviEWKX2EeG0XFpwGgoTxTGhHGDJ7LvHKsIFswDZh8aNH13BFHXI10lYGu1I7SbvC7wNfbKYffC5ui0sNrwZCNmC0Jl07viANzCeedBAoEB44o0Rdm0TXnYh2yRzjgQOOUHxaTsGh6LPuRNLCtXnsrXNNs3oRSM5kDxlsDfsZwv2MrSRBJfr5ptoFK9TdwIAUzLkN49QFyAdW3agxBBGnntVnkAmfrksDm3ffKbvl008H07WS7jGmhvtBxkGgkAR7SKBHKENSRMvSQ53MZ6N79cYFfH0ZfAMZb7UxuiUHZ8BMFiqpnnqIKQkhskhPz6nzDabZV735Qw2iySCHc2tjoW1VAwI5obdatWF1gB1BbjzDVc7ykgIypMi4DceL3TH3IJPSngNUEDpXH7MDfhZnyoAvJvBsJDogXq3EVfWFcEsRuW69BjoXKN9VP82jmMj2hS4n8L4RO018qNhtr8rcbxdagA3X4kcvTMh2ln8jtPDjubENxcoHOq4bWA2weqW2sByzyXqKDuOuPvsIWk4LSFQEaCe1LD3r8XN1jxVYUSMX9rSWbgfzFPUTeVEnl8IzRhff2lDHz9wlwaWNtiOxtwr4mA4X2OL6skXv6wNK4yAvT2bk99N9vo1gxaxmRJEEPF7RlOZgmr2e87GOwAJxs4Kf0Wrw21XvmAgYwQ2wahv3LQHtwViHUbpHzW97XBSCsQvnaXdPF6OS5z4X2uWawTk4ioR5dSANo1VfOikcJliyGvcKCWUtKTFc1zIHlnMCOL1QBZK0pELSehHNqVadzH8zz2AxfetjEiFoPDRVKjBl9EqSqAOSJxOaUOm85xXOn7oYJkEaq6rxSftSZHGIqY8yIA9lnuHZp7eU0pupd2XC4AL61A2GZp67iH94UJWLiE8ExQfWCtqxkxuuTxu3ysr4j9IGDHuwvJpyAMrwflk7ZoFQADQ4PJguQ0EPzYJomb6SzqA6PMPMh6pRv2UZcMU9saoXwWeyIqDlwOjcQg5l63Lrahp5c0ZJTFDWuDywN65R0KqWmJ4wFd6JybfhxYeVLWWDdf5fw";
                //     int s = (int) strlen(result);
                //     addEOFsign(result, &s);
                //     printf("%s   %d", result, s);

                if(FD_ISSET(fd, &open_files_list)) {
                    sentSize = prepareFilenames(files, (int) sizeof(files)/sizeof(files[0]), titles, 'f');
                    addEOFsign(titles, &sentSize);
                    realSentSize = write(fd, titles, sentSize);
                    printf("%d bytes of data sent to client %d - open_files_list\n", realSentSize, fd);
                    FD_CLR(fd, &open_files_list);
                }

                if(FD_ISSET(fd, &update_files_list)) {
                    sentSize = prepareFilenames(files, (int) sizeof(files)/sizeof(files[0]), titles, 'F');
                    addEOFsign(titles, &sentSize);
                    realSentSize = write(fd, titles, sentSize);
                    printf("%d bytes of data sent to client %d - update_files_list\n", realSentSize, fd);
                    FD_CLR(fd, &update_files_list);
                }

                if(FD_ISSET(fd, &open_file)) {
                    sentSize = prepareFileContent(files[clientFile[fd]], content, 'o');
                    addEOFsign(content, &sentSize);
                    realSentSize = write(fd, content, sentSize);
                    printf("%d bytes of data sent to client %d - open_file\n", realSentSize, fd);
                    FD_CLR(fd, &open_file);
                }

				FD_CLR(fd, &wmask);
            }
        }
        printf("Koniec pętli\n");
	}
	close(fd_server);
	return 0;
}
