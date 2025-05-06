#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define BUF_SIZE 1024

int main() {
    int pipe1[2];
    int pipe2[2]; 
    int pipe3[2]; 

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    pid_t child1 = fork();
    if (child1 < 0) {
        perror("First fork failed");
        exit(1);
    }

    if (child1 == 0) {

        close(pipe1[1]); 
        close(pipe2[0]); 
        close(pipe3[0]);
        close(pipe3[1]); 

        char buffer[BUF_SIZE];
        ssize_t n;
        while ((n = read(pipe1[0], buffer, BUF_SIZE)) > 0) {
            for (ssize_t i = 0; i < n; i++) {
                if (islower(buffer[i])) {
                    write(pipe2[1], &buffer[i], 1);
                }
            }
        }

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }

    pid_t child2 = fork();
    if (child2 < 0) {
        perror("Second fork failed");
        exit(1);
    }

    if (child2 == 0) {
        close(pipe1[0]); 
        close(pipe1[1]); 
        close(pipe2[1]);
        close(pipe3[0]);

        FILE *fout = fopen("statistic.txt", "w");
        if (!fout) {
            perror("Cannot open statistic.txt");
            exit(1);
        }

        int freq[26] = {0};
        char ch;
        while (read(pipe2[0], &ch, 1) > 0) {
            freq[ch - 'a']++;
        }

        int distinct = 0;
        for (int i = 0; i < 26; i++) {
            if (freq[i] > 0) {
                fprintf(fout, "%c = %d\n", 'a' + i, freq[i]);
                distinct++;
            }
        }
        fclose(fout);

        write(pipe3[1], &distinct, sizeof(int));
        close(pipe2[0]);
        close(pipe3[1]);
        exit(0);
    }

    
    close(pipe1[0]); 
    close(pipe2[0]); 
    close(pipe2[1]); 
    close(pipe3[1]);

    FILE *fin = fopen("data.txt", "r");
    if (!fin) {
        perror("Cannot open data.txt");
        exit(1);
    }

    char buffer[BUF_SIZE];
    ssize_t n;
    while ((n = fread(buffer, 1, BUF_SIZE, fin)) > 0) {
        write(pipe1[1], buffer, n);
    }

    fclose(fin);
    close(pipe1[1]);

    int result;
    if (read(pipe3[0], &result, sizeof(int)) > 0) {
        printf("Nr of distinct lowercase letters = %d\n", result);
    }

    close(pipe3[0]);
    wait(NULL);
    wait(NULL);
    return 0;
}
