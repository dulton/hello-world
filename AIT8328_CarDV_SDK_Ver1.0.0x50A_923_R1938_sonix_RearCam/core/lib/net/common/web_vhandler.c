/*
*********************************************************************************************************
*                         AIT Multimedia Network Streaming Server
*                     (c) Copyright 2012, Alpha Imaging Technology Corp.
*               All rights reserved. Protected by international copyright laws.
*********************************************************************************************************
*
* Description: This handles the VirtualDirCallbacks that come from the web server.
*
*/

#include <string.h>

#define STRUCT_SESSION_PROPERTY   struct cookie_session_t cookie
#include "inc/amn_cgi.h"
#include "mmp_lib.h"

#ifndef VWEBDBG_LEVEL
// NOTICE for MJPEG developer: for staticMJPEG, set VWEBDBG_LEVEL 1, for liveMJPEG, set 2
#define VWEBDBG_LEVEL          1   // should be 0 for production release
#endif

#if VWEBDBG_LEVEL == 1
#define DBGL_OPENCLOSE         DBGL_DEBUG
#define DBGL_READWRITE         DBGL_DEBUG
#define DBGL_CGIPARAM1         DBGL_VERBOSE
#define DBGL_CGIPARAM2         DBGL_DEBUG
#define DBGL_SESSION           DBGL_VERBOSE
#elif VWEBDBG_LEVEL == 2
#define DBGL_OPENCLOSE         DBGL_VERBOSE
#define DBGL_READWRITE         DBGL_DEBUG
#define DBGL_CGIPARAM1         DBGL_VERBOSE
#define DBGL_CGIPARAM2         DBGL_DEBUG
#define DBGL_SESSION           DBGL_VERBOSE
#elif VWEBDBG_LEVEL == 3
#define DBGL_OPENCLOSE         DBGL_VERBOSE
#define DBGL_READWRITE         DBGL_VERBOSE
#define DBGL_CGIPARAM1         DBGL_VERBOSE
#define DBGL_CGIPARAM2         DBGL_VERBOSE
#define DBGL_SESSION           DBGL_VERBOSE
#else
#define DBGL_OPENCLOSE         DBGL_DEBUG
#define DBGL_READWRITE         DBGL_DEBUG
#define DBGL_CGIPARAM1         DBGL_DEBUG
#define DBGL_CGIPARAM2         DBGL_DEBUG
#define DBGL_SESSION           DBGL_DEBUG
#endif

extern struct VirtualDirCallbacks virtualDirCallback;

/*! a local dir which serves as webserver root */
extern membuffer gDocumentRootDir;

/*! Global variable to denote the state of Upnp SDK == 0 if uninitialized,
 * == 1 if initialized. */

LIST_HEAD(cgi_handler_list);


//*******************************************************************************************************
struct cgi_param_t *cgi_param_iget(IN struct cgi_virt_file_t *f, IN int index)
{
	static int last_index = 0;
	struct list_head *l;
	struct cgi_param_t *prm;
	int i, j;

	j = index;
	if (index < 0)   // traverse all
		j = last_index;

	i = 0;
	list_for_each( l, &f->param_list ) {
		prm = list_entry( l, struct cgi_param_t, link );
		if (i++ == j) { // got it
			if (index < 0)   last_index = i;
			return prm;
		}
	}

	last_index = 0;
	return NULL;
}

struct cgi_param_t *cgi_param_nget(IN struct cgi_virt_file_t *f, IN char *name)
{
	struct list_head *l;
	struct cgi_param_t *prm;

	list_for_each( l, &f->param_list ) {
		prm = list_entry( l, struct cgi_param_t, link );
		if ( !strcasecmp( name, prm->name ) )
			return prm;
	}
	return NULL;
}

// <PATH>?attributes%5B%5D=perky&attributes%5B%5D=thinking&attributes%5B%5D=feeling&s=Record+my+personality%21
// ==> attributes[] = "perky", attributes[] = "thinking", attributes[] = "feeling"
struct cgi_param_t *cgi_param_nget2(IN struct cgi_virt_file_t *f, IN char *name, IN int idx)
{
	struct list_head *l;
	struct cgi_param_t *prm;
	int i = 0;

	list_for_each( l, &f->param_list ) {
		prm = list_entry( l, struct cgi_param_t, link );
		if ( !strcasecmp( name, prm->name ) ) {
			if (i++ == idx)  return prm;
		}
	}
	return NULL;
}

static void construct_cgi_params(IN struct cgi_virt_file_t *f, IN struct cgi_handler_t *h, char *post_parm)
{
	int m, n, i, jv;
	char *p1, *p2, *pL;
	struct cgi_param_t *prm;

	if (f->param_buf)
		return;  // already constructed

	// parsing the param in URL, like: /cgi-bin/Config.cgi?a=1%20%291&bb=22%3c%3e%7b%7d%5c22&ccc=333%2a%2b%2C%2D%2e%2f3333&dddd=44444
	if (!post_parm) {
		post_parm = f->req_url + h->len_cgifile;
		if (*post_parm != '?') {
			char	*p;
			p = strchr(post_parm, '?');
			f->cgi_html_file[0] = '\0';
			if (p) {
				*p = '\0';
				strncpy(f->cgi_html_file, post_parm, MAX_CGI_PATHNAME_SZ - 1);
				*p = '?';
				post_parm = p;
				osal_dprint(CGIPARAM1, "CGI HTML FILE %s", f->cgi_html_file);
			} else {
				return;  // empty params
			}
		}
		post_parm++;
	}

	// counting the numbers of param, and calculate the required param buffer size
	for (n=1, p1=post_parm; *p1; p1++)
		if (*p1 == '&') n++;
	f->n_params = n;
	f->i_param  = 0;
	m = n * (sizeof(struct cgi_param_t) + 2 * sizeof(void*)) + strlen(post_parm);
	osal_dprint(CGIPARAM1, "WEBCGI: param:%s, N=%d, SZ=%d", post_parm, n, m);

	pL = post_parm + strlen(post_parm);
	p1 = post_parm - 1;
	p2 = f->param_buf = osal_malloc( m );
	for (i=0; i<n && p1<pL; i++) {
		prm = (struct cgi_param_t *) p2;
		INIT_LIST_HEAD( &prm->link );
		list_add_tail( &prm->link, &f->param_list );

		// searching '=', for param NAME:
		prm->name = p2 + sizeof(struct cgi_param_t);
		post_parm = ++p1;
		while (*p1) {
			if (*p1 == '=' || *p1 == '&') break;
			p1++;
		}
		if (*p1 == '=') {
			m = p1 - post_parm;
			strncpy( prm->name, post_parm, m );
		} else {
			// is '&' or null
			p1 = post_parm - 1;
			m = 0;
		}
		prm->name[m] = 0;
		p2 = prm->name + (m + sizeof(void*)) / sizeof(void*) * sizeof(void*);  // align to POINTER boundary
		// searching '&', for param VALUE:
		prm->value = p2;
		post_parm = ++p1;
		for (jv=0; (*p1 && *p1 != '&'); p1++) {
			if ((*p1 == '%') && p1[1] >= '2' && p1[1] <= '7')  { // handling of HTML escaping: %2f: '/'  %20: ' '
				m = (p1[1] - '0') << 4;
				p1 += 2;
				if (*p1 <= '9')       prm->value[jv++] = m + (*p1++ - '0');
				else if (*p1 >= 'a')  prm->value[jv++] = m + (*p1++ - 'a') + 10;
				else if (*p1 >= 'A')  prm->value[jv++] = m + (*p1++ - 'A') + 10;
			}
			else
				prm->value[jv++] = *p1;
		}
		prm->value[jv] = 0;
		m = p1 - post_parm;
		p2 = prm->value + (m + sizeof(void*)) / sizeof(void*) * sizeof(void*);  // align to POINTER boundary

		osal_dprint(CGIPARAM2, "WEBCGI: param[%d].name = '%s', .value = '%s' ", i, prm->name, prm->value);
	}
}


//*******************************************************************************************************
char *http_get_cookie(struct cgi_virt_file_t *f, char *name)
{
	int  n;
	char *v, *t;
	char *buf = amn_get_parsebuf();
	http_header_t *cookie;

	cookie = httpmsg_find_hdr_str( f->req, "Cookie" );
	if (!cookie) {
		osal_dprint(VERBOSE, "WEBCGI: no Cookie in HTTP Header");
		return NULL;
	}
	strncpy( buf, cookie->value.buf, PARSEBUF_LEN );
	n = strlen( name );
	v = strstr( buf, name );
	if (v && v[n] == '=') {
		// We've found the pattern: name=   and will proceed with its VALUE part
		v += n+1;
		t = strchr(v, ';');
		if (t)  *t = 0;    // terminate at the seperator position ';'
		osal_dprint(CGIPARAM2, "WEBCGI: cookie %s=%s", name, v);
		return v;
	}
	return NULL;
}

char *http_set_cookie(struct cgi_virt_file_t *f, char *name, char *val, int age)
{
	char *buf = amn_get_parsebuf();
	if (age >= 0)
		snprintf( buf, PARSEBUF_LEN, "Set-Cookie: %s=%s; Max-Age=%d\r\n", name, val, age );
	else
		snprintf( buf, PARSEBUF_LEN, "Set-Cookie: %s=%s; Max-Age=%d\r\n", name, val,
				amn_currConfig_get("Sys.Session.HTTP.Age")->v.intVal );
	return buf;
}

char *__set_AIT_cookie_header( struct cgi_virt_file_t *f )
{
	char *extra_header;
	extra_header = http_set_cookie( f, AIT_COOKIE_SESS_NAME, f->sess->id, -1 );
	strncat( extra_header, EXHEAD__CACHE_CTRL(2), PARSEBUF_LEN );
	return extra_header;
}

char *http_create_session( struct cgi_virt_file_t *f )
{
	int age = amn_currConfig_get("Sys.Session.HTTP.Age")->v.intVal;
	struct cookie_session_t *cookie_sess;

	// WDOG Timer: around age/2 seconds
	f->sess = session_new( f->hand, age * 512, NULL, http_kill_session );
	f->sess->data = f;
	cookie_sess = cast_protocol_data(f->sess, cookie_session_t);
	memcpy( &cookie_sess->foreign_sockaddr, &f->resp->foreign_sockaddr,
			sizeof(struct sockaddr_storage) );

	osal_dprint(SESSION, "WEBCGI: New Session -- IP %s  SessID %s",
				inet_ntoa( conn_foreign_addr(f->resp) ), f->sess->id);
	return __set_AIT_cookie_header( f );
}

char *http_load_session( struct cgi_virt_file_t *f )
{
	char *ssid;
	ssid = http_get_cookie( f, AIT_COOKIE_SESS_NAME );
	if (ssid) {
		f->sess = session_search_id( ssid );
		if (f->sess) {
			struct cookie_session_t *cookie_sess;
			f->sess->data = f;
			cookie_sess = cast_protocol_data(f->sess, cookie_session_t);
			if (conn_foreign_addr(f->resp).s_addr == ((struct sockaddr_in*)(&cookie_sess->foreign_sockaddr))->sin_addr.s_addr)
				return __set_AIT_cookie_header( f ); // success

			osal_dprint(SESSION, "WEBCGI: AIT Session Cookie might be attacked by %s, expect from %s !!!",
					inet_ntoa( conn_foreign_addr(f->resp) ),
					inet_ntoa( ((struct sockaddr_in*)(&cookie_sess->foreign_sockaddr))->sin_addr ));
		}
	}
	osal_dprint(VERBOSE, "WEBCGI: AIT Session Cookie failed");
	return NULL;
}

int http_kill_session( struct amn_session_t *sess, char *reason )
{
	struct cookie_session_t *cookie_sess = cast_protocol_data(sess, cookie_session_t);
	if (reason) {
		// by manually LOGOUT or TEARDOWN the session
		struct cgi_virt_file_t *f = sess->data;
		if (!f || f->sess != sess) {
			osal_dprint(WARN, "WEBCGI: Session inconsistency !! ID=%s", sess->id);
			return -1;
		}
		else
			osal_dprint(SESSION, "WEBCGI: Kill Session -- ID %s  IP %s", f->sess->id,
				inet_ntoa( ((struct sockaddr_in*)(&cookie_sess->foreign_sockaddr))->sin_addr ));
		f->sess = NULL;
	}

	return 0;
}



//*******************************************************************************************************
struct cgi_virt_file_t *new_virt_filehandle( struct cgi_handler_t *h, const char *filename )
{
	int n;
	struct cgi_virt_file_t *f;

	if (strncasecmp( filename, h->cgi_filePath, h->len_cgifile )) {
		osal_dprint(INFO, "WEBCGI: %s doesn't match %s", filename, h->cgi_filePath);
		return NULL;
	}
	f = osal_zmalloc( sizeof(struct cgi_virt_file_t) );
	if (f) {
		f->hand = h;
		h->curr_file = f;

		n = strlen( filename );
		if (n > MAX_CGI_REQ_URL_SZ-1) {
			osal_dprint(INFO, "WEBCGI: %s too long !! Exceeding %d", filename, MAX_CGI_REQ_URL_SZ-1);
			n = MAX_CGI_REQ_URL_SZ-1;
		}
		strncpy( f->req_url, filename, n );

		INIT_LIST_HEAD( &f->param_list );
		construct_cgi_params( f, h, NULL );
	}
	return f;
}

void destroy_virt_filehandle( struct cgi_virt_file_t *f )
{
	if (f->param_buf)
		osal_free(f->param_buf);

	f->hand->curr_file = NULL;
	osal_free(f);
}

//*******************************************************************************************************
// For HTTP-GET,  the call sequences are: get_info / open(UPNP_READ) / read ... close
// For HTTP-POST, the call sequences are: open(UPNP_WRITE) / write / ... close


/// \brief Query information on a file.
/// \param filename Name of the file to query.
/// \param File_Info Pointer to the struction that stores information about
/// the file.
///
/// This function corresponds to get_info from the UpnpVirtualDirCallbacks 
/// structure. It is called by the web server to query information about a
/// file. To perform the operation an appropriate request handler is 
/// created, it then takes care of filling in the data.
///
/// \return 0 Success.
/// \return -1 Error.
static int vdir_handle_get_info(IN const char *filename, IN void *req, OUT char **extra_head, OUT struct File_Info *info, int *extra_func)
{
	enum vhand_authen_spec_t ret;
	struct cgi_virt_file_t *f;
	struct cgi_handler_t *h = find_vdir_handlers(filename);
	
	if (!h) {
		osal_dprint(INFO, "WEBCGI: %s has not been registered !!!", filename); \
		return -1;
	}
	if (h->curr_file)  return -1; /* opened and processing */
	f = new_virt_filehandle( h, filename );
	if (!f)
		return -1;
	f->req  = req;
	f->resp = f->req->enclose_obj;   // in particular, allow to access SOCKET addr info

	if (!h->cb.get_info) {
		osal_dprint(INFO, "WEBCGI: NULL get_info for CGI:%s", filename);
		return -1;
	}

	ret = h->cb.get_info( h, filename, extra_head, info);
	switch(ret) {
	case VHAND_OK__NO_CHECK_AUTH:
	case VHAND_OK__AUTHORIZED:
	case VHAND_OK__AUTHENTICATOR:
		break;

	case VHAND_ERR__NO_CHECK_AUTH:
	case VHAND_ERR__NO_AUTHORITY:
	case VHAND_ERR__ILLEGAL_URL_PARAM:
	case VHAND_ERR__FAIL_AUTHENTICATION:
	case VHAND_ERR__REQUIRE_AUTHENTICATION:
	case VHAND_ERR__OUT_OF_SERVICE:
		osal_dprint(INFO, "WEBCGI: file %s,  get_Info error: %d ", filename, ret);
		destroy_virt_filehandle( f );
		h->curr_file = NULL;
		return ret;
	}

	if (*extra_head == NULL)
		*extra_head = EXHEAD__CACHE_CTRL(2);  // prevent CLIENT from caching the return messages

	osal_dprint(OPENCLOSE, "WEBCGI: file %s,  Info: len=%d mod=%d dir=%d type=%s ", filename,
			(int)info->file_length, (int)info->last_modified, info->is_directory, info->content_type);
	return 0;
}

/// \brief Opens a file for the web server.
/// \param filename Name of the file to open (or actually the full URL)
/// \param mode in which the file will be opened (we only support UPNP_READ)
/// \param File_Info Pointer to the struction that stores information about
/// the file.
///
/// This function is called by the web server when it needs to open a file.
/// It first calls create_request_handler() which tries to identify the
/// request and returns us the appropriate IOHandler.
/// Note, that we return values here, because the SDK does not work with
/// exceptions.
///
/// \return UpnpWebFileHandle A valid file handle.
/// \return NULL Error.
static UpnpWebFileHandle vdir_handle_open(IN const char *filename, IN enum UpnpOpenFileMode mode)
{
	struct cgi_virt_file_t *f; \
	struct cgi_handler_t *h = find_vdir_handlers(filename);
	
	if (!h) {
		osal_dprint(INFO, "WEBCGI: %s has not been registered !!!", filename);
		return NULL;
	}
	if (!h->curr_file) {
		osal_dprint(INFO, "WEBCGI: %s didn't call get_info yet!!!", filename);
		return NULL;
	}
	f = h->curr_file;

	f->mode = mode;
	if (h->cb.open && h->cb.open( f ) < 0)
		return NULL;

	osal_dprint(OPENCLOSE, "WEBCGI: file %s,  opening mode %d ", filename, mode);
	return f;
}

/// \brief Reads a previously opened file sequentially.
/// \param f IOHandler that takes care of this request.
/// \param buf This buffer will be filled by our read functions.
/// \param length Number of bytes to read.
///
/// This function is called by the web server to perform a sequential
/// read from an open file. It calls the read function of the 
/// appropriate IOHandler, which copies \b length bytes from the file
/// or memory into the output buffer.
///
/// \return 0   EOF encountered.
/// \return -1  Error.
static int vdir_handle_read(IN UpnpWebFileHandle fp, OUT char *buf, IN size_t length)
{
	int ret=0;
	struct cgi_virt_file_t *f = fp;
	struct cgi_handler_t   *h = f->hand;

	f->web_rwbuf = buf;
	f->web_rwlen = length;
	if (h->cb.read) ret = h->cb.read( f );
	osal_dprint(READWRITE, "WEBCGI: file %s,  reading %d (%d) bytes, off=%d", f->hand->cgi_filePath, ret, (int)length, f->fread_ptr);
	return ret;
}

/// \brief Writes to a previously opened file sequentially.
/// \param f Handle of the file.
/// \param buf This buffer will be filled by fwrite.
/// \param length Number of bytes to fwrite.
///
/// This function is called by the web server to perform a sequential
/// write to an open file. It copies \b length bytes into the file
/// from the buffer. It should return the actual number of bytes
/// written, in case of a write error this might be less than
/// \b length.
///
/// \return Actual number of bytes written.
///
/// \warning Currently this function is not supported.
static int vdir_handle_write(IN UpnpWebFileHandle fp, IN char *buf, IN size_t length)
{
	struct cgi_virt_file_t *f = fp;
	struct cgi_handler_t   *h = f->hand;

	osal_dprint(READWRITE, "WEBCGI: file %s,  writing %d bytes: '%s' ", f->hand->cgi_filePath, (int)length, buf);
	construct_cgi_params( f, h, buf );

	f->web_rwbuf = buf;
	f->web_rwlen = length;
	if (h->cb.write) return h->cb.write( f );
	return 0;
}

/// \brief Performs a seek on an open file.
/// \param f Handle of the file.
/// \param offset Number of bytes to move in the file. For seeking forwards
/// positive values are used, for seeking backwards - negative. \b Offset must
/// be positive if \b origin is set to \b SEEK_SET
/// \param whence The position to move relative to. SEEK_CUR to move relative
/// to current position, SEEK_END to move relative to the end of file,
/// SEEK_SET to specify an absolute offset.
///
/// This function is called by the web server to perform seek on an a file.
/// The seek operation itself is performed by the responsible IOHandler.
///
/// \return 0 On success, non-zero value on error.
static int vdir_handle_seek(IN UpnpWebFileHandle fp, IN off_t offset, IN int whence)
{
	struct cgi_virt_file_t *f = fp;
	struct cgi_handler_t   *h = f->hand;

	osal_dprint(OPENCLOSE, "WEBCGI: file %s,  seek offset=%d  whence=%d ", f->hand->cgi_filePath, (int)offset, whence);
	if (h->cb.seek) return h->cb.seek( f, offset, whence );
	return 0;
}

/// \brief Closes a previously opened file.
/// \param f IOHandler for that file.
/// 
/// Same as fclose()
///
/// \return 0 On success, non-zero on error.
static int vdir_handle_close( IN UpnpWebFileHandle fp)
{
	int ret = 0;
	struct cgi_virt_file_t *f = fp;
	struct cgi_handler_t   *h = f->hand;

	osal_dprint(OPENCLOSE, "WEBCGI: file %s closing... ", f->hand->cgi_filePath);
	if (h->cb.close) ret = h->cb.close( f );
	destroy_virt_filehandle( f );
	return ret;
}


//*******************************************************************************************************
int prepare_HTTP_response(int method, struct resp_message_t *resp)
{
	//-----------------------------------------------------------//
	// performs: sort of constructor for struct resp_message_t
	//-----------------------------------------------------------//
	memptr hdr_value;
	int		allow;
	
	allow = check_working_rtsp_session(inet_ntoa(((struct sockaddr_in*)(&resp->foreign_sockaddr))->sin_addr));
	if (allow == 0)
		return HTTP_SERVICE_UNAVAILABLE;
	switch (method) {
	/* HTTP server call */
	case HTTPMETHOD_GET:
	case HTTPMETHOD_POST:
	case HTTPMETHOD_HEAD:
	case HTTPMETHOD_SIMPLEGET:
		break;

	/* Soap Call */
	case SOAPMETHOD_POST:
	case HTTPMETHOD_MPOST:
		// callback = soap_device_callback;  // TO-DO in UPnP
		return HTTP_INTERNAL_SERVER_ERROR;

	/* Gena Call */
	case HTTPMETHOD_NOTIFY:
	case HTTPMETHOD_SUBSCRIBE:
	case HTTPMETHOD_UNSUBSCRIBE:
		osal_dprint( INFO, "miniserver %d: got GENA msg\n", resp->info.socket);
		// callback = genaCallback;  // TO-DO in UPnP
		return HTTP_INTERNAL_SERVER_ERROR;

	default:
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	
	resp->persistant = 0;    // HTTP is stateless
	if (httpmsg_find_hdr(&resp->parser.msg, HDR_CONNECTION, &hdr_value) != NULL) {
		if (strncasecmp(hdr_value.buf, "keep-alive", hdr_value.length) == 0) {
			resp->persistant = 1;
		}
	}
	prepare_response( resp );

	/*Initialize instruction header. */
#if 0
	resp->RespInstr.IsVirtualFile = 0;
	resp->RespInstr.IsChunkActive = 0;
	resp->RespInstr.IsRangeActive = 0;
	resp->RespInstr.IsTrailers = 0;
	memset(resp->RespInstr.AcceptLanguageHeader, 0, sizeof(resp->RespInstr.AcceptLanguageHeader));
#endif
	return HTTP_CONTINUE;
}

//*******************************************************************************************************
struct cgi_handler_t *find_vdir_handlers( const char *filePath )
{
	struct list_head *l;
	struct cgi_handler_t *h;

	list_for_each( l, &cgi_handler_list ) {
		h = list_entry( l, struct cgi_handler_t, link );
		if ( !strncasecmp( h->cgi_filePath, filePath, h->len_cgifile) ) {
			return h;
		}
	}
	return NULL;
}

struct cgi_handler_t *register_vdir_handlers( char *filePath, struct cgi_file_operations_t *cb )
{
	int ret;
	struct cgi_handler_t *h;

	h = find_vdir_handlers(filePath);
	if (h) {
		osal_dprint(INFO, "WEBCGI: %s already registered !!!", filePath);
		return h;
	}

	h = osal_zmalloc( sizeof(struct cgi_handler_t) );
	if (h) {
		strncpy( h->cgi_filePath, filePath, MAX_CGI_PATHNAME_SZ-1 );
		h->len_cgifile  = strlen( h->cgi_filePath );

		ret = UpnpAddVirtualDir( h->cgi_filePath );
		if (ret != UPNP_E_SUCCESS) {
			osal_dprint(WARN, "WEBCGI UpnpAddVirtualDir failed for Directory %s, err=%d", filePath, ret);
			osal_free( h );
			return NULL;
		}

		if (cb)  memcpy( (void*)&h->cb, (void*)cb, sizeof(struct VirtualDirCallbacks) );

		INIT_LIST_HEAD( &h->link );
		list_add_tail( &h->link, &cgi_handler_list );
	}
	return h;
}

void init_web_virt_handlers()
{
	int ret;
	char *RootDir;

	virtualDirCallback.get_info = vdir_handle_get_info;
	virtualDirCallback.open = vdir_handle_open;
	virtualDirCallback.read = vdir_handle_read;
	virtualDirCallback.write = vdir_handle_write;
	virtualDirCallback.seek = vdir_handle_seek;
	virtualDirCallback.close = vdir_handle_close;

	// LIBUPNP::upnpapi.c::UpnpInit()
	/* Set the UpnpSdkInit flag to 1 to indicate we're successfully initialized. */
#ifndef	_ENABLE_UPNP_
{
	extern int UpnpSdkInit;
	UpnpSdkInit = 1;
}
#endif
	RootDir = amn_currConfig_get("Sys.Dir.WWW")->v.strVal;
	ret = UpnpSetWebServerRootDir( RootDir );
	if (ret != UPNP_E_SUCCESS) {
		osal_dprint(WARN, "upnp_init: UpnpSetWebServerRootDir failed for Directory %s, err=%d", RootDir, ret);
	}
	else
		osal_dprint(VERBOSE, "upnp_init: UpnpSetWebServerRootDir for Directory %s", RootDir);

	// initialize various CGI handlers
	init_cgi_handlers();
}   

