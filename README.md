# Minesweeper

A desktop implementation of the classic Minesweeper puzzle game,
developed in **C++** using the **SFML (Simple and Fast Multimedia
Library)** framework.

The objective is to reveal every safe cell on the board without
triggering a hidden mine. Numbered cells indicate how many mines are
present in the eight surrounding positions.

## Features

-   Multiple difficulty presets:
    -   **Easy:** 9 × 9 grid with 10 mines
    -   **Medium:** 16 × 16 grid with 40 mines
    -   **Hard:** 30 × 16 grid with 99 mines
-   Custom board configuration
-   Random mine placement
-   Adjacent-mine number calculation
-   Cell revealing and flag placement
-   Automatic revealing of connected empty cells
-   Win and loss detection
-   Timer and mine/flag counters
-   Pause, resume, and restart options
-   Leaderboard with locally stored records
-   Settings and visual/audio feedback
-   SFML-based graphical interface

> Feature availability may depend on the version of the project
> currently checked out. Verify the final build before presenting or
> submitting.

## Tech Stack

-   **Language:** C++
-   **Graphics and windowing:** SFML
-   **Build tools:** g++, pkg-config
-   **Build standard:** C++17

## Requirements

On Ubuntu/Debian-based Linux distributions, install the compiler, build
tools, and SFML development packages:

``` bash
sudo apt update
sudo apt install build-essential pkg-config libsfml-dev
```

Check that the required tools are available:

``` bash
g++ --version
pkg-config --modversion sfml-graphics
```

## Build and Run

1.  Clone the repository:

    ``` bash
    git clone https://github.com/simran649/Minesweeper.git
    cd Minesweeper
    ```

2.  Compile the source files:

    ``` bash
    g++ -std=c++17 main.cpp game.cpp audio.cpp -o game \
    $(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system sfml-audio)
    ```

3.  Run the game:

    ``` bash
    ./game
    ```

If your source files or filenames differ, update the compile command
accordingly. The command above assumes `main.cpp`, `game.cpp`, and
`audio.cpp` are in the project root.

## Controls

  Action                   Control
  ------------------------ ---------------------------------------
  Reveal a cell            Left mouse button
  Place or remove a flag   Right mouse button
  Navigate menus           Mouse
  Pause or resume          Use the in-game control, if available

## Difficulty Levels

  Difficulty          Board size   Number of mines
  ------------ ----------------- -----------------
  Easy                     9 × 9                10
  Medium                 16 × 16                40
  Hard                   30 × 16                99
  Custom         User-configured   User-configured

## How to Play

1.  Start the game and choose a difficulty.
2.  Select a cell to reveal it.
3.  A number on a revealed cell tells you how many mines are in its
    surrounding eight cells.
4.  Use right-click to flag a cell you believe contains a mine.
5.  Continue revealing safe cells and using the number clues to identify
    mines.
6.  Win by revealing all safe cells. Revealing a mine ends the game.

## Core Concepts and Algorithms

-   **Grid representation:** Stores the state and value of each board
    cell.
-   **Random mine placement:** Distributes mines across valid board
    positions.
-   **Neighbour checking:** Examines up to eight surrounding cells to
    calculate the adjacent-mine count.
-   **Flood fill / recursive reveal:** Opens connected empty cells and
    their relevant numbered boundaries.
-   **Game-state management:** Tracks the current state of the game and
    handles transitions such as active, paused, won, and lost.
-   **File handling:** Stores leaderboard information locally.

## Project Structure

The following is the expected source-file layout based on the build
command. Your actual repository may contain additional files and
folders.

``` text
Minesweeper/
├── main.cpp
├── game.cpp
├── audio.cpp
├── game.h
├── audio.h
├── assets/
│   └── audio/
│       └── sfx/
│           ├── button_hover.wav
│           ├── button_click.wav
│           └── tile_reveal.wav
└── leaderboard.txt   # May be created by the application
```

Keep the asset paths used by the source code intact. If audio files are
missing, check the paths expected by the audio implementation.

## Troubleshooting

### SFML headers or libraries are not found

Install the SFML development package and confirm `pkg-config` can locate
it:

``` bash
sudo apt install libsfml-dev pkg-config
pkg-config --cflags --libs sfml-graphics sfml-window sfml-system sfml-audio
```

### The compiler reports a C++ standard error

Compile with C++17:

``` bash
-std=c++17
```

### Audio does not play

-   Confirm the audio files exist.
-   Check that their paths match the paths used in the source code.
-   Confirm the SFML audio library is installed and linked.

### The application cannot find assets

Run the executable from the project directory so relative asset paths
resolve as expected.

## Author
**Simran Singh**
## Updated by
**Vitthal Singh**

## Academic Project

Developed as a team project for the **BCA** programme.

## Future Improvements

-   Online multiplayer support
-   Additional board themes and customization
-   More detailed player statistics
-   Improved accessibility options
-   Additional game modes

## License

No license has been specified for this repository. Add a `LICENSE` file
before allowing others to reuse or redistribute the project.
