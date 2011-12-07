#ifndef __HTTP_H__
#define __HTTP_H__
#include <limits.h>
#ifdef  LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <tcpd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef int SOCKET;
#define INVALID_SOCKET				-1
#endif

#ifdef  WIN32
#include <winsock.h>
#include <limits.h>
#define close	closesocket
#else
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#define SOCKET					int
#define INVALID_SOCKET			-1
#endif

#include "http_storage.h"

#define TRUE						1
#define FALSE						0
#define INFINITY					LONG_MAX

#define HT_OK						0
#define HT_FATAL_ERROR				0xFFFF
#define HT_INVALID_ARGUMENT			0xFFFE
#define HT_SERVICE_UNAVAILABLE   	0xFFFD
#define HT_RESOURCE_UNAVAILABLE		0xFFFC
#define HT_MEMORY_ERROR				0xFFFB
#define HT_NETWORK_ERROR			0xFFFA
#define HT_ILLEGAL_OPERATION		0xFFF9
#define HT_HOST_UNAVAILABLE			0xFFF8		
#define HT_IO_ERROR					0xFFF7

#define HTTP_FIRST_METHOD			0
#define HTTP_GET					0
#define HTTP_PUT           	   		1
#define HTTP_POST             		2
#define HTTP_LOCK             		3
#define HTTP_UNLOCK					4
#define HTTP_PROPFIND				5
#define HTTP_PROPPATCH				6
#define HTTP_MKCOL					7
#define HTTP_DELETE					8
#define HTTP_HEAD					9
#define HTTP_LAST_METHOD			9

typedef struct http_connection HTTP_CONNECTION;
typedef struct http_request HTTP_REQUEST;
typedef struct http_response HTTP_RESPONSE;
typedef struct http_header_field HTTP_HEADER_FIELD;
typedef struct http_auth_parameter HTTP_AUTH_PARAMETER;
typedef struct http_auth_info HTTP_AUTH_INFO;

struct http_connection {
	SOCKET socketd;
	int status;
	char *host;
	struct sockaddr_in address;
	int persistent;
	int lazy;
	HTTP_AUTH_INFO *auth_info;
	char read_buffer[128];
	int read_count;
	int read_index;
};

#define hoststr(c)		((c->host != NULL) ? c->host : inet_ntoa(c->address.sin_addr))

struct http_header_field {
	char *name;
	char *value;
	HTTP_HEADER_FIELD *prev_field;
	HTTP_HEADER_FIELD *next_field;
};

struct http_auth_parameter {
	char *name;
	char *value;
	HTTP_AUTH_PARAMETER *next_parameter;
	HTTP_AUTH_PARAMETER *prev_parameter;
};

struct http_auth_info {
	char *method;
	int count;
	HTTP_AUTH_PARAMETER *first_parameter;
	HTTP_AUTH_PARAMETER *last_parameter;
};

struct http_request {
	int method;
	char *resource;
	HTTP_HEADER_FIELD *first_header_field;
	HTTP_HEADER_FIELD *last_header_field;
	HTTP_STORAGE *content;
};

struct http_response {
	int status_code;
	char *status_msg;
	char *version;
	HTTP_HEADER_FIELD *first_header_field;
	HTTP_HEADER_FIELD *last_header_field;
	HTTP_STORAGE *content;
};

typedef int (*HTTP_EVENT_HANDLER)(HTTP_CONNECTION *connection, HTTP_REQUEST *request, HTTP_RESPONSE *response, void *data);

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*http_allocator)(void *ud, void *ptr, size_t nsize);
void http_set_allocator(http_allocator allocator, void* userdata);

int http_connect(HTTP_CONNECTION **connection, const char *host, short port, const char *username, const char *password);
int http_connect_lazy(HTTP_CONNECTION **connection, const char *host, short port, const char *username, const char *password);
int http_disconnect(HTTP_CONNECTION **connection);
int http_add_header_field(HTTP_REQUEST *request, const char *field_name, const char *field_value);
int http_add_header_field_number(HTTP_REQUEST *request, const char *field_name, int field_value);

int http_exec(HTTP_CONNECTION *connection, int method, const char *resource, 
		HTTP_EVENT_HANDLER on_request_header, HTTP_EVENT_HANDLER on_request_entity, 
		HTTP_EVENT_HANDLER on_response_header, HTTP_EVENT_HANDLER on_response_entity,
		void *data);
int http_exec_error(void);
const char * http_exec_error_msg(void);

const char * http_find_header_field(HTTP_RESPONSE *response, const char *field_name, const char *default_value);
long int http_find_header_field_number(HTTP_RESPONSE *response, const char *field_name, int default_value);
int http_has_header_field(HTTP_RESPONSE *response, const char *field_name, const char *field_value);

#ifdef __cplusplus
}
#endif

#endif
