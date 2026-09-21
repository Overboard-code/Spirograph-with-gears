# Spirograph-with-gears
A C++ program to draw dffirent Hypotrochoid or Epitrochoid in nice colors with animated gears. 

Uses SFML to create the pretty gears.  It must be installed: 

sudo apt install libsfml-dev build-essential

I compiled it with: 

g++ -o spirograph spiro-SFML.cpp -lsfml-graphics -lsfml-window -lsfml-system -no-pie 

I made a Windows EXE (Because I don't use Windows but people I know do:  

x86_64-w64-mingw32-g++ -o spirograph.exe spiro-SFML.cpp -DSFML_STATIC -I./SFML-2.6.2/include -L./SFML-2.6.2/lib -static -static-libgcc -static-libstdc++ -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lfreetype -lopengl32 -lwinmm -lgdi32 -luser32 -lkernel32 -luuid -mwindows 

Just run the executable to start.  The sliders control the gear sizes.  Up and down arrows control the speed.  Click on a color to start using that pen color.  Space restarts the current graph.  
<img width="1300" height="882" alt="spiro" src="https://github.com/user-attachments/assets/f657df7b-b37a-42b9-a0aa-2c38dbefc88a" />
