chcp 65001 
md build  
copy *.* .\build
cd .\build
md include
md lib
cd ..
copy .\include\*.* .\build\include
cd .\build\include
md sec_api
md winapi
cd ..
cd ..
copy .\include\sec_api\*.* .\build\include\sec_api
copy .\include\winapi\*.* .\build\include\winapi
copy .\lib\*.* .\build\lib
cd .\build
del build.bat
del 自制汇编图形虚拟机 完整文档.md
gcc main.c build.c -o AsmGfx.exe -luser32 -lgdi32
del build.c | del main.c | del pack.h
echo OK