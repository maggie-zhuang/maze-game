#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#define MAP_WIDTH 100
#define MAP_HEIGHT 40

char map[MAP_HEIGHT][MAP_WIDTH];
int playerX = -1, playerY = -1;
int on_exit = 0;
char current_map_file[256] = "";

void load_map_file(char *dest) {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (!d) {
        perror("無法開啟資料夾");
        exit(1);
    }

    char *maze_files[100];
    int count = 0;
    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, "maze") && strstr(dir->d_name, ".txt")) {
            maze_files[count] = strdup(dir->d_name);
            count++;
        }
    }
    closedir(d);

    if (count == 0) {
        fprintf(stderr, "找不到任何迷宮檔案！\n");
        exit(1);
    }

    srand(time(NULL));
    int idx = rand() % count;
    strcpy(dest, maze_files[idx]);

    for (int i = 0; i < count; i++) free(maze_files[i]);
}

void load_state(const char *state_file, const char *map_file) {
    FILE *fp = fopen(state_file, "r");
    if (!fp) {
        fp = fopen(map_file, "r");
        if (!fp) {
            perror("無法開啟地圖檔案");
            exit(1);
        }
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
        fgetc(fp); // 跳過換行
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
    // 若不存在記錄，抽一個地圖
    if (!fopen("state.txt", "r")) {
        load_map_file(current_map_file);
        load_state("", current_map_file);
    } else {
        load_state("state.txt", "maze1.txt"); // map_file 不用實際開啟
    }

    if (argc == 2) {
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
        } else {
            move_player(argv[1][0]);
        }
    }

done:
    save_state("state.txt");
    print_map();

    return 0;
}
