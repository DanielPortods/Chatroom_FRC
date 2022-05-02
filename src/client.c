#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include "server.h"
#include "menus.h"
#include "dialog.h"
#include "roomReg.h"

char *NICK;
int SD;
pthread_t myRoom;

int main(){
    wellcome();

    NICK = identify();

    /* Cria socket */
	SD = socket(AF_INET, SOCK_STREAM, 0);
	if (SD < 0)
	{
		fprintf(stderr, "Criacao do socket falhou!\n");
		exit(1);
	}

    while (1)
    {
        int enter = home(NICK);

        if(enter == 1){
            char *addr_s;

            myRoom = createRoom(NICK, &addr_s);
            
            if(myRoom == 0) continue;

			struct sockaddr_in room;
			room.sin_family = AF_INET;
    		room.sin_addr.s_addr = inet_addr(addr_s);
   			room.sin_port = htons(5200);
   			memset(&(room.sin_zero), '\0',8); 

            if (connect(SD, (struct sockaddr *)&room, sizeof(room)) < 0){
		        fprintf(stderr, "Tentativa de conexao falhou!\n");
                pthread_join(myRoom, NULL);
		        exit(1);
            }
            
            startDialogProccess(SD, NICK);            
        }
        else if(enter == 2){
            char *addr_room = listAndSelectRooms();
            if (!strcmp(addr_room, "0")) continue;
            printf("==> %s\n", addr_room);
        }
        else {
            printf("encerrando...\n");
            sleep(1.5);
            break;
        }
    }

	close(SD);
return 0;
}