					demo��ʾ��������


���ĵ����õ���demo��صĺ궨����wm_demo.h�С���ʾdemoʱ��Ҫ�򿪸�demo��Ӧ�ĺ궨�塣
demo��ʾ��Ҫ�ڿ���̨�½��У���DEMO_CONSOLE�������˿���̨��
����̨��������ݻ����ж��Ƿ��������ַ�����
����������ַ�����������������Ӧ��demo��
������������ַ���������socketʱ����Ϊ����ͨ��socket���͡�

1��gpio demo
	����̨����t-gpio���Ӵ�ӡ��Ϣ�м��ɿ���gpio��д�����
	���demo����μ�wm_gpio_demo.c��

2��gpio �ж�demo
	����̨����t-ioisr�����ɲ���gpio12���жϣ���ƽ�仯����뵽�жϻص������У�
	���demo����μ�wm_gpio_demo.c��

3��flash demo
	����̨����t-flash�����ɲ���flash��д��
	���demo����μ�wm_flash_demo.c��

4��master spi demo
	���Ըù�����Ҫ����һģ����Ϊspi���豸������spi�ӿڶԽӺ�֮��
	����̨����t-mspi�����ɿ������Խ����
	���demo����μ�wm_master_spi_demo.c��

5��encrypt demo
	����̨����t-crypt�����ɲ��Լ���API��
	��ǰdemo�Ľӿ��У�AES-128���ӽ��ܣ���RC4���ӽ��ܣ���MD5��HMAC-MD5��SHA1��
	demoʹ�õ�ԭʼ����Ԥ���ڴ����У�16�ֽڣ�
	�ӽ��ܼ���Ľ����ʾ�ڴ����նˣ�
	���demo����μ�wm_crypt_demo.c

6������demo֮ һ����������
	����̨����t-oneshot��ģ�鼴���ڵȴ�һ������״̬�����ֻ�app����ssid�����룬�ȴ������ɹ���
	���demo����μ�wm_connect_net_demo.c��

7������demo֮ api�ӿ�����
	����̨����t-connect("ssid","pwd")��open����ʱpwd��Ҫ�����ַ���("")��ģ�鼴��������״̬��
	���demo����μ�wm_connect_net_demo.c��

8������demo֮ wps pbc����
	����̨����t-wpspbc,���ɿ�ʼWPS PBCģʽ��������������֮�󣬰���AP��WPS��ť�����ɵȴ����������
	���demo����μ�wm_wps_demo.c

9������demo֮ wps pin����
10��t-wpsgetpin
	Ҫ����WPS PINģʽ��������Ҫ������t-wpsgetpin�����ȡPIN Code�� ������Ὣ8λPIN��ʾ�ڿ���̨��
	ͬʱ����Ӧ���ڼ�����ʼ��WPS PIN������ʽ��
	֮�󣬿���̨����t-wpspin,���ɿ�ʼWPS PINģʽ��������Ҫȷ��2����֮�ڽ�PIN���뵽AP����ҳ�棻
	���demo����μ�wm_wps_demo.c

11���ಥ�㲥demo
	����̨����t-mcast�����һ�����û���������ʽ�����ɹ�֮�󣬱�demo����ip(224.1.2.1)��Port(5100)
	�Ķಥ�鷢��20�����ݣ�����Ϊ1024�ֽڡ�
	���demo����μ�wm_mcast_demo.c��

12���ֻ�����demo
	����̨����t-skfwup�����һ�����û���������ʽ�����ɹ�֮�󣬱�demo���Զ�����һ��TCP����������65533�˿ڣ�
	ͨ���ֻ�APP(FwUp)���Զ�ģ��ͨ��TCP�ķ�ʽ����������
	���demo����μ�wm_socket_fwup_demo.c��

13��raw�ӿڵ�socket client �շ�����demo
	��tcp�������ֽ���һ��socket server���˿�1000������������
	����̨��remoteip��������server��ip��ַ��Ȼ������t-rawskc�����һ�����û���������ʽ�����ɹ�֮��
	���Զ�����socket client����server���ӣ����ӳɹ�֮�󼴿���tcp�������ֺͿ���̨�������ݡ�
	���demo����μ�wm_raw_socket_client_demo.c��

14��raw�ӿڵ�socket server �շ�����demo
	����̨����t-rawsks�����һ�����û�������ʽ�����ɹ�֮�󣬼��Զ�����socket server��
	�����˿�1020����tcp�����������Ӹ�server�����ӳɹ�֮�󼴿ɽ�������ͨ�š�
	���demo����μ�wm_raw_socket_server_demo.c��

15����׼�ӿڵ�socket client �շ�����demo
	��ʾ��ʽ��raw �ӿڵ�socket clientһ��;
	���demo����μ�wm_socket_client_demo.c��

16����׼�ӿڵ�socket server �շ�����demo
	��ʾ��ʽ��raw�ӿڵ�socket serverһ��;
	���demo����μ�wm_socket_server_demo.c��

17�����socketͬʱ������demo
	����̨����t-stdsocks�����һ�����û���������ʽ�����ɹ�֮�󣬱�demo���Զ�����һ��TCP����������1234�˿ڣ�
	������tcp�����������Ӹ�Server�����֧��7��Socket���ӣ����ӳɹ��󣬿���ģ�鷢�����ݣ�ģ����ӡ�յ�������
	��uart0���ڡ�
	���demo����μ�wm_socket_server_sel_demo.c��

18��������ap��demo
	����̨����t-softap���demo�����Զ�����һ����soft_ap_demo����OPEN��AP��
	����ͨ���ֻ����ʼǱ��������Ӵ�AP���ԣ� 
	���demo����μ�wm_softap_demo.c; 
	
19��http����demo
	�����ɹ��󣬿���̨����t-httpfwup���ģ�鼴��ʼ�ӵ�ַ��http://RemoteIP:8080/TestWeb/cuckoo.do��
	���ع̼���������������RemoteIP���ֿ�ͨ������Զ��ip��ַ������ġ�
	���demo����μ�wm_http_demo.c��

20��http����demo
	�����ɹ��󣬿���̨����t-httpget���ģ�鼴��ʼ�ӵ�ַ��http://RemoteIP:8080/TestWeb��
	������Ϣ����ӡ�ڿ���̨������RemoteIP���ֿ�ͨ������Զ��ip��ַ������ġ�
	���demo����μ�wm_http_demo.c��

21��http�ϴ�demo
	�����ɹ��󣬿���̨����t-httppost���ģ�鼴��ʼ���ַ��http://RemoteIP:8080/TestWeb/login.do��post���ݣ�
	��������Ϊ: user=xxxxx������t-httppostuser=WinnerMicro������RemoteIP���ֿ�ͨ������Զ��ip��ַ������ġ�
	���demo����μ�wm_http_demo.c��

22������Զ��ip��ַ����
	����ʾ��:remoteip=192.168.1.111�����������ģ�����ͻ���ʱʹ�ã����÷�������ip��ַ��
	���demo����μ�wm_uart_demo.c��

23�����ÿ���̨����������
	����ʾ��:baudrate=115200; ������Ҫ�ֺ���Ϊ�����������
	���demo����μ�wm_uart_demo.c��

24���رտ���̨����
	ͨ������̨�´������󣬿���̨������ʹ�ã����´���Ҫ��λģ�飻
	���demo����μ�wm_uart_demo.c��

25��demo������Ϣ����
	ͨ������̨�´������󣬿��Դ�ӡ��demo�����˵����Ϣ��
	���demo����μ�wm_uart_demo.c��

26������demo֮ apsta�ӿ�����
	����̨����t-apsta("ssid","pwd","ssid2")��open����ʱpwd��Ҫ�����ַ���("")������ָ���ģ�鼴��������״̬��
	ssid2������ַ���("")�������������apʹ�ú�ssidһ�����������ƣ��������ͺ����벻�䡣
	���demo����μ�wm_apsta_demo.c��

