@echo off
:start
cls
del "gameoflife.exe" >nul 2>&1
del "glider_output.txt" >nul 2>&1
echo Compiling...
start /b /w gcc -Wall -Wextra -pedantic -std=c11 gameoflife.c gol.c gol.h -o gameoflife.exe
if %errorlevel% == 0 (
    gameoflife.exe -s -t -g 5 -i test.txt
)
pause
goto start