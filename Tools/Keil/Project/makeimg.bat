@rem ����0: exe
@rem ����1: ����bin�ļ� ,ԭʼ�ļ�����ѹ�����ļ�
@rem ����2 :����ļ� 
@rem ����3:  �����ļ�����,0��image�ļ���1��FLASHBOOT�ļ� 2��secboot�ļ�
@rem ����4: �Ƿ�ѹ���ļ���0��plain�ļ���1��ѹ�������ļ�
@rem ����5: �汾���ļ���ִ����exe֮��汾�Ż��Զ��ۼ�
@rem ����6�����λ��
@rem ����7��ԭʼbin�ļ�
@rem ����8��ѹ���ļ��Ľ�ѹλ��

@echo off

copy WM_W600.map ..\..\..\Bin
copy objs\WM_W600.bin ..\..\..\Bin
cd ..\..\..\Tools

copy ..\Bin\version.txt ..\Bin\version_bk.txt

makeimg.exe "..\Bin\WM_W600.bin" "..\Bin\WM_W600.img" 0 0 "..\Bin\version.txt" E000
makeimg.exe "..\Bin\WM_W600.bin" "..\Bin\WM_W600_SEC.img" 0 0 "..\Bin\version.txt" 7E800
makeimg_all.exe "..\Bin\secboot.img" "..\Bin\WM_W600.img" "..\Bin\WM_W600.FLS"
@del "..\Bin\WM_W600.img"
copy out.img ..\Bin\WM_W600.FLS



