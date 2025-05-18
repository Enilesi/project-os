#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "treasure.h"
#include <fcntl.h>
#include <errno.h>

typedef struct Player{
    char name[100];
    long score;
} Player;

Player players[512];
int playerLen=0;

int get_player_id(char *name ){

    for( int i=0;i<playerLen;i++){
        if(strncmp(name,players[i].name,100)==0){
            return i;
        }
    }
return -1;

}


int main(int argc, char * argv[]){

   if (argc<2){
    printf("Arguments error. Hunt ID must be provided");
   } 

   const char* hunt_id=argv[1];


   char data_path[TREASURE_PATH_LEN];
   snprintf(data_path, sizeof(data_path), "hunt/%s/treasures.dat", hunt_id);

   int treasure_file= open(data_path,O_RDONLY);
   if (treasure_file == -1) {
    fprintf(stderr, "Error opening file '%s': %s\n", data_path, strerror(errno));
    return 1;
    
}
   Treasure treasure;

   while(read(treasure_file,&treasure,sizeof(treasure))>0){
        int playerId= get_player_id(treasure.userName);
        if (playerId == -1) {

        playerId = playerLen++;
        players[playerId].score = treasure.value;
        strncpy(players[playerId].name, treasure.userName, 100);
        
    }
        else{

            players[playerId].score+=treasure.value;
        
        }


   }
   printf("Hunt: %s, users: %d\n",hunt_id,playerLen);

   for(int i=0;i<playerLen;i++){
    
    printf("Player: %s,%d\n", players[i].name, players[i].score);
   
}


}

