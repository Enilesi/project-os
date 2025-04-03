#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>

typedef struct Gps{
    float latitude;
    float longitude;
}Gps;

typedef struct Treasure{
    int id;
    char userName[100];
    Gps coordinates;
    char clue[100];
    int value;

}Treasure;

typedef struct Hunt{
    char id[100];
    Treasure *treasures;
    struct Hunt *next;
}Hunt;


int main(int argc, char * argv[]){
    if(argc<2){
        printf("invalid nuber of arguments");
        exit(1);
    }

    




}