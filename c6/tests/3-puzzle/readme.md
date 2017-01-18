#3-puzzle game

## Introduction

A 3-puzzle game starts like this:

```
+---+---+
| 3 | 1 |
+---+---+
|   | 2 |
+---+---+
```

A slice can be swapped with the empty slice if it has a common edge with the empty slice.
What you need to do is to make it to the final state:

```
+---+---+
| 1 | 2 |
+---+---+
| 3 |   |
+---+---+
```

## How to play

1. run `make` to compile the gui program. You should have the framework `OpenGL` and `GLUT` installed on your computer. (Only tested on Mac OS X 10.11)

2. run `../../nas 3-puzzle.as | ./3-puzzlegui`, and a graphic game board will be shown. Input a lucky number, and the initial state will be generated and is guaranteed to be accessible to the goal. Input the number of the slice you want to move (or '?' if you want the program to automatically make the best move for you) in the terminal/console, press enter, and the operation will be done on the board. When you reach the goal, a message will be shown, and you can enter the character 'E' to exit or 'A' to play again.

## Features

It has something like an AI, helping the player to make a move. However, due to the small size of the state space of this problem, actually we do not need to search for the best move. The best moves are saved in a table instead.
