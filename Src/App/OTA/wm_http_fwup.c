#include <string.h>
#include <stdio.h>
#include "wm_socket_fwup.h"
#include "wm_http_fwup.h"
#include "wm_debug.h"
#include "wm_mem.h"

#if TLS_CONFIG_HTTP_CLIENT

#define HTTP_CLIENT_BUFFER_SIZE  1024

int http_fwup(tls_http_param_t ClientParams)
{
	int                   nRetCode;
    u32                  nSize,nTotal = 0;
    char*                   Buffer;
    tls_http_session_handle_t     pHTTP;
    char token[32];
    u32 content_length=0, size=32;
    struct pbuf *p;

    do
    {
        Buffer = (char*)tls_mem_alloc(HTTP_CLIENT_BUFFER_SIZE);
        if(Buffer == NULL)
            return HTTP_CLIENT_ERROR_NO_MEMORY;
        memset(Buffer, 0, HTTP_CLIENT_BUFFER_SIZE);
        TLS_DBGPRT_INFO("\nHTTP Client v1.0\n\n");
        // Open the HTTP request handle
        pHTTP = tls_http_client_open_request(0);
        if(!pHTTP)
        {
            nRetCode =  HTTP_CLIENT_ERROR_INVALID_HANDLE;
            break;
        }
        // Set the Verb
        nRetCode = tls_http_client_set_verb(pHTTP, WM_VERB_GET);
        if(nRetCode != HTTP_CLIENT_SUCCESS)
        {
            break;
        }        
#if TLS_CONFIG_HTTP_CLIENT_AUTH
        // Set authentication
        if(ClientParams.auth_type != WM_AUTH_SCHEMA_NONE)
        {
            if((nRetCode = tls_http_client_set_auth(pHTTP,ClientParams.auth_type,NULL)) != HTTP_CLIENT_SUCCESS)
            {
                break;
            }

            // Set authentication
            if((nRetCode = tls_http_client_set_credentials(pHTTP,ClientParams.user_name,ClientParams.password)) != HTTP_CLIENT_SUCCESS)
            {
                break;
            }
        }
#endif //TLS_CONFIG_HTTP_CLIENT_AUTH
#if TLS_CONFIG_HTTP_CLIENT_PROXY
        // Use Proxy server
        if(ClientParams.use_proxy == TRUE)
        {
            if((nRetCode = tls_http_client_set_proxy(pHTTP,ClientParams.proxy_host,ClientParams.proxy_port,NULL,NULL)) != HTTP_CLIENT_SUCCESS)
            {

                break;
            }
        }
#endif //TLS_CONFIG_HTTP_CLIENT_PROXY
	 if((nRetCode = tls_http_client_send_request(pHTTP,ClientParams.url,NULL,0,FALSE,0,0)) != HTTP_CLIENT_SUCCESS)
        {
            break;
        }
        // Retrieve the the headers and analyze them
        if((nRetCode = tls_http_client_recv_response(pHTTP,3)) != HTTP_CLIENT_SUCCESS)
        {
            break;
        }
        memset(token, 0, 32);
        if((nRetCode = tls_http_client_find_first_header(pHTTP, "content-length", token, &size)) != HTTP_CLIENT_SUCCESS)
        {
            tls_http_client_find_close_header(pHTTP);
            break;
        }
        tls_http_client_find_close_header(pHTTP);
        content_length = atol(strstr(token,":")+1);
        nRetCode = socket_fwup_accept(0, ERR_OK);
        if(nRetCode != ERR_OK)
            break;
        // Get the data until we get an error or end of stream code
        while(nRetCode == HTTP_CLIENT_SUCCESS || nRetCode != HTTP_CLIENT_EOS)
        {
            // Set the size of our buffer
            nSize = HTTP_CLIENT_BUFFER_SIZE - 4;   
            // Get the data
            nRetCode = tls_http_client_read_data(pHTTP,Buffer+3,nSize,0,&nSize);
		if(nRetCode != HTTP_CLIENT_SUCCESS && nRetCode != HTTP_CLIENT_EOS)
			break;
		while (1) {
		    p = pbuf_alloc(PBUF_TRANSPORT, nSize + 3, PBUF_REF);
		    if (p != NULL) {
		        break;
		    } else {
		        /* delay 1 ticks */
		        tls_os_time_delay(1);
		    } 
		}
		if(nTotal == 0)
			*Buffer = SOCKET_FWUP_START;
		else if(nRetCode == HTTP_CLIENT_EOS)
			*Buffer = SOCKET_FWUP_END;
		else
			*Buffer = SOCKET_FWUP_DATA;
		*(Buffer+1) = (nSize>>8) & 0xFF;
		*(Buffer+2) = nSize & 0xFF;	
		p->payload =  Buffer;
		p->len = p->tot_len = nSize + 3;
        	nTotal += nSize;
		if(content_length)
                  printf("Download %%%d\n", nTotal*100/content_length);
		nRetCode = socket_fwup_recv(0, p, ERR_OK);
		if(nRetCode != ERR_OK)
			break;
        }
    } while(0); // Run only once
    tls_mem_free(Buffer);
    if(pHTTP)
        tls_http_client_close_request(&pHTTP);
    if(ClientParams.verbose == TRUE)
    {
        printf("\n\nHTTP Client terminated %d (got %d kb)\n\n",nRetCode,(nTotal/ 1024));
    }
    if(nRetCode)
        socket_fwup_err(0, nRetCode);
    return nRetCode;
}

int t_http_fwup(char *url)
{
	tls_http_param_t httpParams;
	memset(&httpParams, 0, sizeof(tls_http_param_t));
	if (url == NULL)
	{
		httpParams.url = "http://%d.%d.%d.%d:8080/TestWeb/cuckoo.do";
	}
	else
	{
		httpParams.url = url;
	}
	printf("Location: %s\n",httpParams.url);
	httpParams.verbose = TRUE;
	return http_fwup(httpParams);
}

#endif //TLS_CONFIG_HTTP_CLIENT && TLS_CONFIG_SOCKET_RAW

