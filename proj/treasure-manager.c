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

typedef struct Gps {
    float latitude;
    float longitude;
} Gps;

typedef struct Treasure {
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

#define TREASURE_PATH_LEN 256

void log_action(const char *hunt_id, const char *action) {
    char log_path[TREASURE_PATH_LEN];
    struct stat st = {0};

    snprintf(log_path, sizeof(log_path), "hunt/%s/treasures.log", hunt_id);
    int fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd >= 0) {
        time_t now = time(NULL);
        dprintf(fd, "[%s] %s\n", strtok(ctime(&now), "\n"), action);
        close(fd);

        if (stat("log", &st) == -1) {
            mkdir("log", 0700);
        }

        char symlink_path[TREASURE_PATH_LEN];
        snprintf(symlink_path, sizeof(symlink_path), "log/logged_hunt-%s", hunt_id);
        unlink(symlink_path);
        symlink(log_path, symlink_path);
    }
}

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

    log_action(hunt_id, "Created hunt");
}

Treasure take_treasure() {
    Treasure t;
    printf("Enter Treasure ID: ");
    scanf("%d", &t.id);
    printf("Enter User Name: ");
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
    char file_path[TREASURE_PATH_LEN];
    snprintf(file_path, sizeof(file_path), "hunt/%s/treasures.dat", hunt_id);

    int fd = open(file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("open");
        return;
    }

    Treasure t = take_treasure();
    printf("\n");
    write(fd, &t, sizeof(Treasure));
    close(fd);

    log_action(hunt_id, "Added treasure");
}

void list_treasures(const char *hunt_id) {
    char path[TREASURE_PATH_LEN];
    snprintf(path, sizeof(path), "hunt/%s/treasures.dat", hunt_id);

    struct stat st;
    if (stat(path, &st) == -1) {
        printf("No treasures found for hunt '%s'.\n", hunt_id);
        return;
    }

    printf("Hunt: %s\n", hunt_id);
    printf("File size: %ld bytes\n", st.st_size);
    printf("Last modified: %s", ctime(&st.st_mtime));

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %d | User: %s | Location: (%f, %f) | Clue: %s | Value: %d\n",
               t.id, t.userName, t.coordinates.latitude, t.coordinates.longitude, t.clue, t.value);
    }

    close(fd);
    log_action(hunt_id, "Listed treasures");
}

void view_treasure(const char *hunt_id, int id) {
    char path[TREASURE_PATH_LEN];
    snprintf(path, sizeof(path), "hunt/%s/treasures.dat", hunt_id);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.id == id) {
            printf("Treasure ID: %d\n", t.id);
            printf("User Name: %s\n", t.userName);
            printf("Coordinates: (%f, %f)\n", t.coordinates.latitude, t.coordinates.longitude);
            printf("Clue: %s\n", t.clue);
            printf("Value: %d\n", t.value);
            close(fd);
            log_action(hunt_id, "Viewed treasure");
            return;
        }
    }

    printf("Treasure with ID %d not found in hunt '%s'.\n", id, hunt_id);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("  --add <hunt_id> [treasure]*\n");
        printf("  --list <hunt_id>\n");
        printf("  --view <hunt_id> <treasure_id>\n");
        return 1;
    }

    const char *cmd = argv[1];
    const char *hunt_id = argv[2];

    if (strcmp(cmd, "--add") == 0) {
        add_hunt(hunt_id);

        int i=3;

        while (i < argc && strcmp(argv[i], "treasure") == 0) {
            add_treasure(hunt_id);
            i++;
        }        

    } else if (strcmp(cmd, "--list") == 0) {
        list_treasures(hunt_id);
    } else if (strcmp(cmd, "--view") == 0 && argc == 4) {
        view_treasure(hunt_id, atoi(argv[3]));
    } else {
        printf("Invalid or unsupported command.\n");
        return 1;
    }

    return 0;
}
