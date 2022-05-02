#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int SD_C;

void *receiveAndShowMsg(){
    while (1){
        
		char res[256];
        
		memset(&res, 0x0, sizeof(res));
        
		recv(SD_C, &res, sizeof(res), 0);

		fprintf(stdout, "\n%s> ", res);
		fflush(stdout);
	}
}

void startDialogProccess(int sd, char* nick){
    SD_C = sd;
    
    pthread_t recv_th;
    
    pthread_create(&recv_th, NULL, receiveAndShowMsg, NULL);
    
    int sz = strlen(nick);
    char *identfy = malloc((sz+5)*sizeof(char));

    identfy[0] = '[';
    
    strncat(identfy, nick, sz);

    strcat(identfy, "]: \0");
    
	while (1){   
        char msg[276], bufout[256];
        memset(&msg, 0x0, sizeof(msg));

        strcpy(msg, identfy);

		printf("> ");
        fflush(stdin);
        fscanf(stdin, " %[^\n]", bufout);
		 
		if (strncmp(bufout, "/q", 2) == 0) break;

        strcat(msg, bufout);
        strcat(msg, "\n");
        send(sd, &msg, strlen(msg), 0);
	}
	
    printf("------- encerrando conexao com o servidor -----\n");
    pthread_cancel(recv_th);
    pthread_join(recv_th, NULL);
		
}

int connectServer(char* addr, int sd){

    struct sockaddr_in room;
	room.sin_family = AF_INET;
    room.sin_addr.s_addr = inet_addr(addr);
   	room.sin_port = htons(5200);
   	memset(&(room.sin_zero), '\0', 8); 

    if (connect(sd, (struct sockaddr *)&room, sizeof(room)) < 0){
	    fprintf(stderr, "\nTentativa de conexao falhou!\n");
	    return 1;
    }

return 0;
}