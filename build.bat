g++ -Wall -std=c++17 -c tests/test.cpp -o obj/test.o -I"src" -I"dependencies\SFML-2.6.1\include" -DSFML_STATIC
g++ -o bin/run obj/test.o -L"dependencies\SFML-2.6.1\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lwinmm -lopengl32 -lfreetype -lgdi32
@REM -mwindows
