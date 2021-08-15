#代码格式整理脚本，需引用AStyle.exe文件。
#与AStyle.exe文件放在同一目录，即可对当前目录下包含所有子目录中的文件进行格式整理
#目前仅对.cpp和.c文件进行格式整理，如需对其他文件进行格式整理，请自行添加

for /R %%f in (*.cpp; *.c) do astyle --A1 -Sm0pHUoM100jwbcfQ %%f

for /R %%f in (*.orig) do erase %%f

#pause
