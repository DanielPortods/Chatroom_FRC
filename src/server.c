#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "roomReg.h"

#define STDIN 0

int PORT = 5200, CAPACITY, TRUE = 1, SD_S, ADDRLEN, FDMAX;
char IP_ADR[16] = "127.0.0.", NAME[22], NICKOWNER[15], BUF[256];
fd_set MASTER, READ_FDS, WRITE_FDS;
struct sockaddr_in 	MYADDR, REMOTEADDR;


typedef struct roomData
{
    char ip[16], name[22], nickOwner[15];
    int capacity;
} roomData;

void send_msg(int arg, int n) {
    for (int i = 0; i <= FDMAX; i++) {
        if (FD_ISSET(i, &MASTER)){
                if ((i != arg) && (i != SD_S)) send(i, BUF, n, 0);
        }
    }
}

/*int registerRoom(){
    //AQUI TEM PROBLEMA AO EXCLUIR
    FILE *arq = fopen("files/rooms", "ab+");

    roomData *new = malloc(sizeof(roomData)),
             *bufferRD = malloc(sizeof(roomData));

    new->capacity = CAPACITY;
    strcpy(new->name, NAME);
    strcpy(new->nickOwner, NICKOWNER);

    int count = 1;
    while(fread(bufferRD, sizeof(roomData), 1, arq) != 0) count++;
    printf("%d\n", count);
   
    char aux[3];
    sprintf(aux, "%d", count);

    strcat(IP_ADR, aux);
    
    strcpy(new->ip, IP_ADR);
    
return fwrite(new, sizeof(roomData), 1, arq);
}*/

void *launchRoom(){
    FD_ZERO(&MASTER); 
    FD_ZERO(&READ_FDS);
    
    SD_S = socket(AF_INET, SOCK_STREAM, 0);
    
    if (SD_S < 0){
        fprintf(stderr, "Falha ao criar socket do server da sala %s!\n", NAME);
        exit(1);
    }

    setsockopt(SD_S, SOL_SOCKET, SO_REUSEADDR, &TRUE, sizeof(int));

    if(!registerRoom(CAPACITY, NICKOWNER, NAME, IP_ADR)){
        fprintf(stderr, "Falha ao registrar a sala %s!\n", NAME);
        exit(1);
    }
    
    MYADDR.sin_family = AF_INET;
    MYADDR.sin_addr.s_addr = inet_addr(IP_ADR);
    MYADDR.sin_port = htons(PORT);

    memset(&(MYADDR.sin_zero), '\0',8); 
    ADDRLEN = sizeof(REMOTEADDR);
    if(bind(SD_S, (struct sockaddr *)&MYADDR, sizeof(MYADDR)) < 0){
        fprintf(stderr, "Ligacao Falhou!\n");
        exit(1);
    }

    listen(SD_S, CAPACITY); 
    FD_SET(SD_S, &MASTER);
    FD_SET(STDIN, &MASTER);
    FDMAX = SD_S;

    while (1){
       READ_FDS = MASTER;
       
       select(FDMAX+1, &READ_FDS, NULL, NULL, NULL);

       for (int i = 0; i <= FDMAX; i++) {
	        if (FD_ISSET(i, &READ_FDS)) {
	            if (i == SD_S) {
	                int newfd = accept(SD_S, (struct sockaddr *)&REMOTEADDR, (socklen_t*)&ADDRLEN);
	                FD_SET(newfd, &MASTER);
	                if (newfd > FDMAX) FDMAX = newfd;
	            } 
                else {
		        	memset (&BUF, 0 , sizeof (BUF));
	        		int n = recv(i, BUF, sizeof(BUF), 0);
	        		send_msg(i, n); 
                } 
            }
  	    }
	}    
}

pthread_t createRoom(char* nick, char* ladoServ){

    strcpy(NICKOWNER, nick);

    while (1)
    {
        printf("\n-------------------------------------\n");
        printf("Digite um nome para a sua sala (max 20 caracteres):\n [0] Cancelar\n\n>> ");
        int tam = scanf(" %[^\n]", NAME);

        if(strlen(NAME) == 1 && NAME[0] == '0') return 0;
        else if(tam > 20) {
            printf("Nome muito longo! Max 20 caracteres...\n");
            sleep(2);
            continue;
        }
        else break;        
    }

    while (1)
    {
        printf("\n-------------------------------------\n");
        printf("Digite a capacidade da sua sala (max 20 participantes):\n [0] Cancelar\n\n>> ");
        
        scanf(" %d", &CAPACITY);

        if(CAPACITY == 0) return 0;

        else if(CAPACITY > 20) {
            printf("O número máximo de participantes é 20!\n");
            sleep(2);
            continue;
        }
        else break;
    }

    pthread_t room;
    pthread_create(&room, NULL, launchRoom, NULL);
    sleep(2);

    char *ip_r = inet_ntoa(MYADDR.sin_addr);
    
    memcpy(ladoServ, &ip_r, sizeof(ip_r));

return room;
}