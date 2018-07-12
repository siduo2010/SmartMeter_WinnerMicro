#include <string.h>
#include "wm_include.h"
#include "wm_config.h"
#include "wm_http_client.h"
#include "lwip/sys.h"
#include "wm_wl_task.h"

#if TLS_CONFIG_HTTP_CLIENT_TASK
#define HTTP_CLIENT_STK_SIZE 256
static sys_mbox_t http_client_mbox = SYS_MBOX_NULL;
static OS_STK httpClientStk[HTTP_CLIENT_STK_SIZE]; 
#define    HTTP_CLIENT_BUFFER_SIZE   1024
extern u8 pSession_flag;
static u32  http_snd_req(tls_http_session_handle_t http,
                            tls_http_param_t client_params,
                            tls_http_verb_t verb,
                            char* snd_data,
                            u32 data_len,
                            tls_http_client_recv_callback_fn recv_fn)
{
    u32   nRetCode;
    u32   nSize,nTotal = 0;
    char* Buffer = NULL;
    u32   nSndDataLen ;
    char  token[32];
    u32   size = 32;
    char *pRecvData = NULL;
    u32   totallen;
    do
    {
        Buffer = tls_mem_alloc(HTTP_CLIENT_BUFFER_SIZE);
        if(Buffer == NULL)
        {
            return HTTP_CLIENT_ERROR_NO_MEMORY;
        }
        memset(Buffer, 0, HTTP_CLIENT_BUFFER_SIZE);
        //printf("\nHTTP Client v1.0\n\n");
        nSndDataLen = (snd_data==NULL ? 0 : data_len);
        // Set the Verb
        nRetCode = tls_http_client_set_verb(http, verb);
        if(nRetCode != HTTP_CLIENT_SUCCESS)
        {
            break;
        }
#if TLS_CONFIG_HTTP_CLIENT_AUTH
        // Set authentication
        if(client_params.auth_type != WM_AUTH_SCHEMA_NONE)
        {
            if((nRetCode = tls_http_client_set_auth(http,client_params.auth_type,NULL)) != HTTP_CLIENT_SUCCESS)
            {
                break;
            }

            // Set authentication
            if((nRetCode = tls_http_client_set_credentials(http,client_params.user_name,client_params.password)) != HTTP_CLIENT_SUCCESS)
            {
                break;
            }
        }
#endif //TLS_CONFIG_HTTP_CLIENT_AUTH
#if TLS_CONFIG_HTTP_CLIENT_PROXY
        // Use Proxy server
        if(client_params.use_proxy == TRUE)
        {
            if((nRetCode = tls_http_client_set_proxy(http,client_params.proxy_host,client_params.proxy_port,NULL,NULL)) != HTTP_CLIENT_SUCCESS)
            {

                break;
            }
        }
#endif //TLS_CONFIG_HTTP_CLIENT_PROXY
	 if((nRetCode = tls_http_client_send_request(http,client_params.url,snd_data,nSndDataLen,verb==WM_VERB_POST || verb==WM_VERB_PUT,0,0)) != HTTP_CLIENT_SUCCESS)
        {
            break;
        }
        // Retrieve the the headers and analyze them
        if((nRetCode = tls_http_client_recv_response(http,30)) != HTTP_CLIENT_SUCCESS)
        {
            break;
        }
        memset(token, 0, 32);
        if((nRetCode = tls_http_client_find_first_header(http, "content-length", token, &size)) != HTTP_CLIENT_SUCCESS)
        {
        	totallen = HTTP_CLIENT_BUFFER_SIZE;
        }
        else
        {
        	totallen = atol(strstr(token,":")+1);
        	
        }
        tls_http_client_find_close_header(http);
	 //printf("Start to receive data from remote server...\n");
        // Get the data until we get an error or end of stream code
        pSession_flag = 0;
        while(nRetCode == HTTP_CLIENT_SUCCESS || nRetCode != HTTP_CLIENT_EOS)
        {
            // Set the size of our buffer
            nSize = HTTP_CLIENT_BUFFER_SIZE;   
            // Get the data
            nRetCode = tls_http_client_read_data(http,Buffer,nSize,300,&nSize);
            if(nRetCode != HTTP_CLIENT_SUCCESS && nRetCode != HTTP_CLIENT_EOS)
                break;
            pRecvData = tls_mem_alloc(nSize);
            if(pRecvData == NULL)
            {
                nRetCode = HTTP_CLIENT_ERROR_NO_MEMORY;
                break;
            }
            memcpy(pRecvData, Buffer, nSize);
            recv_fn(http, pRecvData, totallen, nSize);
        }
    } while(0); // Run only once
    if(Buffer)
    tls_mem_free(Buffer);
    if(http)
        tls_http_client_close_request(&http);
    if(client_params.verbose == TRUE)
    {
        printf("\n\nHTTP Client terminated %d (got %d kb)\n\n",nRetCode,(nTotal/ 1024));
    }
    return nRetCode;
}

static void http_client_rx(void *sdata)
{
	int ret;
	void *msg;
	tls_http_client_msg *http_msg;
	for(;;) 
	{
		sys_arch_mbox_fetch(http_client_mbox, (void **)&msg, 0);
		http_msg = (tls_http_client_msg *)msg;
		ret = http_snd_req(http_msg->session,
					http_msg->param,
					http_msg->method,
					http_msg->send_data,
					http_msg->data_len,
					http_msg->recv_fn);
//		printf("http_client_rx ret=%d\n", ret);
		if(ret == HTTP_CLIENT_SUCCESS || ret == HTTP_CLIENT_EOS)
		{
            ;
		}
		else
		{
			if(http_msg->err_fn != NULL)
				http_msg->err_fn(http_msg->session, ret);
		}
		if(http_msg->send_data != NULL)
			tls_mem_free(http_msg->send_data);
		if(http_msg->param.url)
			tls_mem_free(http_msg->param.url);
		tls_mem_free(http_msg);
	}

}


int tls_http_client_task_init(void)
{
	if(sys_mbox_new(&http_client_mbox, 32) != ERR_OK) {
          return WM_FAILED;
       }
	tls_os_task_create(NULL, "httpc",
			http_client_rx,
                    NULL,
                    (void *)httpClientStk,          /* 任务栈的起始地址 */
                    HTTP_CLIENT_STK_SIZE * sizeof(u32), /* 任务栈的大小     */
                    TLS_HTTP_CLIENT_TASK_PRIO,
                    0);
	return WM_SUCCESS;
}

int tls_http_client_post(tls_http_client_msg * msg)
{
	int len;
	err_t err;
	tls_http_client_msg * msg1 = tls_mem_alloc(sizeof(tls_http_client_msg));
	if(!msg1)
		return HTTP_CLIENT_ERROR_NO_MEMORY;
	memset(msg1, 0, sizeof(tls_http_client_msg));
	memcpy(msg1, msg, sizeof(tls_http_client_msg));
	if(msg->data_len > 0)
	{
		msg1->send_data = tls_mem_alloc(msg->data_len);
		if(msg1->send_data == NULL)
		{
			tls_mem_free(msg1);
			return HTTP_CLIENT_ERROR_NO_MEMORY;
		}
		memcpy(msg1->send_data, msg->send_data, msg->data_len);
	}
	else
		msg1->send_data = NULL;
	if(msg->param.url != NULL)
	{
		len = strlen(msg->param.url);
		msg1->param.url = tls_mem_alloc(len + 1);
		if(msg1->param.url == NULL)
		{
			if(msg1->send_data)
				tls_mem_free(msg1->send_data);
	        	tls_mem_free(msg1);
			return HTTP_CLIENT_ERROR_NO_MEMORY;
		}
		memset(msg1->param.url, 0, len + 1);
		memcpy(msg1->param.url,msg->param.url,len);
	}
	
        // Open the HTTP request handle
        msg->session= tls_http_client_open_request(0);
        if(!msg->session)
        {
        	if(msg1->send_data)
			tls_mem_free(msg1->send_data);
		if(msg1->param.url)
			tls_mem_free(msg1->param.url);
        	tls_mem_free(msg1);
		return HTTP_CLIENT_ERROR_INVALID_HANDLE;
        }
	msg1->session = msg->session;
	err = sys_mbox_trypost(http_client_mbox, msg1);
	if(err != ERR_OK)
	{
		tls_http_client_close_request(&msg->session);
        	if(msg1->send_data)
			tls_mem_free(msg1->send_data);
		if(msg1->param.url)
			tls_mem_free(msg1->param.url);
		tls_mem_free(msg1);
	}
	return err;
}


#endif //TLS_CONFIG_HTTP_CLIENT_TASK

