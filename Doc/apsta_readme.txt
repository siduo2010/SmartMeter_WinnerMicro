AP+STAʹ��ָ��


ĿǰAP+STA��ʹ�ó��˱�д�������ֻ��ͨ��ATָ�������ƣ�������ο���wm_apsta_demo.c����ATָ��ʹ�ò������£�
1.���ù���ģʽΪAP+STA��
AT+WPRT=!3
2.����SOFTAP��SSID��
AT+APSSID=!"SOTAP_SSID"
3.����SOFTAP�ļ�������(��WPA2-TKIP)��
AT+APENCRY=!5
4.����SOFTAP������(��ASCII��87654321)��
AT+APKEY=!1,0,87654321
5.����SOFTAP��IP��ַ��Ϣ��
AT+APNIP=!1,192.168.8.1,255.255.255.0,192.168.8.1,192.168.8.1
6.����STA��SSID��
AT+SSID=!"STA_SSID"
7.����STA�����룺
AT+KEY=!1,0,12345678
8.��������
AT+WJOIN
9.��ѯSTA����״̬
AT+LKSTT
10.��ѯSOFTAP����״̬
AT+APLKSTT
11.STA����
AT+WLEAV
12.SOFTAP����
AT+WLEAV=2
