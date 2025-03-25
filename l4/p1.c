#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//gcc -o program problem1.c
//./program input.txt output.txt e


int main(int argc, char *argv[]) {
    if (argc != 4) {
        const char *msg = "Usage: ./program <input_filepath> <output_filepath> <character>\n";
        write(2, msg, strlen(msg));
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];
    char target = argv[3][0];

    int input_fd = open(input_path, O_RDONLY);
    if (input_fd < 0) {
        perror("Error opening input file");
        return 1;
    }

    int output_fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd < 0) {
        perror("Error opening/creating output file");
        close(input_fd);
        return 1;
    }

    char buffer[1024];
    ssize_t bytes_read;

    int lowercase = 0, uppercase = 0, digits = 0, target_count = 0;

    while ((bytes_read = read(input_fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; ++i) {
            char c = buffer[i];
            if (c >= 'a' && c <= 'z') lowercase++;
            else if (c >= 'A' && c <= 'Z') uppercase++;
            else if (c >= '0' && c <= '9') digits++;
            if (c == target) target_count++;
        }
    }

    struct stat st;
    if (stat(input_path, &st) < 0) {
        perror("Error using stat()");
        close(input_fd);
        close(output_fd);
        return 1;
    }

    char output[256];
    int len = snprintf(output, sizeof(output),
        "number of lowercase letters: %d\n"
        "number of uppercase letters: %d\n"
        "number of digits: %d\n"
        "number of character occurrences: %d\n"
        "file size: %ld\n",
        lowercase, uppercase, digits, target_count, st.st_size);

    write(output_fd, output, len);

    close(input_fd);
    close(output_fd);
    return 0;
}


