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

#define TREASURE_PATH_LEN 256

typedef struct {
    float latitude;
    float longitude;
} Gps;

typedef struct {
    int id;
    char userName[100];
    Gps coordinates;
    char clue[100];
    int value;
} Treasure;

typedef struct Hunt {
    char id[100];
    Treasure *treasures;
    struct Hunt *next;
} Hunt;

void log_message(const char *hunt_id, const char *message) {
    char log_path[TREASURE_PATH_LEN];
    snprintf(log_path, sizeof(log_path), "hunt/%s/treasures.log", hunt_id);

    int log_fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd < 0) return;

    time_t now = time(NULL);
    char *time_str = strtok(ctime(&now), "\n");
    dprintf(log_fd, "[%s] %s\n", time_str, message);
    close(log_fd);

    if (access("log", F_OK) == -1) {
        mkdir("log", 0700);
    }

    char symlink_path[TREASURE_PATH_LEN];
    snprintf(symlink_path, sizeof(symlink_path), "log/logged_hunt-%s", hunt_id);
    unlink(symlink_path);
    symlink(log_path, symlink_path);
}

void create_hunt(const char *hunt_id) {
    if (access("hunt", F_OK) == -1) {
        mkdir("hunt", 0700);
    }

    char hunt_path[TREASURE_PATH_LEN];
    snprintf(hunt_path, sizeof(hunt_path), "hunt/%s", hunt_id);

    if (access(hunt_path, F_OK) == -1) {
        mkdir(hunt_path, 0700);
    }

    char data_path[TREASURE_PATH_LEN];
    snprintf(data_path, sizeof(data_path), "%s/treasures.dat", hunt_path);
    int data_fd = open(data_path, O_CREAT | O_APPEND, 0644);
    if (data_fd >= 0) close(data_fd);

    char log_path[TREASURE_PATH_LEN];
    snprintf(log_path, sizeof(log_path), "%s/treasures.log", hunt_path);
    int log_fd = open(log_path, O_CREAT | O_APPEND, 0644);
    if (log_fd >= 0) close(log_fd);

    log_message(hunt_id, "Created hunt");
}

Treasure input_treasure() {
    Treasure t;

    printf("Enter Treasure ID: ");
    scanf("%d", &t.id);

    printf("Enter Username: ");
    scanf("%s", t.userName);

    printf("Enter Latitude: ");
    scanf("%f", &t.coordinates.latitude);

    printf("Enter Longitude: ");
    scanf("%f", &t.coordinates.longitude);

    printf("Enter Clue: ");
    scanf(" %[^\n]", t.clue);

    printf("Enter Value: ");
    scanf("%d", &t.value);

    return t;
}

void add_treasure(const char *hunt_id) {
    char data_path[TREASURE_PATH_LEN];
    snprintf(data_path, sizeof(data_path), "hunt/%s/treasures.dat", hunt_id);

    int fd = open(data_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("open");
        return;
    }

    Treasure t = input_treasure();
    write(fd, &t, sizeof(Treasure));
    close(fd);

    log_message(hunt_id, "Added treasure");
}

void list_treasures(const char *hunt_id) {
    char data_path[TREASURE_PATH_LEN];
    snprintf(data_path, sizeof(data_path), "hunt/%s/treasures.dat", hunt_id);

    if (access(data_path, F_OK) == -1) {
        printf("No treasures found for hunt '%s'.\n", hunt_id);
        return;
    }

    struct stat file_stat;
    if (stat(data_path, &file_stat) == 0) {
        printf("Hunt: %s\n", hunt_id);
        printf("File size: %ld bytes\n", file_stat.st_size);
        printf("Last modified: %s", ctime(&file_stat.st_mtime));
    }

    int fd = open(data_path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    Treasure t;
    printf("\nList of treasures:\n");
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %d | User: %s | Location: (%f, %f) | Clue: %s | Value: %d\n",
               t.id, t.userName, t.coordinates.latitude, t.coordinates.longitude, t.clue, t.value);
    }

    close(fd);
    log_message(hunt_id, "Listed treasures");
}

void view_treasure(const char *hunt_id, int target_id) {
    char data_path[TREASURE_PATH_LEN];
    snprintf(data_path, sizeof(data_path), "hunt/%s/treasures.dat", hunt_id);

    int fd = open(data_path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.id == target_id) {
            printf("Treasure Details:\n");
            printf("ID: %d\nUser: %s\nCoordinates: (%f, %f)\nClue: %s\nValue: %d\n",
                   t.id, t.userName, t.coordinates.latitude, t.coordinates.longitude, t.clue, t.value);
            close(fd);
            log_message(hunt_id, "Viewed treasure");
            return;
        }
    }

    printf("Treasure with ID %d not found in hunt '%s'.\n", target_id, hunt_id);
    close(fd);
}
void print_menu() {
    printf("\nAvailable commands:\n");
    printf("  --add [hunt_id]\n");
    printf("  --add [hunt_id] treasure\n");
    printf("  --list [hunt_id]\n");
    printf("  --view [hunt_id] [treasure_id]\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_menu();
        return 1;
    }

    const char *command = argv[1];
    const char *hunt_id = argv[2];


    if (strcmp(command, "--add") == 0) {
        create_hunt(hunt_id);
        for (int i = 3; i < argc && strcmp(argv[i], "treasure") == 0; ++i) {
            add_treasure(hunt_id);
        }
    } else if (strcmp(command, "--list") == 0) {
        list_treasures(hunt_id);
    } else if (strcmp(command, "--view") == 0 && argc == 4) {
        int treasure_id = atoi(argv[3]);
        view_treasure(hunt_id, treasure_id);
    } else {
        printf("Invalid or unsupported command.\n");
        return 1;
    }

    return 0;
}
