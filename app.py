from flask import Flask, render_template, request, session
import subprocess
import os
import time

app = Flask(__name__)
app.secret_key = "anything-you-want"

MAX_LEVEL = 3

@app.route("/", methods=["GET", "POST"])
def index():
    move = None
    result = None

    # 初始化關卡與計時
    if "level" not in session:
        session["level"] = 1
    if "start_time" not in session:
        session["start_time"] = time.time()

    level = session["level"]
    map_file = f"maze{level}.txt"

    # POST：處理指令
    if request.method == "POST":
        move = request.form.get("move")
        if move in ['w', 'a', 's', 'd', 'cheat']:
            result = subprocess.run(["./maze_game_web", move, map_file], capture_output=True, text=True)
            output = result.stdout

            # ✅ 判斷是否通關（C 程式輸出包含 🎉）
            if "🎉" in output:
                elapsed = int(time.time() - session["start_time"])

                if session["level"] < MAX_LEVEL:
                    session["message"] = f"🎉 恭喜完成第 {level} 關！花費時間：{elapsed} 秒\\n是否繼續下一關？"
                    session["level"] += 1
                    level += 1
                else:
                    session["message"] = f"🎉 恭喜完成所有關卡！總用時：{elapsed} 秒\\n是否重新開始第 1 關？"
                    session["level"] = 1
                    level = 1

                session["start_time"] = time.time()
                if os.path.exists("state.txt"):
                    os.remove("state.txt")

                return render_template("index.html", maze=output, elapsed=elapsed, level=level, message=session["message"])
    else:
        if os.path.exists("state.txt"):
            os.remove("state.txt")
        session["start_time"] = time.time()
        result = subprocess.run(["./maze_game_web", map_file], capture_output=True, text=True)

    maze_output = result.stdout if result else "⚠️ 無法取得迷宮狀態"
    elapsed = int(time.time() - session["start_time"])
    message = session.pop("message", None)

    return render_template("index.html", maze=maze_output, elapsed=elapsed, level=level, message=message)

if __name__ == "__main__":
    app.run(debug=True)
