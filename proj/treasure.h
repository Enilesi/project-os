
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
