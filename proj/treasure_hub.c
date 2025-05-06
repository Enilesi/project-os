#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

volatile sig_atomic_t monitor_running = 0;
pid_t monitor_pid = -1;

static void print_menu() {
    puts("Available commands:");
    puts("  start_monitor");
    puts("  list_hunts");
    puts("  list_treasures <hunt>");
    puts("  view_treasure <hunt> <id>");
    puts("  stop_monitor");
    puts("  help");
    puts("  exit");
}

void sigchld_handler(int sig) {
    int saved_errno = errno;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        if (pid == monitor_pid) {
            monitor_running = 0;
            printf("\n[Hub] Monitor process exited.\n");
        }
    }
    errno = saved_errno;
}

void send_command_to_monitor(const char *command_line) {
    int fd = open("cmd.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        dprintf(fd, "%s\n", command_line);
        close(fd);
        kill(monitor_pid, SIGUSR1);
    } else {
        perror("[Hub] open cmd.txt");
    }
}

void start_monitor() {
    if (monitor_running) {
        printf("[Hub] Monitor already running.\n");
        return;
    }

    monitor_pid = fork();
    if (monitor_pid < 0) {
        perror("[Hub] fork");
        exit(1);
    } else if (monitor_pid == 0) {
        execl("./monitor", "./monitor", NULL);
        perror("[Hub] execl failed");
        exit(1);
    } else {
        monitor_running = 1;
        printf("[Hub] Monitor started with PID %d\n", monitor_pid);
    }
}

void stop_monitor() {
    if (!monitor_running) {
        printf("[Hub] No monitor is running.\n");
        return;
    }
    send_command_to_monitor("stop_monitor");
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }

    print_menu();

    char command[256];

    while (1) {
        printf("\n> ");
        fflush(stdout);

        if (!fgets(command, sizeof(command), stdin))
            break;

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "start_monitor") == 0) {

            start_monitor();

        } else if (strcmp(command, "stop_monitor") == 0) {

            stop_monitor();

        } else if (strcmp(command, "exit") == 0) {

            if (monitor_running) {
                printf("[Hub] Error: Monitor still running. Use stop_monitor first.\n");
            } else {
                printf("[Hub] Exited.\n");
                break;
            }

        } else if (strncmp(command, "list_hunts", 10) == 0 ||  strncmp(command, "list_treasures", 14) == 0 ||
                   strncmp(command, "view_treasure", 13) == 0) {

            if (!monitor_running) {
                printf("[Hub] Error: Monitor is not running.\n");

            } else {

                send_command_to_monitor(command);

            }

        } else {
            printf("[Hub] Unknown or unsupported command.\n");
        }
    }

    return 0;
}
