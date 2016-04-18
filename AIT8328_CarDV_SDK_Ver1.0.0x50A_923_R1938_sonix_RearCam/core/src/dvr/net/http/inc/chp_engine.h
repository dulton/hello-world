/*
*********************************************************************************************************
*                         AIT Multimedia Network -- CHP Execution Engine
*                     (c) Copyright 2012, Alpha Imaging Technology Corp.
*               All rights reserved. Protected by international copyright laws.
*********************************************************************************************************
*
* Description:  CHP, AIT's special "C-language personal Home Page", is a PHP-like web-service tools.
*
*--------------------------------------------------------------------------------------------------------
* CHP APIs, with PHP similar ones
*	CHP_PRINTF(...)               => echo, print
*	CHP_SERVER_SELF               => $_SERVER['PHP_SELF'], return the name of the current CGI-handler's web-path
*	CHP_REMOTE_ADDR               => $_SERVER['REMOTE_ADDR']
*	CHP_REMOTE_PORT               => $_SERVER['REMOTE_PORT']
*	CHP_GET_VARS(strName)         => $_GET[strName] / $HTTP_GET_VARS[]
*	CHP_GET_ARRAYS(strName)       => $_GET[strName] / $HTTP_GET_VARS[]
*	CHP_IN_ARRAY(val,arrayName)   => corresponds to PHP in_array(val,arrayName)
*	CHP_POST_VARS(strName)        => $_POST[strName] / $HTTP_POST_VARS[] ... not supported yet
*	CHP_COOKIE_VARS(strName)      => $_COOKIE[strName] / $HTTP_COOKIE_VARS[] ... not supported yet
*--------------------------------------------------------------------------
* CHP APIs for access of the Configuration Space. The INTEGER values are just strings here.
*   CHP_GET_CONFIG(key)           => get the (string) value from Config, exact match
*   CHP_WGET_CONFIG(key,val,name) => get the (string) value from Config, wildcard match
*   CHP_SET_CONFIG(key,val)       => set the (string) value to   Config
*--------------------------------------------------------------------------------------------------------
*/
#ifndef __AMN_CHP_EXEC_ENGINE_DEFINITIONS__
#define __AMN_CHP_EXEC_ENGINE_DEFINITIONS__

#include "inc/amn_cgi.h"

typedef int (*chp_wread_cback_t)( struct cgi_virt_file_t *f );
typedef int (*chp_getinfo_cback_t)( struct cgi_virt_file_t *f );

//*******************************************************************************************************
#ifdef __AMN_OS_LINUX__
#define CHP_PRINTF(fmt, ...)   CGI_PRINTF( _chp_fp_->web_rwbuf, _chp_fp_->web_rwptr, _chp_fp_->web_rwlen, fmt, ##__VA_ARGS__ )
#else
#define CHP_PRINTF(...)        CGI_PRINTF( _chp_fp_->web_rwbuf, _chp_fp_->web_rwptr, _chp_fp_->web_rwlen, __VA_ARGS__ )
#endif

// $_SERVER['PHP_SELF']: The filename of the currently executing script, relative to the document root.
//                       For instance, $_SERVER['PHP_SELF'] in a script at the address
//                       http://example.com/test.php/foo.bar would be /test.php/foo.bar.
#define CHP_SERVER_SELF             _chp_fp_->hand->cgi_filePath

#define CHP_REMOTE_ADDR             conn_foreign_addr(_chp_fp_->resp)
#define CHP_REMOTE_PORT             conn_foreign_port(_chp_fp_->resp)

// Return the HTTP Header value for the given name. Example,
//   CHP_HTTP_HEAD("Host") => $_SERVER['HTTP_HOST'], gives the client's host name or IP address
//   CHP_HTTP_HEAD("User-Agent") => $_SERVER['HTTP_USER_AGENT'], denotes the user agent accessing the page.
//   CHP_HTTP_HEAD("Accept-Language") => $_SERVER['HTTP_ACCEPT_LANGUAGE'], such as 'en'
#define CHP_HTTP_HEAD(hdrName)    httpmsg_find_hdr_str( _chp_fp_->req, hdrName ) ->value.buf

// CHP_GET_VARS(strName) => $_GET[strName] / $HTTP_GET_VARS[]
static INLINE char* __CHP_GET_VARS(struct cgi_virt_file_t *f, char *strName)
{
	struct cgi_param_t *prm;
	prm = cgi_param_nget( f, strName );
	if (!prm)  return NULL;
	else       return prm->value;
}
#define CHP_GET_VARS(strName)       __CHP_GET_VARS(_chp_fp_, strName)

// CHP_GET_ARRAYS(arrayName,i) => $_GET[arrayName], for example: attributes[i]
static INLINE char* __CHP_GET_ARRAYS(struct cgi_virt_file_t *f, char *arrayName, int idx)
{
	struct cgi_param_t *prm;
	prm = cgi_param_nget2( f, arrayName, idx );
	if (!prm)  return NULL;
	else       return prm->value;
}
#define CHP_GET_ARRAYS(arrayName,i)   __CHP_GET_ARRAYS(_chp_fp_, arrayName,i)

// CHP_IN_ARRAY(val,arrayName) => if (in_array($val, $arrayName)) { ... }
static INLINE int __CHP_IN_ARRAY(struct cgi_virt_file_t *f, char *val, char *arrayName)
{
	int i;
	struct cgi_param_t *prm;
	for (i=0; i<1000; i++) {
		prm = cgi_param_nget2( f, arrayName, i );
		if (!prm)
			return 0;  // FALSE
		if (!strcasecmp(prm->value,val))
			return 1;  // TRUE
	}
	return 0;  // FALSE
}
#define CHP_IN_ARRAY(val,arrayName)   __CHP_IN_ARRAY(_chp_fp_, val,arrayName)

#define CHP_POST_VARS(strName)        // => $_POST[strName] / $HTTP_POST_VARS[] ... not supported yet

// => $_COOKIE[strName] / $HTTP_COOKIE_VARS[] ... not supported yet
#define CHP_COOKIE_VARS(var)            http_get_cookie( _chp_fp_, var )
#define CHP_SET_COOKIE(var,val,age)     chp_set_cookie( _chp_fp_, var, val, age )

#define CHP_GET_CONFIG(key)             chp_get_config(key)
#define CHP_WGET_CONFIG(key,name)       chp_wget_config(key,&(name))
#define CHP_SET_CONFIG(key,val)         amn_currConfig_set(key,val)

//*******************************************************************************************************
struct chp_handler_t {
	struct list_head   link;
	chp_wread_cback_t  wread;
	chp_getinfo_cback_t  getinfo;
	struct cgi_handler_t *cgi;
	struct cgi_virt_file_t *curr_file;
};

//*******************************************************************************************************
struct chp_handler_t *register_CHP_servlets( char *filePath, chp_wread_cback_t wread, chp_getinfo_cback_t getinfo );
char *chp_get_config( char *key );
char *chp_wget_config( char *key, char **cfg_name );
void chp_set_cookie( struct cgi_virt_file_t *f, char *var, char *val, int age );

#endif
