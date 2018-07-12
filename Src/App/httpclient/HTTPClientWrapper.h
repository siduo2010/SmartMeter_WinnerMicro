
#ifndef HTTP_CLIENT_WRAPPER
#define HTTP_CLIENT_WRAPPER

#include "wm_config.h"
#include "lwip/arch.h"

#if TLS_CONFIG_HTTP_CLIENT_SECURE
#if TLS_CONFIG_USE_POLARSSL
#include "polarssl/config.h"
#include "polarssl/ssl.h"
#else
#include "matrixsslApi.h"
#endif
#endif
// Compilation mode
#define _HTTP_BUILD_WIN32            // Set Windows Build flag


///////////////////////////////////////////////////////////////////////////////
//
// Section      : Microsoft Windows Support
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _HTTP_BUILD_WIN32

//#pragma warning (disable: 4996) // 'function': was declared deprecated (VS 2005)
#include <stdlib.h>
#include <string.h>
//#include <memory.h>
#include <stdio.h>
#include <ctype.h>
//#include <time.h>
//#include <winsock.h>

#include "wm_type_def.h"
#include "wm_sockets.h"
#include "wm_http_client.h"
// Generic types
// Sockets (Winsock wrapper)

#define                              HTTP_ECONNRESET     (ECONNRESET) 
#define                              HTTP_EINPROGRESS    (EINPROGRESS)
#define                              HTTP_EWOULDBLOCK    (EWOULDBLOCK)

#define SOCKET_ERROR            (-1)
#define SOCKET_SSL_MORE_DATA            (-2)
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Section      : Functions that are not supported by the AMT stdc framework
//                So they had to be specificaly added.
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus 
extern "C" { 
#endif

#if TLS_CONFIG_HTTP_CLIENT_SECURE
#if TLS_CONFIG_USE_POLARSSL
typedef ssl_context   tls_ssl_t;
#else
typedef ssl_t   tls_ssl_t;
#endif
#endif


    // STDC Wrapper implimentation
    int                                 HTTPWrapperIsAscii              (int c);
    int                                 HTTPWrapperToUpper              (int c);
    int                                 HTTPWrapperToLower              (int c);
    int                                 HTTPWrapperIsAlpha              (int c);
    int                                 HTTPWrapperIsAlNum              (int c);
    char*                               HTTPWrapperItoa                 (char *buff,int i);
    void                                HTTPWrapperInitRandomeNumber    (void);
    long                                HTTPWrapperGetUpTime            (void);
    int                                 HTTPWrapperGetRandomeNumber     (void);
    int                                 HTTPWrapperGetSocketError       (int s);
    unsigned long                       HTTPWrapperGetHostByName        (char *name,unsigned long *address);
    int                                 HTTPWrapperShutDown             (int s,int in);  
    // SSL Wrapper prototypes
#if TLS_CONFIG_HTTP_CLIENT_SECURE
    int                                 HTTPWrapperSSLConnect           (tls_ssl_t **ssl_p,int s,const struct sockaddr *name,int namelen,char *hostname);
    int                                 HTTPWrapperSSLNegotiate         (tls_http_session_handle_t pSession,int s,const struct sockaddr *name,int namelen,char *hostname);
    int                                 HTTPWrapperSSLSend              (tls_ssl_t *ssl,int s,char *buf, int len,int flags);
    int                                 HTTPWrapperSSLRecv              (tls_ssl_t *ssl,int s,char *buf, int len,int flags);
    int                                 HTTPWrapperSSLClose             (tls_ssl_t *ssl, int s);
    int                                 HTTPWrapperSSLRecvPending       (tls_http_session_handle_t pSession,int s);
#endif
    // Global wrapper Functions
#define                             IToA                            HTTPWrapperItoa
#define                             GetUpTime                       HTTPWrapperGetUpTime
#define                             SocketGetErr                    HTTPWrapperGetSocketError 
#define                             HostByName                      HTTPWrapperGetHostByName
#define                             InitRandomeNumber               HTTPWrapperInitRandomeNumber
#define                             GetRandomeNumber                HTTPWrapperGetRandomeNumber

#ifdef __cplusplus 
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Section      : Global type definitions
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

//#ifndef NULL
//#define NULL                         0
//#endif
//#define TRUE                         1
//#define FALSE                        0

#ifdef u_long
#undef u_long
#endif
#ifdef BOOL
#undef BOOL
#endif
#ifdef UCHAR
#undef UCHAR
#endif
#ifdef CHAR
#undef CHAR
#endif
#ifdef UINT16
#undef UINT16
#endif
#ifdef INT16
#undef INT16
#endif
#ifdef UINT32
#undef UINT32
#endif
#ifdef INT32
#undef INT32
#endif
#ifdef UINT64
#undef UINT64
#endif
#ifdef INT64
#undef INT64
#endif
#ifdef ULONG
#undef ULONG
#endif
#ifdef LONG
#undef LONG
#endif
#define VOID                    void
typedef int BOOL;
typedef unsigned char           UCHAR;
//typedef signed char             CHAR;
typedef char                    CHAR;
typedef unsigned short          UINT16; 
typedef signed short            INT16;
typedef unsigned int            UINT32;
typedef signed int              INT32;
typedef unsigned long long      UINT64;
typedef long long               INT64;
typedef unsigned long           ULONG;
typedef signed long             LONG;
typedef unsigned long           u_long;

// HTTP Supported authentication methods 
typedef enum _HTTP_AUTH_SCHEMA
{
    AuthSchemaNone      = WM_AUTH_SCHEMA_NONE,
    AuthSchemaBasic,
    AuthSchemaDigest,
    AuthSchemaKerberos,
    AuthNotSupported

} HTTP_AUTH_SCHEMA;

// HTTP supported verbs
typedef enum _HTTP_VERB
{
    VerbGet             = WM_VERB_GET,
    VerbHead,
    VerbPost,
    VerbPut,
    VerbNotSupported
    // Note: others verb such as connect and put are currently not supported

} HTTP_VERB;

// Data structure that the caller can request at any time that will include some information regarding the session
#if 0
typedef struct _HTTP_CLIENT
{
    UINT32  HTTPStatusCode;                 // HTTP Status code (200 OK)
    UINT32  RequestBodyLengthSent;          // Total bytes sent (body only)
    UINT32  ResponseBodyLengthReceived;     // Total bytes received (body only)
    UINT32  TotalResponseBodyLength;        // as extracted from the “content-length" header
    UINT32  HttpState;
} HTTP_CLIENT;

typedef struct _HTTPParameters
{
    CHAR*                  Uri;        
    CHAR*                  ProxyHost;  
    UINT32                 UseProxy ;  
    UINT32                 ProxyPort;
    UINT32                 Verbose;
    CHAR*                  UserName;
    CHAR*                  Password;
    HTTP_AUTH_SCHEMA       AuthType;
} HTTPParameters;
#endif
typedef tls_http_client_t HTTP_CLIENT;
typedef tls_http_param_t  HTTPParameters;

// Global socket structures and definitions
#define HTTP_INVALID_SOCKET          (-1)
typedef struct sockaddr_in           HTTP_SOCKADDR_IN;
typedef struct timeval               HTTP_TIMEVAL; 
typedef struct hostent               HTTP_HOSTNET;
typedef struct sockaddr              HTTP_SOCKADDR;
typedef struct in_addr               HTTP_INADDR;


#endif // HTTP_CLIENT_WRAPPER
