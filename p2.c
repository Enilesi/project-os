#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Convert octal string to decimal
int octal_to_decimal(const char *str, int size) {
    int result = 0;
    for (int i = 0; i < size && str[i] >= '0' && str[i] <= '7'; i++) {
        result = result * 8 + (str[i] - '0');
    }
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        const char *msg = "Usage: ./program <tar_file_path>\n";
        write(2, msg, strlen(msg));
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error opening tar file");
        return 1;
    }

    char header[512];
    ssize_t bytes_read = read(fd, header, 512);
    if (bytes_read != 512) {
        perror("Error reading tar header");
        close(fd);
        return 1;
    }

    char file_name[101], owner_id[9], group_id[9], size_str[13];
    memcpy(file_name, header, 100); file_name[100] = '\0';
    memcpy(owner_id, header + 108, 8); owner_id[8] = '\0';
    memcpy(group_id, header + 116, 8); group_id[8] = '\0';
    memcpy(size_str, header + 124, 12); size_str[12] = '\0';

    int file_size = octal_to_decimal(size_str, 12);
    int uid = octal_to_decimal(owner_id, 8);
    int gid = octal_to_decimal(group_id, 8);

    char output[256];
    int len = snprintf(output, sizeof(output),
        "File name : %s\n"
        "File size : %d\n"
        "Owner id : %d\n"
        "Group id : %d\n",
        file_name, file_size, uid, gid);

    write(1, output, len);
    close(fd);
    return 0;
}
