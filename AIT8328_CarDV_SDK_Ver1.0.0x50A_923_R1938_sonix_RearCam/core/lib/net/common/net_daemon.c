/*
*********************************************************************************************************
*              AIT Multimedia Network -- Server Daemon for Streaming Server && HTTP
*                     (c) Copyright 2012, Alpha Imaging Technology Corp.
*               All rights reserved. Protected by international copyright laws.
*********************************************************************************************************
*
* Description: 
*
*/

#define __ENABLE_LWIP_SOCKET_REDEFINE__
#include "inc/net_serv.h"
#include "inc/amn_event.h"
#include "netapp.h"
struct mserv_request_t {
	/*! Connection handle. */
	SOCKET connfd;
	/*! . */
	struct sockaddr_storage foreign_sockaddr;
};

static struct net_service_t net_services[NET_SERVICES_TOTAL];
char *net_servname[NET_SERVICES_TOTAL] = { "RTSP", "HTTP-STREAM", "HTTP-WEB" };

extern void web_server_callback(http_parser_t *parser, INOUT http_message_t *req, SOCKINFO *info);

//*******************************************************************************************************
void tcp_send_message( struct resp_message_t *resp )
{
	int timeout = 0;
	if (resp->content)
		http_SendMessage( &(resp)->info, &timeout, "bb",
				(resp)->headers.buf, (resp)->headers.length,
				(resp)->content, (resp)->content_len );
	else
		http_SendMessage( &(resp)->info, &timeout, "b",
				(resp)->headers.buf, (resp)->headers.length );
}

/*!
 * \brief Send Error Message.
 *        Rewrite from LIBUPNP::miniserver.c
 */
static UPNP_INLINE void handle_error(
	/*! [in] Socket Information object. */
	SOCKINFO *info,
	/*! [in] HTTP Error Code. */
	int http_error_code,
	/*! [in] Major Version Number. */
	int major,
	/*! [in] Minor Version Number. */
	int minor)
{
	osal_dprint(VERBOSE, "HTTP/RTSP Error: %d", http_error_code);
	http_SendStatusResponse(info, http_error_code, major, minor);
}

void prepare_response(struct resp_message_t *resp)
{
	/* init */
	membuffer_init(&resp->headers);
	resp->content = NULL;
	resp->content_len = 0;
	resp->req = &resp->parser.msg;
	resp->req->enclose_obj = resp;  // to associate the child object with this enclosing one

	osal_dprint(VERBOSE, "client request: '%s' '%s' ", resp->req->method_name, resp->req->urlbuf );
}

/*!
 * \brief Based on the type pf message, appropriate callback is issued.
 *        Rewrite from LIBUPNP::miniserver.c
 *
 * \return 0 on Success or HTTP_INTERNAL_SERVER_ERROR if Callback is NULL.
 */
static int dispatch_request(
	/*! [in] HTTP parser object. */
	http_parser_t *hparser,
	/*! [out] Response Message Control Structure for HTTP/RTSP Request. */
	struct resp_message_t *resp)
{
	int	ret;
	/*
	 * call to service prepare
	 *	HTTP: http_handle_callback
	 *	RTSP: rtsp_protocol_prepare
	 */
	ret = resp->serv->prepare( (int)hparser->msg.method, resp );
	if (ret != HTTP_CONTINUE)
		return ret;
	/*refer to revision 2422: [A810] Buf fix of another OS tasks racing

      Observed problem: sometimes ARM will encounter Data Abort within session_destroy(), which comes from the
      TEARDOWN of RTSP session in liveRTSP_teardown()

      There was already a mutex-lock within the Streamer Server's Event Manager. However, since the integration
      of HTTP/RTSP parser with libupnp, that execution path is entirely exposed nakedly !!
      Using the same lock: evman_exec_lock to protect it. */
	osal_mutex_lock( &evman_exec_lock );
	/*
	 * call to service handler
	 *	HTTP: http_handle_callback
	 *	RTSP: rtsp_protocol_handler
	 */
	ret = resp->serv->handler( (int)hparser->msg.method, resp );
	osal_mutex_unlock( &evman_exec_lock );

	return ret;
}

/*!
 * \brief Receive the request and dispatch it for handling.
 *        Rewrite from LIBUPNP::miniserver.c
 */
static osal_thread_t handle_request(
	/*! [in] Request Message to be handled. */
	void *args)
{
	int http_error_code;
	int ret_code;
	int major = 1;
	int minor = 1;
	http_message_t *hmsg = NULL;
	int timeout;
	struct resp_message_t *resp = (struct resp_message_t *)args;
	SOCKET connfd = resp->connfd;

	osal_dprint( VERBOSE, "miniserver fd=%d: INIT", connfd );
	/* parser_request_init( &parser ); */ /* LEAK_FIX_MK */
	hmsg = &resp->parser.msg;
	ret_code = sock_init_with_ip( &resp->info, connfd, (struct sockaddr *)&resp->foreign_sockaddr);
	if (ret_code != UPNP_E_SUCCESS) {
		httpmsg_destroy(hmsg);
		osal_free(resp);
		return osal_thread_RETURN;
	}
	ADD_SOCKINFO_REF(&resp->info);
	do {
		/* read */
		//timeout = HTTP_DEFAULT_TIMEOUT;
		//"timeout = 0" means to use SOCK_READ_WRITE_DEFAULT_TIMEOUT as timeout 
		timeout = 0;
		ret_code = http_RecvMessage( &resp->info, &resp->parser, HTTPMETHOD_UNKNOWN, &timeout, &http_error_code);
		if (ret_code != 0) {
			if (ret_code == UPNP_E_TIMEDOUT) {
				goto L_continue_http_loop;
			}
			osal_dprint(INFO, "miniserver fd=%d: socket may be closed, err(%d)", connfd, ret_code);
			goto error_handler;
		}

		osal_dprint(VERBOSE, "miniserver fd=%d: PROCESS...", connfd);
		/* dispatch */
		http_error_code = dispatch_request(&resp->parser, resp);
		if (http_error_code != 0) {
			osal_dprint(INFO, "miniserver fd=%d: http_err=%d", connfd, http_error_code);
			goto error_handler;
		}

L_continue_http_loop:
		http_error_code = 0;
		httpmsg_destroy(hmsg);
		
		if(get_NetAppStatus() == NETAPP_WIFI_DOWN)
			break;
		
	} while (resp->persistant);

error_handler:
	osal_dprint(VERBOSE, "miniserver fd=%d: END...", connfd);
	if (http_error_code > 0) {
		if (hmsg) {
			major = hmsg->major_version;
			minor = hmsg->minor_version;
		}

		// When Socket is outage, we have no way to send HTTP-ERROR response
		if (ret_code != UPNP_E_SOCKET_ERROR)
			handle_error(&resp->info, http_error_code, major, minor);
	}
	httpmsg_destroy(hmsg);

#if 0
	// drop_conn()
	if( resp->proto_state )
		switch( resp->proto )
		{
		case CONN_PROTO_HTTP:
			http_conn_disconnect( resp );
			break;
		case CONN_PROTO_RTSP:
			rtsp_conn_disconnect( resp );
			break;
		}
	evmsg_remove( (struct ammo_evmsg*)resp->read_event );
	if( resp->second_read_event ) evmsg_remove( (struct ammo_evmsg*)resp->second_read_event );
	evmsg_remove( (struct ammo_evmsg*)resp->write_event );
	if( resp->fd >= 0 ) close( resp->fd );
	resp->fd = -1;
	if( resp->second_fd >= 0 ) close( resp->second_fd );
	resp->second_fd = -1;
	if( resp->next ) resp->next->prev = resp->prev;
	if( resp->prev ) resp->prev->next = resp->next;
	else conn_list = resp->next;
	osal_bgfree( resp );
#endif
	session_event_teardown_resp(resp);
	if (REL_SOCKINFO_REF(&resp->info) == 0) {
		// printc("FREE %d\r\n", resp->info.socket);
		sock_destroy(&resp->info, SD_BOTH);
		osal_free(resp);
	}

	osal_dprint(VERBOSE, "miniserver %d: COMPLETE", connfd);
	return osal_thread_RETURN;
}

static void do_accept( struct ammo_evmsg *ei, void *d )
{
	struct net_service_t *serv = (struct net_service_t *)d;
	int connfd, i;
	union {
		struct sockaddr_storage storage;
		struct sockaddr_in      in4;
	} clientAddr;
	struct resp_message_t *resp;
	//struct conn *c;

	i = sizeof( clientAddr );
	if( ( connfd = net_accept( serv->fd, (struct sockaddr *)&clientAddr.in4, (socklen_t*)&i ) ) < 0 ) {
		osal_dprint(WARN, "Fails accepting %s connection: %s", serv->name, osal_strerror() );
		// Wait resource
		sys_msleep(500);
		return;
	}
	osal_dprint(VERBOSE, "accepted %s connection from %s:%d, fd=%d, listener=%d", serv->name, 
			inet_ntoa( clientAddr.in4.sin_addr ), ntohs( clientAddr.in4.sin_port ), connfd, serv->fd );

	resp = (struct resp_message_t *)osal_zmalloc( sizeof (struct resp_message_t) );
	
	if(!resp){
		osal_dprint(ERR, "%s : osal_zmalloc failed", __func__ );
		goto L_fail_do_accept;
	}
	
	if( net_fcntl( connfd, F_SETFL, O_NONBLOCK ) < 0 ) {
		osal_dprint(INFO, "Fails setting O_NONBLOCK: %s", osal_strerror() );
		goto L_fail_do_accept;
	}

	i = 1;
	if( net_setsockopt( connfd, SOL_TCP, TCP_NODELAY, &i, sizeof( i ) ) < 0 ) {
		osal_dprint(INFO, "Fails setting TCP_NODELAY: %s", osal_strerror() );
		goto L_fail_do_accept;
	}

	// LIBUPNP::miniserver.c::schedule_request_job()
	
	resp->connfd = connfd;
	resp->serv   = serv;
	memcpy(&resp->foreign_sockaddr, &clientAddr, sizeof(clientAddr));

	if (sys_thread_avaliable(LWIP_STK_SIZE, 0) == 0) {
		printc("++++NO AVALIABLE THREAD to do_accept++++\n");
		goto L_fail_do_accept;
	}
	osal_thread_new( handle_request, resp );
	return;
L_fail_do_accept:
	net_close(connfd);
	if (resp)
		osal_free(resp);
	return;
}

static struct net_service_t *tcp_listen( struct net_service_t *serv )
{
	struct sockaddr_in addr;
#ifdef __AMN_OS_LINUX__
	int opt;
#endif

	if( ( serv->fd = net_socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		osal_dprint(ERR, "create NetDaemon socket: %s", osal_strerror() );
		return NULL;
	}

#ifdef __AMN_OS_LINUX__
	opt = 1;
	if( net_setsockopt( serv->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) ) < 0 )
		osal_dprint(WARN, "ignore setsockopt error: %s", osal_strerror() );
#endif

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0;
	addr.sin_port = htons( serv->port );
	if( net_bind( serv->fd, (struct sockaddr *)&addr, sizeof( addr ) ) < 0 ) {
		osal_dprint(ERR, "fail to bind socket: %s", osal_strerror() );
		net_close( serv->fd );
		return NULL;
	}

	if( net_listen( serv->fd, 5 ) < 0 ) {
		osal_dprint(ERR, "fail to setup TCP-LISTEN: %s", osal_strerror() );
		net_close( serv->fd );
		return NULL;
	}

	evmsg_new_FD( serv->fd, 0, 0, do_accept, serv );
	osal_dprint(VERBOSE, "Net Service %s: listen on port %d, fd=%d", serv->name, serv->port, serv->fd );

	return serv;
}

//*******************************************************************************************************

// We may have the same port supporting RTSP & HTTP at the same time
// Binding of PORT to HTTP or RTSP service
struct net_service_t *netserver_new( int port, int serv_type, svr_prepare_f prepare, svr_handler_f handler)
{
	struct net_service_t *serv;
	int i;

	for (i=0; i<NET_SERVICES_TOTAL; i++) {
		if (net_services[i].port == port) {
			osal_dprint(INFO, "Reused port=%d  serv_type=%d", port, serv_type);
			return NULL;
		}
	}

	if (serv_type >= NET_SERVICES_TOTAL || port <= 0 || port > 65535 ) {
		osal_dprint(ERR, "invalid param: port=%d  serv_type=%d", port, serv_type);
		return NULL;
	}

	serv = &net_services[serv_type];
	serv->port = port;
	serv->name = net_servname[serv_type];
	serv->prepare = prepare;
	serv->handler = handler;
	osal_dprint(VERBOSE, "%s Server PORT: %d", serv->name, port);
	return tcp_listen( serv );
}



