# ✅ Flask app.py（支援隨機地圖）
from flask import Flask, render_template, request, session
import subprocess
import os
import time
import random

app = Flask(__name__)
app.secret_key = "anything-you-want"

@app.route("/", methods=["GET", "POST"])
def index():
    move = None
    result = None

    # 初始化地圖與時間
    if "start_time" not in session:
        session["start_time"] = time.time()
    if "map_file" not in session:
        maze_files = [f for f in os.listdir('.') if f.startswith('maze') and f.endswith('.txt')]
        session["map_file"] = random.choice(maze_files) if maze_files else "maze1.txt"

    map_file = session["map_file"]

    # 處理指令
    if request.method == "POST":
        move = request.form.get("move")
        if move in ['w', 'a', 's', 'd', 'cheat']:
            result = subprocess.run(["./maze_game_web", move, map_file], capture_output=True, text=True)
            output = result.stdout

            if "🎉" in output:  # 完成訊息含 🎉
                elapsed = int(time.time() - session["start_time"])
                session["message"] = f"🎉 你完成迷宮！花費時間：{elapsed} 秒\n是否重新挑戰另一張隨機迷宮？"
                session.pop("map_file", None)
                session["start_time"] = time.time()
                if os.path.exists("state.txt"):
                    os.remove("state.txt")
                return render_template("index.html", maze=output, elapsed=elapsed, message=session["message"])
    else:
        if os.path.exists("state.txt"):
            os.remove("state.txt")
        session["start_time"] = time.time()
        result = subprocess.run(["./maze_game_web", map_file], capture_output=True, text=True)

    maze_output = result.stdout if result else "⚠️ 無法取得迷宮狀態"
    elapsed = int(time.time() - session["start_time"])
    message = session.pop("message", None)

    return render_template("index.html", maze=maze_output, elapsed=elapsed, message=message)

if __name__ == "__main__":
    app.run(debug=True)
