#�����ʽ����ű���������AStyle.exe�ļ���
#��AStyle.exe�ļ�����ͬһĿ¼�����ɶԵ�ǰĿ¼�°���������Ŀ¼�е��ļ����и�ʽ����
#Ŀǰ����.cpp��.c�ļ����и�ʽ��������������ļ����и�ʽ�������������

for /R %%f in (*.cpp; *.c) do astyle --A1 -Sm0pHUoM100jwbcfQ %%f

for /R %%f in (*.orig) do erase %%f

#pause
