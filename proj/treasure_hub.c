#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>



#define READ 0
#define WRITE 1

int pipes[2] = {-1,-1};


volatile sig_atomic_t monitor_running = 0;
pid_t monitor_pid = -1;


typedef enum {

    COMMAND_START_MONITOR,
    COMMAND_STOP_MONITOR,
    COMMAND_EXIT,
    COMMAND_LIST_HUNTS,
    COMMAND_LIST_TREASURES,
    COMMAND_VIEW_TREASURE,
    COMMAND_HELP,
    COMMAND_CALCULATE_SCORE,
    COMMAND_UNKNOWN

} CommandName;


void print_prompt() {

    usleep(30000);
    printf("\nGive new command:\nTH > ");
    usleep(20000);
    fflush(stdout);

}


static void print_menu() {

    puts("\nAvailable commands:");
    puts("  start_monitor");
    puts("  list_hunts");
    puts("  list_treasures <hunt>");
    puts("  view_treasure <hunt> <id>");
    puts("  stop_monitor");
    puts("  calculate_score");
    puts("  \033[1;33mhelp\033[0m"); 
    puts("  exit");

}

CommandName get_command(const char* command) {

    if (strcmp(command, "start_monitor") == 0) return COMMAND_START_MONITOR;
    if (strcmp(command, "stop_monitor") == 0) return COMMAND_STOP_MONITOR;
    if (strcmp(command, "exit") == 0) return COMMAND_EXIT;
    if (strncmp(command, "list_hunts", 10) == 0) return COMMAND_LIST_HUNTS;
    if (strncmp(command, "list_treasures", 14) == 0) return COMMAND_LIST_TREASURES;
    if (strncmp(command, "view_treasure", 13) == 0) return COMMAND_VIEW_TREASURE;
    if (strcmp(command, "help") == 0) return COMMAND_HELP;
    if (strcmp(command, "calculate_score") == 0) return COMMAND_CALCULATE_SCORE;
    return COMMAND_UNKNOWN;

}



void clean_output(){

    if(pipes[READ]<0){
        return;
    }

    char buffer[128];

    int n;

    while((n= read(pipes[READ], buffer, sizeof(buffer)))>0){
        write(STDOUT_FILENO,buffer,n);
    }

}



void stop_monitor_handler(int sig) {

    int saved_errno = errno;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        if (pid == monitor_pid) {
            monitor_running = 0;
            printf("\nMonitor process exited.\n");
            print_prompt();
            fflush(stdout);
        }
    }
    errno = saved_errno;

}

void send_command_to_monitor(const char *command_line) {

    int fd = open("commands.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        dprintf(fd, "%s\n", command_line);
        close(fd);
        kill(monitor_pid, SIGUSR1);
    } else {
        perror("open commands.txt");
    }
    usleep(50000);
    clean_output();

}


void start_monitor() {

    
    if(pipe(pipes)==-1){
        perror("Pipe failed");
        return;
    }


    if (monitor_running) {
        printf("Monitor already running.\n");
        return;
    }

    monitor_pid = fork();

    if (monitor_pid < 0) {
        perror("fork");
        exit(1);

    } else if (monitor_pid == 0) {

        close(pipes[READ]);
        dup2(pipes[WRITE],STDERR_FILENO);
        dup2(pipes[WRITE],STDOUT_FILENO);


        execl("./monitor", "./monitor", NULL);
        perror("execl failed");
        exit(1);

    } else {
        monitor_running = 1;
        
        close(pipes[WRITE]);
        int flags=fcntl(pipes[READ],F_GETFL,0);
        fcntl(pipes[READ],F_SETFL,flags|O_NONBLOCK);

        printf("Monitor started with PID %d\n", monitor_pid);
       print_prompt();

        clean_output(pipes);
        


    }
}


void stop_monitor() {

    if (!monitor_running) {
        printf("No monitor is running.\n");
        return;
    }
    send_command_to_monitor("stop_monitor");

}

void calculate_scores_for_all_hunts() {

    FILE *fp = popen("ls -1 hunt 2>/dev/null", "r");
    if (!fp) {

        perror("popen");
        return;

    }

    char hunt_id[128];

    while (fgets(hunt_id, sizeof(hunt_id), fp)) {

        hunt_id[strcspn(hunt_id, "\n")] = '\0';

        int fd[2];
        if (pipe(fd) == -1) {

            perror("pipe");
            continue;

        }

        pid_t pid = fork();
        if (pid == 0) {

            close(fd[READ]);
            dup2(fd[WRITE], STDOUT_FILENO);
            dup2(fd[WRITE], STDERR_FILENO);
            execl("./p.exe", "./p.exe", hunt_id, NULL);
            perror("execl");
            exit(1);

        } else if (pid > 0) {

            close(fd[WRITE]);
            char buf[256];
            ssize_t n;
            printf("\n");

            while ((n = read(fd[READ], buf, sizeof(buf))) > 0) {

                write(STDOUT_FILENO, buf, n);

            }

            close(fd[READ]);
            waitpid(pid, NULL, 0);

        } else {

            perror("fork");
            
        }
    }

    pclose(fp);

}


int main() {

    struct sigaction sa;
    sa.sa_handler = stop_monitor_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGCHLD, &sa, NULL) < 0) {

        perror("sigaction");
        exit(1);

    }

    print_menu();

    char command[256];
    print_prompt();

    while (1) {

        if (!fgets(command, sizeof(command), stdin))
            break;

        command[strcspn(command, "\n")] = '\0';
        CommandName command_name = get_command(command);

        switch (command_name) {
            case COMMAND_START_MONITOR:
                start_monitor();
                break;

            case COMMAND_STOP_MONITOR:
                stop_monitor();
                break;

            case COMMAND_EXIT:
                if (monitor_running) {

                    printf("Error: Monitor still running. Use stop_monitor first.\n");
                    print_prompt();

                } else {

                    printf("Exited.\n");
                    return 0;

                }
                break;

            case COMMAND_LIST_HUNTS:
            case COMMAND_LIST_TREASURES:
            case COMMAND_VIEW_TREASURE:
                if (!monitor_running) {

                    printf("Error: Monitor is not running.\n");
                    print_prompt();

                } else {

                    send_command_to_monitor(command);

                }
                break;

            case COMMAND_HELP:

                print_menu();
                print_prompt();
                break;

            case COMMAND_CALCULATE_SCORE:

                calculate_scores_for_all_hunts();
                print_prompt();
                break;

            default:

                printf("Unknown or unsupported command.\n");
                print_prompt();
                break;
        }
    }

    return 0;

}