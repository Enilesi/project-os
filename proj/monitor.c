#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


volatile sig_atomic_t got_signal = 0;

void handle_signal(int sig) {

    got_signal = 1;

}

void process_command(const char *cmdline) {

    char command[256], hunt_id[128];
    int treasure_id;

    if (strcmp(cmdline, "stop_monitor") == 0) {

        printf("Received stop_monitor so exiting.\n");
        usleep(200000);
        exit(0);

    } else if (strcmp(cmdline, "list_hunts") == 0) {

        printf("List of hunts:\n");
        system("ls -1 hunt 2>/dev/null");

    } else if (sscanf(cmdline, "list_treasures %127s", hunt_id) == 1) {

        printf("List of treasures for hunt '%s':\n", hunt_id);
        char cmd[300];
        snprintf(cmd, sizeof(cmd), "./treasure_manager --list %s", hunt_id);
        system(cmd);

    } else if (sscanf(cmdline, "view_treasure %127s %d", hunt_id, &treasure_id) == 2) {

        printf("View treasure %d in hunt '%s':\n", treasure_id, hunt_id);
        char cmd[300];
        snprintf(cmd, sizeof(cmd), "./treasure_manager --view %s %d", hunt_id, treasure_id);
        system(cmd);

    } else {

        printf("Unknown command: %s\n", cmdline);
        
    }

     printf("\nGive new command:\nTH > ");
    fflush(stdout);


}

int main() {
    
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    while (1) {

        pause();
        if (got_signal) {
            
            got_signal = 0;

            FILE *f = fopen("commands.txt", "r");
            if (f) {
                char line[256];
                if (fgets(line, sizeof(line), f)) {
                    line[strcspn(line, "\n")] = '\0';
                    process_command(line);
                }
                fclose(f);
            }
        }
    }

    return 0;
}
