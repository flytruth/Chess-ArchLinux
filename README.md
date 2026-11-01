# Chess

A terminal chess game for Arch Linux (tested there — it'll probably run anywhere with a C++ compiler). You play against the bot. No fancy animations, no two-player mode, just chess.

[![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)]()
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)]()
[![Tested on](https://img.shields.io/badge/tested%20on-Arch%20Linux-1793d1.svg)](https://archlinux.org/)

---

## What it is

A playable chess game written in C++. You play white (or whatever side you pick), the bot plays the other. It runs in the terminal. There are no animations — moves appear instantly. It's not designed for two human players.

The bot is simple but it plays real chess. If you want to test yourself against something that isn't completely braindead, this works.

---

## Requirements

- A C++ compiler (`g++` or `clang++`)
- Linux (tested on Arch, should work on other distros)

---

## Build and run

There's a `compile.sh` in the repo. Run it:

```bash
bash compile.sh
```

Or just compile directly:

```bash
g++ -std=c++17 -O2 -o chess main.cpp chessutils.cpp
./chess
```

---

## Files

| File | What it does |
|---|---|
| `main.cpp` | Entry point and game loop |
| `chessutils.cpp` | Move generation, board logic, bot |
| `chessutils.h` | Header for the above |
| `LRUCache.hpp` | LRU cache used somewhere in the search |
| `compile.sh` | Build script |
| `assets/` | Whatever visual assets the game uses |
| `a.out` | Prebuilt binary (probably from `compile.sh`) |

The `LRUCache.hpp.gch` file is a precompiled header — you can ignore it or delete it, your compiler will regenerate it.

---

## Limitations

- **Not two-player.** You vs. bot only.
- **No animations.** Moves land immediately.
- **Terminal only.** If you want graphics, this isn't it.
- **Tested on Arch.** It should compile on other distros but I haven't verified.

---
