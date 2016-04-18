/*
 * http_serv.c
 */
#include "net_serv.h"
#include "amn_module.h"

int http_handle_callback( int method, struct resp_message_t *resp )
{
	/* HTTP server call */
	web_server_callback( &resp->parser, resp->req, &resp->info );
	return resp->req->status_code != HTTP_OK;
	//return 0;
}

//*******************************************************************************************************
/*
 * The function is NETSERV_TYPE
 */
int serv_http_init(loreq_t *loreq)
{
	static int	_init_ = 0;
	struct amn_cfgspace_value_t *cfg_val;
	int rtsp, httpw_port, https;

	if (_init_ != 0 || loreq->nmtype != NETSERV_TYPE) return 0;
	_init_ = 1;

	// Web & Streaming services
	cfg_val = amn_currConfig_get( "Net.Port.Service" );
	amn_cfgval_ifields3( cfg_val, rtsp, httpw_port, https );
	
	netserver_new( httpw_port, NET_SERVICE_HTTPW, prepare_HTTP_response, http_handle_callback );
	return _init_;
}
#pragma arm section code = "netnode_init", rwdata = "netnode_init", zidata = "netnode_init"
netnode_init_t	_serv_init_http = serv_http_init;
#pragma arm section code, rwdata,  zidata
