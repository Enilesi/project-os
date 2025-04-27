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

void sigchld_handler(int sig) {
    int saved_errno = errno;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        if (pid == monitor_pid) {
            monitor_running = 0;
            printf("\nMonitor process exited.\n");
        }
    }
    errno = saved_errno;
}

void start_monitor() {
    if (monitor_running) {
        printf("Monitor already running.\n");
        return;
    }

    monitor_pid = fork();
    if (monitor_pid < 0) {
        perror("fork");
        exit(1);
    } else if (monitor_pid == 0) {
        printf("[Monitor] Process started with PID %d\n", getpid());

        struct sigaction sa;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);


        while (1) {
            pause(); 
        }
        exit(0);
    } else {
        monitor_running = 1;
    }
}

void stop_monitor() {
    if (!monitor_running) {
        printf("No monitor is running.\n");
        return;
    }
    printf("Sending SIGTERM to monitor (PID %d)...\n", monitor_pid);
    kill(monitor_pid, SIGTERM);
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
                printf("Error: Monitor still running. Use stop_monitor first.\n");
            } else {
                printf("Exited\n");
                break;
            }
        } else {
            if (!monitor_running) {
                printf("Error: No monitor process is running.\n");
            } else {
                printf("Unknown or incomplete command. (implement list_hunts etc later)\n");
            }
        }
    }

    return 0;
}
