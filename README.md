裡面有一個C語言的程式可以自己單獨運行: maze_game.c，如果只是想用terminal玩看看可以開。
是用鍵盤 w a s d 操控，記得切換輸入法。

其餘的都是跟C語言融合網頁運行相關的內容，詳細如下。

 app.py – Flask 後端主程式

    建立 Flask 網頁伺服器
    接收使用者指令（WASD、cheat）
    呼叫 C 程式處理迷宮邏輯
    顯示畫面、通關訊息、計時資訊

maze_game_web.c  核心 C 程式

    負責處理迷宮邏輯（地圖載入、移動、作弊跳關）
    輸出迷宮狀態至標準輸出（stdout）
    操作共享檔案 state.txt（儲存即時地圖）

maze1.txt, maze2.txt, maze3.txt  各關卡迷宮地圖

    使用 # 表示牆壁、空格表示可通行
    S 表示起點、 E 表示終點
    每一關對應不同關卡難度
    在首次進入該關時載入並複製成 state.txt


state.txt 迷宮當前狀態檔（C 與 Flask 共用）

    用來記錄玩家目前位置與地圖進度
    C 程式更新它 → Flask 顯示畫面
    Flask 只讀，不寫
    格式與迷宮地圖相同，但用 @ 表示玩家位置

templates/index.html 前端介面（Jinja2 模板）

    顯示迷宮畫面（文字地圖）
    提供按鈕控制（WASD、作弊）
    顯示計時器與通關彈窗
    使用 Jinja2 語法：{{ maze }}、{{ elapsed }}、{{ message }}

main.py Replit專用的啟動程式 (這個只有在Replit裡面)

    自動在 Replit 上執行時編譯 maze_game_web.c
    只需執行一次就產生可執行檔 maze_game_web
    程式範例：

.replit Replit 設定檔

    告訴 Replit 用哪個檔案當主程式
    指定使用 `app.py` 為執行入口
    內容範例：

build.sh（Render 用）

    僅用於 Render 平台（自動部署）
    功能：
    在 Render 部署時自動編譯 C 程式
    內容範例：

requirements.txt

    定義 Flask 所需套件供 pip 安裝
    內容：
    ```
    flask
    gunicorn  # 如果用 Render
    ```