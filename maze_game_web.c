#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_WIDTH 100
#define MAP_HEIGHT 40

char map[MAP_HEIGHT][MAP_WIDTH];
int playerX = -1, playerY = -1;
int on_exit = 0;

int load_state(const char *state_file) {
    FILE *fp = fopen(state_file, "r");
    if (!fp) return 0;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            char ch = fgetc(fp);
            map[i][j] = ch;
            if (ch == '@') {
                playerX = i;
                playerY = j;
            }
        }
        fgetc(fp);
    }
    fclose(fp);
    return 1;
}

void load_map_file(const char *map_file) {
    FILE *fp = fopen(map_file, "r");
    if (!fp) {
        perror("無法開啟迷宮地圖檔案");
        exit(1);
    }

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            char ch = fgetc(fp);
            map[i][j] = ch;
            if (ch == '@') {
                playerX = i;
                playerY = j;
            }
            if (ch == 'S' && playerX == -1) {
                playerX = i;
                playerY = j;
                map[i][j] = '@';
            }
        }
        fgetc(fp);
    }
    fclose(fp);
}

void save_state(const char *filename) {
    FILE *fp = fopen(filename, "w");
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            fputc(map[i][j], fp);
        }
        fputc('\n', fp);
    }
    fclose(fp);
}

int is_valid(int x, int y) {
    return x >= 0 && x < MAP_HEIGHT && y >= 0 && y < MAP_WIDTH && map[x][y] != '#';
}

void move_player(char dir) {
    int newX = playerX, newY = playerY;
    if (dir == 'w') newX--;
    if (dir == 's') newX++;
    if (dir == 'a') newY--;
    if (dir == 'd') newY++;

    if (is_valid(newX, newY)) {
        if (map[playerX][playerY] == '@')
            map[playerX][playerY] = ' ';
        playerX = newX;
        playerY = newY;
        map[playerX][playerY] = '@';
    }
}

void print_map() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            putchar(map[i][j]);
        }
        putchar('\n');
    }

    if (on_exit || map[playerX][playerY] == 'E') {
        printf("🎉 恭喜你抵達終點！本關完成。\n");
    }
}

int main(int argc, char *argv[]) {
    const char *state_file = "state.txt";
    const char *map_file = "maze1.txt"; // 預設地圖檔

    if (argc >= 2 && argv[1][0] != 'w' && argv[1][0] != 'a' && argv[1][0] != 's' && argv[1][0] != 'd' && strcmp(argv[1], "cheat") != 0) {
        map_file = argv[1];
    }
    if (argc >= 3) {
        map_file = argv[2];
    }

    if (!load_state(state_file)) {
        load_map_file(map_file);
    }

    if (argc >= 2) {
        if (strcmp(argv[1], "cheat") == 0) {
            for (int i = 0; i < MAP_HEIGHT; i++) {
                for (int j = 0; j < MAP_WIDTH; j++) {
                    if (map[i][j] == 'E') {
                        map[playerX][playerY] = ' ';
                        playerX = i;
                        playerY = j;
                        on_exit = 1;
                        map[playerX][playerY] = '@';
                        printf("🎉 你已完成本關（作弊）！\n");
                        goto done;
                    }
                }
            }
        } else if (strlen(argv[1]) == 1) {
            move_player(argv[1][0]);
        }
    }

done:
    save_state(state_file);
    print_map();
    return 0;
}
