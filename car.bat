@echo off
gcc -Wall -pedantic -std=c11 gameoflife.c gol.c gol.h -o gameoflife.exe
if errorlevel 0 (
    gameoflife.exe
)