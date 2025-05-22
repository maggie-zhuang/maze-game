#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // Windows 專用，Linux 請改 termios
#include <time.h>
#include <stdbool.h>
#include <unistd.h> // 用 sleep()（Linux: usleep; Windows 可用 Sleep 但需 windows.h）

#define MAP_WIDTH 100
#define MAP_HEIGHT 40
#define MAX_LEVEL 3

char **map;
int playerX, playerY;

void allocate_map() {
    map = malloc(sizeof(char *) * MAP_HEIGHT);
    for (int i = 0; i < MAP_HEIGHT; i++) {
        map[i] = malloc(sizeof(char) * MAP_WIDTH);
    }
}

void free_map() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        free(map[i]);
    }
    free(map);
}

void load_map(const char *filename) {
    allocate_map();
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("地圖檔案載入失敗");
        exit(1);
    }

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            fscanf(fp, "%c", &map[i][j]);
            if (map[i][j] == 'S') {
                playerX = i;
                playerY = j;
            }
        }
        fgetc(fp); // 跳過換行
    }
    fclose(fp);
}

void print_map(int level) {
    system("cls"); // Linux 用 "clear"
    printf("【迷宮遊戲 - 關卡 %d】\n", level + 1);

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (i == playerX && j == playerY)
                printf("@");
            else
                printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

int is_valid_move(int x, int y) {
    return x >= 0 && x < MAP_HEIGHT && y >= 0 && y < MAP_WIDTH && map[x][y] != '#';
}

int play_level(int level) {
    bool cheat_mode = false;

    printf("是否要開啟第 %d 關的作弊模式？(Y/N)：", level + 1);
    char cm;
    do {
        cm = getchar();
    } while (cm == '\n');
    if (cm == 'y' || cm == 'Y') {
        cheat_mode = true;
    }

    if (cheat_mode) {
        printf("🛠️ 作弊模式啟動中，跳過第 %d 關...\n", level + 1);
        time_t start_time = time(NULL);
        sleep(1); // 測試可改短
        time_t end_time = time(NULL);
        printf("🎉 (作弊模式) 完成第 %d 關！花費時間：%.0f 秒\n", level + 1, difftime(end_time, start_time));
        _getch();
        return 1;
    }

    // 🟢 正常模式開始
    char filename[20];
    sprintf(filename, "maze%d.txt", level + 1);
    load_map(filename);

    time_t start_time = time(NULL);

    while (1) {
        print_map(level);
        printf("WASD 控制移動，Q 離開\n");

        char move = _getch();

        int newX = playerX;
        int newY = playerY;

        if (move == 'q' || move == 'Q') {
            printf("你選擇退出遊戲！\n");
            exit(0);
        }
        if (move == 'w' || move == 'W') newX--;
        if (move == 's' || move == 'S') newX++;
        if (move == 'a' || move == 'A') newY--;
        if (move == 'd' || move == 'D') newY++;

        if (is_valid_move(newX, newY)) {
            playerX = newX;
            playerY = newY;
        }

        if (map[playerX][playerY] == 'E') {
            time_t end_time = time(NULL);
            double elapsed = difftime(end_time, start_time);
            print_map(level);
            printf("🎉 恭喜完成第 %d 關！花費時間：%.0f 秒\n", level + 1, elapsed);
            _getch();
            break;
        }
    }

    free_map();
    return 1;
}



int main() {
    for (int level = 0; level < MAX_LEVEL; level++) {
        int result = play_level(level);

        printf("是否繼續下一關？(Y/N)：");
        char choice;
        do {
            choice = getchar();
        } while (choice == '\n');

        if (choice != 'y' && choice != 'Y') {
            printf("遊戲結束，感謝遊玩！\n");
            break;
        }
    }

    printf("🏁 你已完成所有關卡或選擇結束遊戲。\n");
    return 0;
}


