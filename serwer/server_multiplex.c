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

#define LINES 30
#define CHARS 100
#define READ_BUFFER 1024
#define WRITE_BUFFER 1024
#define MAX_CLIENTS_NUMBER 10

// void printText(char text[LINES][CHARS]){
//     printf("TEXT AFTER MODIFICATION\n");
//     for (int i = 0; i < LINES; i++) {
//         printf("%s", text[i]);
//     }
// }

void insertNewline(char s[LINES][CHARS], int line, int pos) {

    int i = pos, j = 0;
    char temp[CHARS], in[CHARS], tmp, c = '\n';

    memset(temp, 0, CHARS);
    memset(in, 0, CHARS);

    if (pos == 0) {
        strcpy(in, s[line]);
        strcpy(s[line], "\n");
    } else if (s[line][pos] == '\n') {
        strcpy(in, "\n");
    } else {
        tmp = s[line][i];
        s[line][i++] = c;
        c = tmp;
        do {
            in[j++] = c;
            c = s[line][i];
            s[line][i++] = '\0';
        } while (c);
        in[j] = '\0';
    }

    j = line;
    while (j<LINES-1) {
        j++;
        strcpy(temp, s[j]);
        strcpy(s[j], in);
        strcpy(in, temp);
    }
}

void insertCharacter(char s[LINES][CHARS], char c, int line, int pos) {
    char tmp;
    int i = pos;

    do {
        tmp = s[line][i];
        s[line][i++] = c;
        c = tmp;
    } while (c);
    s[line][i] = c;
}

void deleteCharacter(char s[LINES][CHARS], int line, int pos) {

    int j, i, len = (int) strlen(s[line]) - 1;
    if (pos > len) pos = len;
    else if (pos == -1) {
        if (line != 0) {
            line--;
            pos = (int) strlen(s[line]) - 1;
        } else return;
    }
    i = pos;

    if (s[line][i] == '\n') { //usuwanie znaku nowej linii i przesuwanie pozostałych linii
        s[line][i] = '\0';
        strcpy(s[line], strcat(s[line], s[line+1]));
        j = line;
        while (j < LINES -1) { 
            j++; 
            strcpy(s[j], s[j+1]); 
        };
        strcpy(s[j], "");
    } else {
        do {
            s[line][i] = s[line][i+1];
            i++;
        } while(s[line][i+1]);
        s[line][i] = '\0';
    }
}

struct file {
    int index;
    char name[100];
    char content[LINES][CHARS];
    int connectedClients[100];
};

void addEOFsign(char* message, int* size) {
    strcpy(message, strcat(message, "<<<!EOF!>>>"));
    *size += 11;
}

int prepareFilenames (struct file files[], int size, char titles[3000], char mode) {
    int tsize = 0, i;
    char str[2];
    memset(titles, 0, 3000);

    
    str[0] = mode;
    strcpy(titles, str);
    tsize++;

    for(i = 0; i < size; i++) {
        tsize += strlen(files[i].name);
        if (strcmp(files[i].name, "") != 0) {
            tsize += 1;
            strcpy(titles, strcat(titles, "#"));
            strcpy(titles, strcat(titles, files[i].name));
        }
    }

    addEOFsign(titles, &tsize);

    return tsize;
}



void addNewFile(struct file files[], int size, char filename[100]) {
    int i;
    for(i = 1; i < size; i++) {
        if (strcmp(files[i].name, "") == 0) {
            strcpy(files[i].name, filename);
            break;
        }
    }
}

void openFileForClient(struct file files[], int size, char filename[100], int clientFd, int clientFile[100]) {
    int index, i;
    for (i = 0; i<size; i++) {
        if(strcmp(files[i].name, filename) == 0) index = i;
    }
    clientFile[clientFd] = index;
}

int prepareFileContent (struct file f, char content[3000], char mode) {

    int tsize = 0, i;
    char str[3];
    char index[3];

    memset(content, 0, 3000);
    
    str[0] = mode;
    str[1] = '#';
    strcpy(content, str);
    tsize += 2;
    
    memset(index, 0, 3);
    sprintf(index,"%d", f.index);

    strcpy(content, strcat(content, index));
    tsize += strlen(index);

    strcpy(content, strcat(content, "#"));
    tsize++;

    for(i = 0; i < LINES; i++) {
        strcpy(content, strcat(content, f.content[i]));
        tsize += strlen(f.content[i]);
    }
    
    addEOFsign(content, &tsize);

    return tsize;
}

int main(int argc, char ** argv){
    
    int fd_server, fd_client, fdmax, fd_active, on = 1, i, j, fd, index, rcv;
    struct sockaddr_in client_addr, server_addr;
    socklen_t client_addr_size = sizeof(client_addr);
	static struct timeval timeout;
    fd_set connected, wmask, rmask, newly_connected, open_files_list, update_files_list, read_file_content, update_file_content, set_update_flag;

    struct file files[30];
    int receivedSize, sentSize, line, position, realSentSize, clientFile[100];
    char receivedMsg[100], buffer[READ_BUFFER], titles[3000], action, filename[100], *token, character, content[3000];

    //create & bind socket
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
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
    FD_ZERO(&read_file_content);
    FD_ZERO(&update_file_content);
    FD_ZERO(&set_update_flag);

    fdmax = fd_server;
	
    //prepare files 
    for (i = 0; i<30; i++) {
        files[i].index = i;
        strcpy(files[i].name, "");
        for (j = 0; j<LINES; j++) 
            strcpy(files[i].content[j], "");
    }
    strcpy(files[0].name, "file1");
    strcpy(files[0].content[0], "sample text");

    for (i = 0; i < 100; i++) { clientFile[i] = -1; }
    
	while(1) {
        
        //manage clients file descriptors
        for(fd = fd_server + 1; fd <= fdmax; fd++) {

            if (FD_ISSET(fd, &newly_connected)) { 
                FD_SET(fd, &connected); 
                FD_SET(fd, &open_files_list);
            }

            if (FD_ISSET(fd, &open_files_list)) FD_SET(fd, &wmask);

            if (FD_ISSET(fd, &update_files_list)) { 
                wmask = connected;
                update_files_list = connected;
            }

            if (FD_ISSET(fd, &read_file_content)) FD_SET(fd, &wmask);

            if (FD_ISSET(fd, &connected)) FD_SET(fd, &rmask);

            if (FD_ISSET(fd, &update_file_content) && !FD_ISSET(fd, &set_update_flag)) {
                index = clientFile[fd];
                for (i=0; i<100; i++) {
                    if (FD_ISSET(i, &connected) && clientFile[i] == index && !FD_ISSET(i, &set_update_flag)) {
                        FD_SET(i, &update_file_content);
                        FD_SET(i, &set_update_flag);
                        FD_SET(i, &wmask);
                    }
                }
            }
        }
        FD_SET(fd_server, &rmask);
        FD_ZERO(&newly_connected);
        FD_ZERO(&set_update_flag);


        // select read-from ready clients 
        timeout.tv_sec = 5*60; timeout.tv_usec = 0;
        if( (fd_active = select(fdmax+1, &rmask, &wmask, (fd_set*) NULL, &timeout)) == 0 ) {
            printf("Server timed out...\n"); continue;
        }


        // accept new clients 
        if (FD_ISSET(fd_server, &rmask)) {
            fd_active -= 1;
            fd_client = accept(fd_server, (struct sockaddr*) &client_addr, &client_addr_size);
            printf("New connection: %s,  fd: %d\n\n", inet_ntoa( (struct in_addr) client_addr.sin_addr), (int) fd_client);
            FD_SET(fd_client, &newly_connected);
            if(fd_client > fdmax) fdmax = fd_client; // update max file descriptor
        }

        // iterate active clients
        for (fd = fd_server + 1; fd <= fdmax && fd_active > 0; fd++) {

        	if (FD_ISSET(fd, &rmask)) {
        		fd_active -= 1;

                //safely read message from client
                rcv = 0;
                memset(receivedMsg, 0, 100);
        		while((receivedSize = read(fd, buffer, READ_BUFFER)) > 0) {
                    memcpy(receivedMsg+rcv, buffer, receivedSize);
                    rcv += receivedSize;
                    if (strstr(receivedMsg, "#<<<!!!EOF!!!>>>") != NULL) {
                        break;
                    }
                }

                //process message
                if(strlen(receivedMsg) != 0) {
                    
                    token = strtok(receivedMsg, "#");
                    if (token != NULL) action = *(token);
                    else action = ' ';
                    
                    if (action == 'o' || action == 'a' || action == 'c') {

                        token = strtok(NULL, "#");
                        strcpy(filename, token);     // open, add, close file
                        printf("%d -- %c %s\n\n", fd, action, filename);

                        if (action == 'a') {  // create new file on serwer
                            addNewFile(files, (int) sizeof(files)/sizeof(files[0]), filename);
                            FD_SET(fd, &update_files_list); 
                        }
                        else if (action == 'o') {  // open selected file
                            openFileForClient(files, (int) sizeof(files)/sizeof(files[0]), filename, fd, clientFile);
                            FD_SET(fd, &read_file_content);
                        }
                        else if (action == 'c') {  // close file (disconnect)
                            clientFile[fd] = -1;
                            FD_SET(fd, &update_files_list);
                        }


                    } else if (action == 'd' || action == 'i' || action == 'n') {

                        token = strtok(NULL, "#");
                        line = (int) strtol(token, (char **) NULL, 10);

                        token = strtok(NULL, "#");
                        position = (int) strtol(token, (char **) NULL, 10);

                        printf("%d -- %c %d %d", fd, action, line, position);

                        if (action == 'i') { 
                            token = strtok(NULL, "#");
                            character = (int) strtol(token, (char **)NULL, 10);

                            printf(" (%d)\n\n", (int) character);
                            insertCharacter(files[clientFile[fd]].content, character , line, position);
                        }
                        else if (action == 'd') {
                            printf("\n\n");
                            deleteCharacter(files[clientFile[fd]].content, line, position);
                        }
                        else if (action == 'n') {
                            printf("\n\n");
                            insertNewline(files[clientFile[fd]].content, line, position);
                        }
                        FD_SET(fd, &update_file_content);

                    } else if (action == 'r') {
                        printf("DISCONNECT client %d \n\n", fd);
                        FD_CLR(fd, &connected);
                        FD_CLR(fd, &open_files_list);
                        FD_CLR(fd, &update_files_list);
                        FD_CLR(fd, &read_file_content);
                        FD_CLR(fd, &update_file_content);
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

                if(FD_ISSET(fd, &open_files_list)) {
                    sentSize = prepareFilenames(files, (int) sizeof(files)/sizeof(files[0]), titles, 'f');
                    realSentSize = write(fd, titles, sentSize);
                    printf("SENT %d bytes to client %d - OPEN FILES\n\n", realSentSize, fd);
                    FD_CLR(fd, &open_files_list);
                }

                if(FD_ISSET(fd, &update_files_list)) {
                    sentSize = prepareFilenames(files, (int) sizeof(files)/sizeof(files[0]), titles, 'F');
                    realSentSize = write(fd, titles, sentSize);
                    printf("SENT %d bytes to client %d - UPDATE OPEN FILES\n\n", realSentSize, fd);
                    FD_CLR(fd, &update_files_list);
                }

                if(FD_ISSET(fd, &read_file_content)) {
                    sentSize = prepareFileContent(files[clientFile[fd]], content, 'o');
                    realSentSize = write(fd, content, sentSize);
                    printf("SENT %d bytes to client %d - FILE CONTENT\n\n", realSentSize, fd);
                    FD_CLR(fd, &read_file_content);
                }

                if(FD_ISSET(fd, &update_file_content)) {
                    sentSize = prepareFileContent(files[clientFile[fd]], content, 'u');
                    realSentSize = write(fd, content, sentSize);
                    printf("SENT %d bytes to client %d - UPDATE FILE CONTENT\n\n", realSentSize, fd);
                    FD_CLR(fd, &update_file_content);
                }

				FD_CLR(fd, &wmask);
            }
        }
	}
	close(fd_server);
	return 0;
}
