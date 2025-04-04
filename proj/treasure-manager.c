#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

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

#define TREASURE_PATH_LEN 256

void add_hunt(const char *hunt_id) {
    char path[TREASURE_PATH_LEN];
    snprintf(path, sizeof(path), "hunt/%s", hunt_id);
    struct stat st = {0};

    if (stat("hunt", &st) == -1) {
        mkdir("hunt", 0700);
    }

    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }

    char data_path[TREASURE_PATH_LEN], log_path[TREASURE_PATH_LEN];
    snprintf(data_path, sizeof(data_path), "%s/treasures.dat", path);
    snprintf(log_path, sizeof(log_path), "%s/treasures.log", path);

    int fd1 = open(data_path, O_CREAT | O_APPEND, 0644);
    if (fd1 >= 0) close(fd1);
    int fd2 = open(log_path, O_CREAT | O_APPEND, 0644);
    if (fd2 >= 0) close(fd2);
}




int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Invalid number of arguments.\n");
        return 1;
    }

    if (strcmp(argv[1], "--add") == 0 && argc == 3) {
        const char *hunt_id = argv[2];
        add_hunt(hunt_id);
        
    } else {
        printf("Invalid or unsupported command.\n");
        return 1;
    }

    return 0;
}