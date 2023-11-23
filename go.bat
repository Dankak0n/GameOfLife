@echo off
echo Start!
echo Compiling...
g++ -c main.cpp -I"SFML-2.6.1\include" -I"D:\Dankakon\WriteCode\Clion4ick\bin\mingw\include" -DSFML_STATIC
g++ main.o -o run -L"SFML-2.6.1\lib" -L"D:\Dankakon\WriteCode\Clion4ick\bin\mingw\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lwinmm -lopengl32 -lfreetype -lgdi32
@REM -mwindows
echo Compiled!
.\run
echo Finished!
del main.o