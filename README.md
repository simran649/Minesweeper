# 🧨 Minesweeper (SFML C++)

A fully functional graphical implementation of the classic **Minesweeper** game built using **C++ and SFML 3**.

---

## 🚀 Features

* 🎮 Interactive grid-based gameplay
* 🧠 Flood fill (auto-reveal empty cells)
* 💣 Random mine generation
* 🚩 Flagging system (right-click)
* 🔢 Number hints (adjacent mine count)
* ❌ Game Over screen with overlay
* 🏆 Win detection with UI feedback
* 🎨 Clean SFML-based graphical interface

---

## 🛠️ Tech Stack

* **Language:** C++
* **Graphics Library:** SFML 3
* **Compiler:** MinGW (g++)
* **IDE:** VS Code

---

## 📂 Project Structure

```
Minesweeper/
│
├── main.cpp        # Handles window, events, rendering
├── game.cpp        # Game logic implementation
├── game.h          # Class definitions
├── build.bat       # Compile & run script
├── *.dll           # SFML runtime libraries
└── assets/
    └── Roboto-Regular.ttf
```

---

## ⚙️ How to Run

### 1. Install SFML

Download SFML and extract it (e.g. `C:\SFML`)

---

### 2. Compile

Using terminal:

```
g++ main.cpp game.cpp -o game -I C:\SFML\include -L C:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system
```

---

### 3. Run

```
game
```

---

## 🎮 Controls

* **Left Click:** Reveal cell
* **Right Click:** Place / remove flag

---

## 📸 Screenshot

<img width="404" height="442" alt="image" src="https://github.com/user-attachments/assets/82fee2ea-5b44-4eef-8033-e0b02b1a4d26" />


---

## 🧠 Concepts Used

* Object-Oriented Programming (OOP)
* 2D rendering using SFML
* Event-driven programming
* Grid-based algorithms
* Flood fill (DFS/BFS logic)

---


## 🙌 Author

**Simran Singh**

---

## ⭐ If you like this project

Give it a star on GitHub ⭐

## Quote

Any fool can write code that a computer can understand.  Good programmers write code that humans can understand.
