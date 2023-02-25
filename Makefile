


run:
	IF NOT EXIST "%CD%\bin" mkdir "bin"
	copy "*.dll" "bin\."
	IF NOT EXIST "%CD%\bin\assets" mkdir "bin\assets"
	copy "assets\*.*" "bin\assets\."
	g++ -I src\include -L src\lib -o bin\ball.exe ball.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -static-libgcc -static-libstdc++ -mwindows


debug:
	IF NOT EXIST "%CD%\bin" mkdir "bin"
	copy "*.dll" "bin\."
	IF NOT EXIST "%CD%\bin\assets" mkdir "bin\assets"
	copy "assets\*.*" "bin\assets\."
	g++ -I src\include -L src\lib -o bin\ball.exe ball.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -static-libgcc -static-libstdc++

clean:
	del icon.res