#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "menus.h"

typedef struct roomData
{
    char ip[16], name[22], nickOwner[15];
    int capacity, status;
} roomData;

int registerRoom(int cp, char* owner, char* name, char* ip){
    //AQUI TEM PROBLEMA AO EXCLUIR
    FILE *arq = fopen("files/rooms", "ab+");

    int sz = sizeof(roomData);

    roomData *new = malloc(sz),
             *bufferRD = malloc(sz);

    new->capacity = cp;
    new->status = 1;
    strcpy(new->name, name);
    strcpy(new->nickOwner, owner);
    
    int count = 1;
    while(fread(bufferRD, sz, 1, arq) != 0){
        count++;
        if(!bufferRD->status) break;
    }

    char aux[3];
    sprintf(aux, "%d", count);

    strcat(ip, aux);

    strcpy(new->ip, ip);
    
    FILE *arq_temp = fopen("files/rooms_bkp", "ab+");
    rewind(arq);

    for (int i = 1; i < count; i++)
    {
        fread(bufferRD, sz, 1, arq);
        fwrite(bufferRD, sz, 1, arq_temp);
    }

    fseek(arq, sz*1, SEEK_CUR);
    int ret = fwrite(new, sz, 1, arq_temp);    

    if(ret){
        while (fread(bufferRD, sz, 1, arq) != 0){
            fwrite(bufferRD, sz, 1, arq_temp);
        }
        fclose(arq);
        remove("files/rooms");
        rename("files/rooms_bkp", "files/rooms");
    }else remove("files/rooms_bkp");

    fclose(arq_temp);
    free(new);
    free(bufferRD);
    
return ret;
}

char *listAndSelectRooms(){
    char *ret = malloc(16);

    while (1)
    {
        int sel = listOfRooms();

        if(!sel) {
            ret = "0";
            break;
        }

        FILE *arq = fopen("files/rooms", "rb");

        int sz = sizeof(roomData);
        roomData *bufferRD = malloc(sz);

        fseek(arq, (sel-1)*sz, SEEK_SET);
        fread(bufferRD, sz, 1, arq);

        fclose(arq);

        if(bufferRD == NULL){
            printf("\n-----------------------\n\n   Opção inválida...\n");
            sleep(1.4);
            continue;
        }
        //printf("%s\n", bufferRD->ip);
        strcpy(ret, (char*) &bufferRD->ip);
        free(bufferRD);
        break;
    }

return ret;    
}

void listAllRooms(){
    FILE *arq = fopen("files/rooms", "rb");

    int i = 0;

    if(arq != NULL){
        int sz = sizeof(roomData);
        roomData *bufferRD = malloc(sz);

        while (fread(bufferRD, sz, 1, arq) != 0){
            if(!bufferRD->status) continue;
            printf("\n  [%d] - %s, Vagas: %d - Dono: @%s\n", i+1, bufferRD->name, bufferRD->capacity, bufferRD->nickOwner);
            i++;
        }

        fclose(arq);
        free(bufferRD);
    }

    if(!i) {
        printf("\n  Ops... Nenhuma sala disponível o.O\n");
    }
    else printf("\nDigite o número da sala que deseja entrar:\n\n");
}