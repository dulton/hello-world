/*
*********************************************************************************************************
*                         AIT Multimedia Network -- HTTP Server
*                     (c) Copyright 2012, Alpha Imaging Technology Corp.
*               All rights reserved. Protected by international copyright laws.
*********************************************************************************************************
*
* Description: 
*
*/
#include "inc/amn_cgi.h"
#include "inc/auto_syscfg.h"
#include "inc/amn_module.h"
#include "netapp.h"
#include "mmpf_fs_api.h"
#include "AHC_UF.h"
#include "AHC_General.h"

int dcim_filter_fmt(void *hinst, char *inout, void *attr, void *arg);

void* FindFirstFile(char *inout, int bufsize, int *retsize, FFFILTER_FN* pFilter);
void FindFileClose(void *hinst);
int FindNextFile(void *hinst, char *out, int bufsize, int *retsize, FFFILTER_FN* pFilter);
int FindFileAttr(void *hinst, char *in, unsigned int* size, char** fmt, int *rd, FS_FILETIME *ft);
int FindFileGetPosition(void *hinst);
void *FindFileSetPosition(void *hinst, int pos, FFFILTER_FN* pFilter);
typedef struct {
	FS_DIR		*pdir;
	FS_DIRENT	*pent;
	int			pos;
} DIRPAIR;
//
//
#define amn_webConfig_wget(key,cfg_val, cfg_name)	\
		do { \
			int i = web_config_wget(key, &cfg_val, &cfg_name); \
			if (i >= 0) { \
				cfg_name = amn_ConfigSpaceDict[i]; \
				cfg_val  = amn_config_get( CONFIG_SPACE_TYPE_CURRENT, cfg_name ); \
			} \
} while (0)

int cgi_dir(IN struct cgi_virt_file_t *f, OUT char *buf, OUT char **endp, IN int maxlen);
int cgi_delete_file(IN struct cgi_virt_file_t *f, OUT char *buf, OUT char **endp, IN int maxlen);
int cgi_play_file(IN struct cgi_virt_file_t *f, OUT char *buf, OUT char **endp, IN int maxlen);
int cgi_switch_camera(IN struct cgi_virt_file_t *f, OUT char *buf, OUT char **endp, IN int maxlen);
extern AHC_BOOL m_ubPlaybackRearCam;


//*******************************************************************************************************
// when readbuf is NULL, this routine will work in dry-run: facilitate the CONTENT-LENGTH calculation
int cgi_snprintf(OUT char *readbuf, IN int size, char *fmt, ...)
{
	int n;
	char *strbuf;
	va_list args;

	va_start(args, fmt);
	if (readbuf) {
		n = vsnprintf(readbuf, size + 1, fmt, args);
	}
	else {
		strbuf = osal_malloc(size);
		n = vsnprintf(strbuf, size + 1, fmt, args);
		osal_free(strbuf);
	}
	va_end(args);

	return n;
}

char *cgi_err_text(int err_code)
{
	switch(err_code) {
	case CGI_ERR__OK:
		return "OK";
	case CGI_ERR__SYS_BUSY:
		return "System Busy";
	case CGI_ERR__ILL_PARAM:
		return "Illegal Parameter";
	case CGI_ERR__UNKNOWN_ACTION:
		return "Unknown CGI-PARAM: Action";
	case CGI_ERR__UNKNOWN_PROPERTY:
		return "Unknown CGI-PARAM: Property";
	case CGI_ERR__UNKNOWN_PARAM:
		return "Unknown CGI-PARAM: extra parameter";
	case CGI_ERR__MISS_PARAM:
		return "Missing CGI-PARAM";
	case CGI_ERR__INTERNAL_ERROR:
		return "Internal Error";
	case CGI_ERR__ILL_USER:
		return "Illegal User";
	case CGI_ERR__ERR_PASS:
		return "Error Password";
	case CGI_ERR__AUTHEN_FAIL:
		return "Authentication Failed";
	case CGI_ERR__NO_AUTHOR:
		return "User no authority for the requested operation";
	case CGI_ERR__REQUIRE_AUTHEN:
		return "Require authentication for the requested operation";
	}
	return "???";
}

void cgi_action_prepare( struct cgi_virt_file_t *f )
{
	struct cgi_param_t *prmAction;

	if (!f->param_buf) {
		cgi_set_state( f, CGI_ACTSTATE__NOT_READY );
		return;
	}
	else
		cgi_set_state( f, CGI_ACTSTATE__START );    // can proceed and will track the reading of N'th Config Name

	prmAction = cgi_param_nget( f, "action" );
	if (!prmAction) {
		cgi_set_error( f, CGI_ERR__ILL_PARAM );
		return;
	}

	if      ( !strcasecmp( prmAction->value, "get" ) )        cgi_set_action( f, CGI_ACTION_GET      );
	else if ( !strcasecmp( prmAction->value, "set" ) )        cgi_set_action( f, CGI_ACTION_SET      );
	else if ( !strcasecmp( prmAction->value, "add" ) )        cgi_set_action( f, CGI_ACTION_ADD      );
	else if ( !strcasecmp( prmAction->value, "del" ) )        cgi_set_action( f, CGI_ACTION_DEL      );
	else if ( !strcasecmp( prmAction->value, "play") )        cgi_set_action( f, CGI_ACTION_PLAY     );
	else if ( !strcasecmp( prmAction->value, "mod" ) )        cgi_set_action( f, CGI_ACTION_MOD      );
	else if ( !strcasecmp( prmAction->value, "dir" ) )        cgi_set_action( f, CGI_ACTION_DIR      );
	else if ( !strcasecmp( prmAction->value, "reardir" ) )    cgi_set_action( f, CGI_ACTION_REARDIR      );
	else if ( !strcasecmp( prmAction->value, "login" ) )      cgi_set_action( f, CGI_ACTION_LOGIN    );
	else if ( !strcasecmp( prmAction->value, "logout" ) )     cgi_set_action( f, CGI_ACTION_LOGOUT   );
	else if ( !strcasecmp( prmAction->value, "switch" ) )     cgi_set_action( f, CGI_ACTION_SWITCH   );
	else if ( !strcasecmp( prmAction->value, "sense" ) )      cgi_set_action( f, CGI_ACTION_SENSE    );
	else if ( !strcasecmp( prmAction->value, "flash" ) )      cgi_set_action( f, CGI_ACTION_FWUPDATE );
	else if ( !strcasecmp( prmAction->value, "log" ) )        cgi_set_action( f, CGI_ACTION_LOG      );
	else if ( !strcasecmp( prmAction->value, "setcamid" ) )        cgi_set_action( f, CGI_ACTION_CAMSWITCH      );
	else                                                      cgi_set_error(  f, CGI_ERR__UNKNOWN_ACTION );
}

void cgi_config_prepare( struct cgi_virt_file_t *f )
{
	struct cgi_param_t *prmProperty;

	cgi_action_prepare(f);
	if (!f->param_buf) return;

	prmProperty = cgi_param_nget( f, "property" );
	if (!prmProperty) {
		cgi_set_error( f, CGI_ERR__ILL_PARAM );
		return;
	}
	strncpy( cgi_get_property(f), prmProperty->value, max( MAX_VIRT_FILE_PRIVATE_BUFSZ, strlen(prmProperty->value) ) );
}

// when buf is NULL, this routine will work in dry-run mode
int cgi_processs_configs(IN struct cgi_virt_file_t *f, OUT char *buf, IN int maxlen)
{
	struct cgi_param_t *prmProperty;
	struct cgi_param_t *prmValue, *prmAttrs;
	struct amn_cfgspace_value_t *cfg_val;
	char *cfg_name;
	char *ptr = buf;
	int  resp_code = CGI_ERR__OK;
	int		ret;

	#define exit_with_response(_res)  do { \
		resp_code = _res; \
		goto L_exit_simple_response; \
	} while (0)
	
	#define	exit_more_config(_res)	do {	\
		resp_code = _res;	\
		goto L_exit_more_response;	\
	} while (0)
	
	#define	exit_http_response(_res) do {	\
		resp_code = _res;	\
		goto L_exit_http_response;	\
	} while (0)
	
	#define	exit_xml_response(_res) do {	\
		resp_code = _res;	\
		goto L_exit_xml_response;	\
	} while (0)
	
	//osal_dprint(VERBOSE, "CGI STATE %d %p", cgi_get_state(f), buf);
	switch(cgi_get_state(f)) {
	case CGI_ACTSTATE__EOF:
		if (!buf)  cgi_set_state( f, CGI_ACTSTATE__START );      // after DRY-RUN, we are ready for REAL-RUN
		return 0;  // EOF

	case CGI_ACTSTATE__NOT_READY:
		return 0;  // no URL's param in the HTTP-GET/POST request

	case CGI_ACTSTATE__START:
		// get the Config Name: the URL-Parameter 'Property'
		prmProperty = cgi_param_nget2( f, "property", cgi_get_param(f) );
		osal_dprint(VERBOSE, "ACTION %d %s", cgi_get_action(f), (prmProperty)? prmProperty->value : "(NULL)");
		if (!prmProperty)
			exit_with_response( CGI_ERR__ILL_PARAM );
		strncpy( cgi_get_property(f), prmProperty->value, max( MAX_VIRT_FILE_PRIVATE_BUFSZ, strlen(prmProperty->value) ) );
		// If Camera is in HDMI not allow CGI command except action is get
		if (cgi_get_action(f) != CGI_ACTION_GET && IS_CAMERA_IN_HDMI()) {
			exit_with_response( CGI_ERR__ );
		}
		switch(cgi_get_action(f)) {
		case CGI_ACTION_SET:
		case CGI_ACTION_ADD:
			cfg_val = amn_webConfig_get( cgi_get_property(f) );
			break;
		case CGI_ACTION_DEL:
			cfg_val = amn_webConfig_get( cgi_get_property(f) );
			if (!cfg_val) {
				// an example of cgi to delete file.
				// cgi-bin/Config.cgi?action=del&property=DCIM$100__DSC$MOV_1163.AVI
				// the value of property not in config file, do deleting file!!
				cfg_val = (struct amn_cfgspace_value_t*)1;	// give a dummy to skip checking at below!!
			}
			break;
		case CGI_ACTION_DIR:
		case CGI_ACTION_REARDIR:
			cfg_val = (struct amn_cfgspace_value_t*)1;	// give a dummy to skip checking at below!!
			break;

		case CGI_ACTION_GET:
			amn_webConfig_wget( cgi_get_property(f), cfg_val, cfg_name);
			break;

		case CGI_ACTION_PLAY:
			// an example of cgi to delete file.
			// cgi-bin/Config.cgi?action=play&property=DCIM$100__DSC$MOV_1163.MOV
			// the value of property not in config file, do deleting file!!
			cfg_val = (struct amn_cfgspace_value_t*)1;	// give a dummy to skip checking at below!!
			break;
		case CGI_ACTION_CAMSWITCH:
			cfg_val = (struct amn_cfgspace_value_t*)1;
			break;
			
		default:
			resp_code = cgi_get_error(f);
			if (resp_code != 0)
				goto L_exit_simple_response;
			exit_with_response( CGI_ERR__INTERNAL_ERROR );
			break;
		}

		// check if the Property exist or not !!
		if (!cfg_val)
			exit_with_response( CGI_ERR__UNKNOWN_PROPERTY );

		// now actual process the CGI action
		switch(cgi_get_action(f)) {
		case CGI_ACTION_SET:
			prmValue = cgi_param_nget2( f, "value", cgi_get_param(f) );
			if (!prmValue)
				exit_with_response( CGI_ERR__UNKNOWN_PARAM );
			
			osal_dprint(VERBOSE, "CONFIG.CGI: State %d", cgi_get_state( f ));
			// Check and Do camera CGI first, if return 0, do AMN config set
			// Normally, return is -1 when the CGI is for camera and processed,
			// if it is -2 means the CGI processed but failed!
			// And WEB should return CGI_ERR__
			ret = web_config_act(cgi_get_property(f), prmValue->value, buf);
			if (ret == 0) {
			 	amn_config_set( CONFIG_SPACE_TYPE_CURRENT, cgi_get_property(f), prmValue->value);
			} else if (ret == -2) {
				exit_with_response( CGI_ERR__ );
			} else if (ret >= CGI_ERR__ILL_PARAM) {
				exit_with_response( ret );
			}
			if (buf) osal_dprint(DEBUG, "CONFIG.CGI: Set %s = %s", cgi_get_property(f), prmValue->value);
			osal_dprint(VERBOSE, "CONFIG.CGI: Set %s = %s", cgi_get_property(f), prmValue->value);
			cgi_inc_param(f);
			prmProperty = cgi_param_nget2( f, "property", cgi_get_param(f) );
			if (prmProperty) {
				exit_more_config( CGI_ERR__OK );
			} else {
				exit_with_response( CGI_ERR__OK );
			}
			break;
		case CGI_ACTION_ADD:
			prmAttrs = cgi_param_nget2( f, "attrs", cgi_get_param(f) );
			if (!prmAttrs)
				exit_with_response( CGI_ERR__UNKNOWN_PARAM );

			if (buf) osal_dprint(DEBUG, "CONFIG.CGI: Add %s = %s", cgi_get_property(f), prmAttrs->value);
			exit_with_response( CGI_ERR__OK );
			break;

		case CGI_ACTION_DEL:
			if (buf) osal_dprint(DEBUG, "CONFIG.CGI: Del %s", cgi_get_property(f));
			exit_with_response(cgi_delete_file(f, buf, &ptr, maxlen));
			break;

		case CGI_ACTION_PLAY:
			if (buf) osal_dprint(DEBUG, "CONFIG.CGI: Play %s", cgi_get_property(f));
			exit_http_response(cgi_play_file(f, buf, &ptr, maxlen));
			break;

		case CGI_ACTION_CAMSWITCH:
			cfg_val = amn_webConfig_get( cgi_get_property(f) );
			if (!cfg_val)
				exit_with_response( CGI_ERR__UNKNOWN_PROPERTY );
			prmValue = cgi_param_nget2( f, "value", cgi_get_param(f) );
			if ((!prmValue)||(strcmp(prmValue->value,"front")&&strcmp(prmValue->value,"rear")))
				exit_with_response( CGI_ERR__UNKNOWN_PARAM );	

			if ((!netapp_getRearcamReady())&&(!strcmp(prmValue->value,"rear")))
				exit_with_response( CGI_ERR__ );
			if (buf) osal_dprint(DEBUG, "CONFIG.CGI: Play %s", cgi_get_property(f));{
				if (!strcmp(cfg_val->v.strVal,prmValue->value)) {
					printc("same camera source\n");
					exit_with_response( CGI_ERR__OK );
				}	
				else
					exit_http_response(cgi_switch_camera(f, buf, &ptr, maxlen));
			}
			break;

		case CGI_ACTION_GET:
			CGI_PRINTF( buf, ptr, maxlen, "0\nOK\n" );
			goto L_process_getcfg;

		case CGI_ACTION_DIR:
            m_ubPlaybackRearCam = AHC_FALSE;
			AHC_UF_SetRearCamPathFlag(AHC_FALSE);
			if (1/*!NetApp_GetRecordStatus()*/)
				exit_xml_response(cgi_dir(f, buf, &ptr, maxlen));
			else
				exit_with_response( CGI_ERR__SYS_BUSY );
		case CGI_ACTION_REARDIR:
            m_ubPlaybackRearCam = AHC_TRUE;
			AHC_UF_SetRearCamPathFlag(AHC_TRUE);
#if (REAR_CAM_TYPE != REAR_CAM_TYPE_NONE)
				exit_xml_response(cgi_dir(f, buf, &ptr, maxlen));
#else
				exit_with_response( CGI_ERR__ );
#endif
		}
		break;

	default:
		amn_webConfig_wget( cgi_get_property(f), cfg_val, cfg_name);
L_process_getcfg:
		cgi_inc_state(f);
		switch(cgi_get_action(f)) {
		case CGI_ACTION_GET:
			if (cfg_val) {
				switch(cfg_val->type) {
				case CFGSPACE_VALTYP__INT:
					CGI_PRINTF( buf, ptr, maxlen, "%s=%d\n", cfg_name, cfg_val->v.intVal );
					if (buf) osal_dprint(DEBUG, "CONFIG.CGI: %s = %d, len=%d", cfg_name, cfg_val->v.intVal, (int)(ptr - buf));
					break;
				case CFGSPACE_VALTYP__STR:
					CGI_PRINTF( buf, ptr, maxlen, "%s=%s\n", cfg_name, cfg_val->v.strVal );
					if (buf) osal_dprint(DEBUG, "CONFIG.CGI: %s = %s, len=%d", cfg_name, cfg_val->v.strVal, (int)(ptr - buf));
					break;
				}
			}
			else {
				cgi_inc_param(f);
				prmProperty = cgi_param_nget2( f, "property", cgi_get_param(f) );
				if (prmProperty) {
					strncpy( cgi_get_property(f), prmProperty->value, max( MAX_VIRT_FILE_PRIVATE_BUFSZ, strlen(prmProperty->value) ) );
					exit_more_config( CGI_ERR__OK );
				} else {
					if (!buf)  cgi_set_state( f, CGI_ACTSTATE__START );      // after DRY-RUN, we are ready for REAL-RUN
					else       cgi_set_state( f, CGI_ACTSTATE__EOF );        // after REAL-RUN, simply disable it
				}
			}
			break;
		default:
			exit_with_response( CGI_ERR__INTERNAL_ERROR );
			break;
		}
		break;
	}

	return ptr - buf;

L_exit_simple_response:
	CGI_PRINTF( buf, ptr, maxlen, "%d\n%s\n", resp_code, cgi_err_text(resp_code) );
	cgi_set_state( f, CGI_ACTSTATE__EOF );
	return ptr - buf;

L_exit_more_response:
	CGI_PRINTF( buf, ptr, maxlen, "%d\n%s\n", resp_code, cgi_err_text(resp_code) );
	return ptr - buf;

L_exit_http_response:
	if (resp_code != CGI_ERR__OK)
		CGI_PRINTF( buf, ptr, maxlen, "%d\n%s\n", resp_code, cgi_err_text(resp_code) );
	cgi_set_state( f, CGI_ACTSTATE__EOF );
	return ptr - buf;
	
L_exit_xml_response:
	cgi_set_state( f, CGI_ACTSTATE__EOF );
	return ptr - buf;
}

/*
 * do some action instead of value set
 * for example property=Net&value=reset
 * return == -1: this function take care the setting
 *        ==  0: this function ignore the setting, caller have to do
 */
#ifdef __AMN_OS_LINUX__
#define pseudo_cmd(c)
#else
void pseudo_cmd(char* pcmd);
#endif
extern int webwait_uiproc(void* handle, void *arg);
extern void webwait_uisignal(void* handle, int result);
int web_config_act(char *key, char *val, char *buf)
{
	int ret = 0;
	if (buf==NULL)
		return -1;

	ret = NetApp_SetKeyevent(key, val, webwait_uiproc, webwait_uisignal);
	ait_printf("%s %d %d\r\n", __func__, __LINE__, ret);		
	return ret;	// tell caller to do something
}

// CGI GET property=Camera.Preview.MJPEG.status.record
static int cameraVideoStatus(char *arg, struct amn_cfgspace_value_t **val)
{
	if(NetApp_GetRecordStatus()) {
		strcpy((*val)->v.strVal,  "Recording");
	} else {		
		strcpy((*val)->v.strVal,  "Standby");
	}
	return -1;
}
// CGI GET property=Camera.Preview.MJPEG.status.mode
static int cameraSensorMode(char *arg, struct amn_cfgspace_value_t **val)
{
	const char	*mode;
	
	mode = netapp_SensorStatus();
	strcpy((*val)->v.strVal, mode);
	return -1;
}
// CGI GET property=Camera.Preview.MJPEG.TimeStamp.[year|month|day|hour|minute|second]
static int cameraTime(char *arg, struct amn_cfgspace_value_t **val)
{
	char	*p;
	int		data;
	
	p = strrchr(arg, '.');
	if (p == NULL) return 0; // should not return 0;
	p++;
	if (strcmp(p, "year") == 0) {
		data = netapp_get_TimeStamp_year();
	} else if (strcmp(p, "month") == 0) {
		data = netapp_get_TimeStamp_month();
	} else if (strcmp(p, "day") == 0) {
		data = netapp_get_TimeStamp_day();
	} else if (strcmp(p, "hour") == 0) {
		data = netapp_get_TimeStamp_hour();
	} else if (strcmp(p, "minute") == 0) {
		data = netapp_get_TimeStamp_minute();
	} else if (strcmp(p, "second") == 0) {
		data = netapp_get_TimeStamp_second();
	} else {
		return 0;
	}
	(*val)->v.intVal = data;
	return -1;
}
// CGI GET property=Camera.Menu.UIMode
static int cameraUIMode(char *arg, struct amn_cfgspace_value_t **val)
{
	//for letv test, old version is only "snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_GetUIState());"
	//temp solution is skip UI state, let device can playback & streaming  
#ifdef __CONFIG_SDK_LETV_CDV01__  //get tmp mode from netconfig for demo
	if (!strcmp((amn_config_get( CONFIG_SPACE_TYPE_CURRENT, "Camera.Menu.UIMode" ))->v.strVal,"")) {
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_GetUIState());
	}
	else
		snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", (amn_config_get( CONFIG_SPACE_TYPE_CURRENT, "Camera.Menu.UIMode" ))->v.strVal);// CCCC
#else
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_GetUIState());
#endif
	return -1;
}
// CGI GET property=Camera.Menu.DefMode
static int cameraDefMode(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_GetDefMode());
	return -1;
}
// CGI GET property=Camera.Menu.VideoRes
static int cameraVideoRes(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_Video_Resolution());
	return -1;
}
// CGI GET property=Camera.Menu.ImageRes
static int cameraImageRes(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_Image_Resolution());
	return -1;
}
// CGI GET property=Camera.Menu.MTD
static int cameraMTD(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_MTD_Level());
	return -1;
}
// CGI GET property=Camera.Menu.AWB
static int cameraAWB(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_WB());
	return -1;
}
// CGI GET property=Camera.Menu.Flicker
static int cameraFlicker(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_Flicker_Hz());
	return -1;
}
// CGI GET property=Camera.Menu.EV
static int cameraEV(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_EV_Level());
	return -1;
}
// CGI GET property=Camera.Menu.PhotoBurst
static int cameraPhotoBurst(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_PhotoBurst());
	return -1;
}
// CGI GET property=Camera.Menu.Timelapse
static int cameraTimelapse(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_Timelapse());
	return -1;
}
// CGI GET property=Camera.Menu.Upsidedown
static int cameraUpsidedown(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_Upsidedown());
	return -1;
}
// CGI GET property=Camera.Menu.SpotMeter
static int cameraSpotMeter(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_SpotMeter());
	return -1;
}
// CGI GET property=Camera.Menu.LoopingVideo
static int cameraLoopingVideo(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_LoopingVideo());
	return -1;
}
// CGI GET property=Camera.Menu.HDR
static int cameraHDR(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_HDR());
	return -1;
}
// CGI GET property=Camera.Menu.Q-SHOT
static int cameraQ_SHOT(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_Q_SHOT());
	return -1;
}
// CGI GET property=Camera.Menu.StatusLights
static int cameraStatusLights(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_StatusLights());
	return -1;
}
// CGI GET property=Camera.Menu.SoundIndicator
static int cameraSoundIndicator(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_SoundIndicator());
	return -1;
}
// CGI GET property=Camera.Menu.PowerSaving
static int cameraPowerSaving(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_PowerSaving());
	return -1;
}
// CGI GET property=Camera.Menu.FWversion
static int cameraFWversion(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_FW_Version());
	return -1;
}
// CGI GET property=Camera.Menu.IsStreaming
static int cameraIsStreaming(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s",
				(netapp_getStreamingMode())? "YES" : "NO");
	return -1;
}
// CGI GET property=Camera.File.Total
static int cameraFileTotal(char *arg, struct amn_cfgspace_value_t **val)
{
	(*val)->v.intVal = 0;
	AHC_UF_GetTotalFileCount((unsigned int*)&(*val)->v.intVal);
	return -1;
}
// CGI GET property=Camera.Menu.PowerOffDelay
static int cameraPowerOffDelay(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_PowerOffDelay());
	return -1;
}
// CGI GET property=Camera.Menu.TVSystem
static int cameraTVSystem(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_TVSystem());
	return -1;
}
// CGI GET property=Camera.Menu.TV
static int cameraTVStatus(char *arg, struct amn_cfgspace_value_t **val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_TVStatus());
	return -1;
}
// CGI Get property=Camera.Menu.SD0
static int cameraSD0(char *arg, struct amn_cfgspace_value_t ** val)
{
	snprintf((*val)->v.strVal, (*val)->max_strlen, "%s", netapp_get_SD0Status());
	return -1;
}
// CGI GET property=Camera.Preview.MJPEG.WarningMSG
static int cameraWarning(char *arg, struct amn_cfgspace_value_t **val)
{
	if (netapp_warning_msg_state())
		if (netapp_get_warning_msg_info())
			strcpy((*val)->v.strVal,  "NO CARD!\n");
	return -1;
}
// CGI GET property=Camera.Record.*
// How does APP calculate the current video recording time?
// In APP, video_recording_time = Current MediaTimeStamp - Camera.Record.Start
static int cameraRecord(char *arg, struct amn_cfgspace_value_t **val)
{
	char *p;
	int		data;

	p = strrchr(arg, '.');
	if (p == NULL) return 0; // should not return 0;
	p++;
	if (strcmp(p, "Avaliable") == 0) {
		data = netapp_get_RecordTimeAvailable();
	} else if (strcmp(p, "Remaining") == 0) {
		data = netapp_get_RecordTimeRemaining();
	} else if (strcmp(p, "Total") == 0) {
		data = netapp_get_RecordTimeTotal();
	} else if (strcmp(p, "Start") == 0) {
		data = netapp_get_RecordTimeStart();
	} else {
		return -1;
	}
	(*val)->v.intVal = data;
	return -1;
}
// CGI GET property=Camera.Capture.Remaining
static int cameraCaptureRemaining(char *arg, struct amn_cfgspace_value_t **val)
{
	(*val)->v.intVal = netapp_get_CaptureRemaining();
	return -1;
}
// CGI GET property=Camera.Battery.Level
static int cameraBatteryLevel(char *arg, struct amn_cfgspace_value_t **val)
{
	(*val)->v.intVal = netapp_get_PowerLevel();
	return -1;
}

typedef int (*PFN_GET_HANDLER)(char *arg, struct amn_cfgspace_value_t **val);
typedef struct {
	const char*			getArg;
	const int			getArgLen;
	PFN_GET_HANDLER		getHandler;
} CGIGET_HANDLER;

#define	CGI_GET(k)	(k), sizeof(k) - 1

CGIGET_HANDLER get_Handler[] = {
	{CGI_GET("Camera.Record."),						cameraRecord},
	{CGI_GET("Camera.Capture.Remaining"),			cameraCaptureRemaining},
	{CGI_GET("Camera.Battery.Level"),				cameraBatteryLevel},
	{CGI_GET("Camera.Preview.MJPEG.status.record"), cameraVideoStatus},
	{CGI_GET("Camera.Preview.MJPEG.status.mode"),	cameraSensorMode},
	{CGI_GET("Camera.Preview.MJPEG.TimeStamp."),	cameraTime},
	//
	{CGI_GET("Camera.Menu.VideoRes"),				cameraVideoRes},
	{CGI_GET("Camera.Menu.ImageRes"),				cameraImageRes},
	{CGI_GET("Camera.Menu.UIMode"),					cameraUIMode},
	{CGI_GET("Camera.Menu.DefMode"),				cameraDefMode},
	{CGI_GET("Camera.Menu.MTD"),					cameraMTD},
	{CGI_GET("Camera.Menu.AWB"),					cameraAWB},
	{CGI_GET("Camera.Menu.Flicker"),				cameraFlicker},
	{CGI_GET("Camera.Menu.EV"),						cameraEV},
	{CGI_GET("Camera.Menu.PhotoBurst"),				cameraPhotoBurst},
	{CGI_GET("Camera.Menu.Timelapse"),				cameraTimelapse},
	{CGI_GET("Camera.Menu.UpsideDown"),				cameraUpsidedown},
	{CGI_GET("Camera.Menu.SpotMeter"),				cameraSpotMeter},
	{CGI_GET("Camera.Menu.LoopingVideo"),			cameraLoopingVideo},
	{CGI_GET("Camera.Menu.VideoClipTime"),			cameraLoopingVideo},
	{CGI_GET("Camera.Menu.HDR"),					cameraHDR},
	{CGI_GET("Camera.Menu.Q-SHOT"),					cameraQ_SHOT},
	{CGI_GET("Camera.Menu.StatusLights"),			cameraStatusLights},
	{CGI_GET("Camera.Menu.SoundIndicator"),			cameraSoundIndicator},
	{CGI_GET("Camera.Menu.PowerSaving"),			cameraPowerSaving},
	{CGI_GET("Camera.Menu.IsStreaming"),			cameraIsStreaming},
	{CGI_GET("Camera.Menu.FWversion"),				cameraFWversion},
	{CGI_GET("Camera.Menu.PowerOffDelay"),			cameraPowerOffDelay},
	{CGI_GET("Camera.Menu.TVSystem"),				cameraTVSystem},
	{CGI_GET("Camera.Menu.TV"),						cameraTVStatus},
	{CGI_GET("Camera.Menu.SD0"),					cameraSD0},
	{CGI_GET("Camera.Preview.MJPEG.WarningMSG"),    cameraWarning},
	{CGI_GET("Camera.File.Total"),					cameraFileTotal},
	{NULL}
};
/*
 * get a runtime state
 * for example property=mj_w&property=mj_h
 * return == -1: this function take care the setting
 *        >=  0: this function ignore the setting, caller have to do next
 */
int wildstrcmp(char *s1, const char* ws2, int ws2len);
int web_config_wget(char *key, struct amn_cfgspace_value_t **val, char **name)
{
	int		i, idx;

	i = amn_config_wlocate(key);
	*name = amn_ConfigSpaceDict[i];
	*val  = amn_config_get(CONFIG_SPACE_TYPE_CURRENT, *name);
	// If it is a RUNTIME value, read it from specify API and return -1
	// or return i for further.
	if (!((*val)->attr & CFGSPACE_RUNTIME))
		return i;
	for (idx = 0; get_Handler[idx].getArg != NULL; idx++) {
		if (wildstrcmp(*name, get_Handler[idx].getArg, get_Handler[idx].getArgLen) == 0) {
			return get_Handler[idx].getHandler(*name, val);
		}
	}
	return i;
}

int web_config_getEx(char *key, char *szval, int size)
{
	int	idx;
	struct amn_cfgspace_value_t	*cfg_val;
	
	for (idx = 0; get_Handler[idx].getArg != NULL; idx++) {
		const char	*tail;
		
		tail = strrchr(get_Handler[idx].getArg, '.');
		if (tail == NULL)
			tail = (char*)get_Handler[idx].getArg;
		else
			tail++;
		if (strcmpi(key, (char*)tail) == 0) {
			cfg_val = amn_webConfig_get(key);
			if (cfg_val == NULL)
				return 0;
			if (get_Handler[idx].getHandler(key, &cfg_val) != -1) {
				break;	// Error
			}
			switch(cfg_val->type) {
			case CFGSPACE_VALTYP__INT:
				return sprintf(szval, "%d", cfg_val->v.intVal);
			case CFGSPACE_VALTYP__STR:
				strncpy(szval, cfg_val->v.strVal, size - 1);
				return strlen(cfg_val->v.strVal);
			}
		}
	}
	return 0;
}
void cgi_printf(char* buf, char** ptr, int size, char *fmt, ...)
{
    int n;
    char *strbuf;
    va_list args;

    va_start(args, fmt);
    if (buf) {
        n = vsnprintf(*ptr, size + 1, fmt, args);
    }
    else {
        strbuf = osal_malloc(size);
        n = vsnprintf(strbuf, size + 1, fmt, args);
        osal_free(strbuf);
    }
    va_end(args);
    *ptr += n;

}

//*******************************************************************************************************
typedef struct {
	unsigned int	netmID;
	netcgi_init_t	module[1];
} netcg_t;

#pragma arm section rwdata = "netcgi_init", zidata = "netcgi_init"
unsigned int	netcgi_header = NC_HEADER_ID;
#pragma arm section code, rwdata,  zidata
//
void init_cgi_handlers( )
{
	loreq_t	lr;
	amn_setup_loreq(&lr, NC_HEADER_ID, 0, 0);
	amn_load_module(&lr, (netmo_t*)SECTION_START(NETCGI_INIT),
								   SECTION_END(NETCGI_INIT));
}

/*
 * arg:
 *   0: JPEG
 *   2: AVI
 *   3: MOV
 *   4: MP4
 *   5: 3GP
 *  -1: ALL
 * refer AHC_DCF.c m_DcfSupportFileType
 */
int dcim_filter_fmt(void *hinst, char *inout, void *attr, void *arg)
{
	char			*p;
	int				fmt = (int)attr;
	unsigned char	req = (((unsigned char)(int)arg) & (~(GROUP_FLAG)));
	PFFINST			pffi;

	if (!hinst)
		return 0;
	pffi = (PFFINST)(hinst);

	//printc("File %s fmt %d arg %d\r\n", inout, fmt, (int)arg);
	if (fmt != req && req != ALL_TYPE)
		return 0;
	if ((req != ALL_TYPE) && ((unsigned char)(int)arg) & GROUP_FLAG) {
		// Get the first member of group
		FILE_GRPINFO	ginf;
		FindFileGroup(hinst, &ginf);
		if (ginf.grpno != 1)
			return 0;
	}
	p = strchr(inout, '\\');
	if (p) {
		int		i;
		for (i = 0; *p != NULL; p++, i++) {
			*(inout + i) = (*p == '\\')? *p = '/' : *p;
		}
		*(inout + i) = '\0';
	}
	return 1;
}
#define	CGIID(a, b, c, d)	((a) + (b << 8) + (c << 16) + (d << 24))
#define	CGIID_DIR			CGIID('d','i','r',' ')
#define	MAX_FILE_LEN	80
typedef	struct {
		int				cgiid;
		int				retco;
		char			filename[256];
		unsigned int	used;
		int				bufsize;
		char			*buf;
} XMLBUF;
/*
 * dir_copy
 */
int dir_copy(void *dst, void *src, int size)
{
	retbuf_t	*rb;
	XMLBUF		*xmlb;
	int			len;
	
	rb = (retbuf_t*)src;
	xmlb = *(XMLBUF**)&rb->buf[0];
	len = min(size, xmlb->used - rb->off);
	memcpy(dst, xmlb->buf + rb->off, len);
	rb->off += len;
	return len;
}
int dir_close(void *rb)
{
	XMLBUF		*xmlb;
	
	xmlb = *(XMLBUF**)&((retbuf_t*)rb)->buf[0];
	osal_free(xmlb);
	osal_free(rb);
	return 0;
}
/*
 * output XML format of file list
 * cgi-bin/Config.cgi?action=dir&property=DCIM&format=[all[_group]|avi|jpeg[_group]|mov|3gp|mp4]&count=[<number items>]{&from=<from>}
 */
int cgi_dir(IN struct cgi_virt_file_t *f, OUT char *buf, OUT char **endp, IN int maxlen)
{
	static void		*hinst = NULL;
	struct cgi_param_t *prmProperty;
	struct cgi_param_t *prmValue;
	int  resp_code = CGI_ERR__OK;
	char			*ptr, *str;
	int				rlen;
	unsigned int	flen;
	int				from, cnt, rc;
	int				rd;
	char			*fmt;
	FS_FILETIME		tim;
	FFFILTER_FN		filter;
	char			*filename, *sz;
	XMLBUF			*xmlb, **tmp;
	retbuf_t		*rb;
#ifdef __CONFIG_SDK_LETV_CDV01__
	unsigned int idx=0;
#endif
	if (buf == NULL)
		return CGI_ERR__;
	// init DIR XML
	#define	MAX_XML_BUFFER	(64*1024)
	xmlb = (XMLBUF*)osal_malloc(MAX_XML_BUFFER);
	if (xmlb == NULL) {
		return CGI_ERR__;
	}
	xmlb->cgiid	  = CGIID_DIR;
	xmlb->used	  = 0;
	xmlb->bufsize = MAX_XML_BUFFER - sizeof(XMLBUF);
	xmlb->buf	  = (char*)xmlb + sizeof(XMLBUF);
	maxlen = xmlb->bufsize;
	ptr  = str = xmlb->buf;
	rb   = (retbuf_t*)f->hand->control;
	tmp  = (XMLBUF**)&rb->buf[0];
	*tmp = xmlb;
	rb->copyfn  = dir_copy;
	rb->closefn = dir_close;
	rb->off    = 0;
	// the property is path
	filename = &xmlb->filename[0];
	prmProperty = cgi_param_nget( f, "property");
	// FileName is by DCF
	strcpy(filename, "SD:");
	sz = filename + 3;
	prmValue = cgi_param_nget( f, "format");
	if (prmValue) {
		filter.ffn = dcim_filter_fmt;
		if (!stricmp(prmValue->value, "all")) {
			filter.arg = (void*)ALL_TYPE;
		} else if (!stricmp(prmValue->value, "jpeg")) {
			filter.arg = (void*)0;
		} else if (!stricmp(prmValue->value, "avi")) {
			filter.arg = (void*)2;
		} else if (!stricmp(prmValue->value, "mov")) {
			filter.arg = (void*)3;
		} else if (!stricmp(prmValue->value, "mp4")) {
			filter.arg = (void*)4;
		} else if (!stricmp(prmValue->value, "3gp")) {
			filter.arg = (void*)5;
		} else {
			resp_code = CGI_ERR__UNKNOWN_PROPERTY;
			goto __end_dir;
		}
	} else {
		resp_code = CGI_ERR__UNKNOWN_PROPERTY;
		goto __end_dir;
	}
	// Get group option
	prmValue = cgi_param_nget( f, "group");
	if (prmValue) {
		if (!stricmp(prmValue->value, "all")) {
			filter.arg = (void*)((int)filter.arg | GROUP_FLAG);
		}
	}
	AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8 *)"MP4,MOV,JPG,AVI");
	if ((!strcmp(prmProperty->value,"DCIM"))||(!strcmp(prmProperty->value,"Normal"))) {
		AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);
	}else if (!strcmp(prmProperty->value,"Park")) {
		#if ( DCF_DB_COUNT >= 2)
			AHC_UF_SelectDB(DCF_DB_TYPE_2ND_DB);
		#else
			AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);
		#endif
	}if (!strcmp(prmProperty->value,"Event")) {
		#if ( DCF_DB_COUNT >= 3)
			AHC_UF_SelectDB(DCF_DB_TYPE_3RD_DB);
		#else
			AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);
		#endif
	}if (!strcmp(prmProperty->value,"Photo")) {
		#if ( DCF_DB_COUNT >= 4)
			AHC_UF_SelectDB(DCF_DB_TYPE_4TH_DB);
		#else
			AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);
		#endif
	}
	
	// Get read from
	prmValue = cgi_param_nget( f, "from");
	from = (prmValue)? atol(prmValue->value) : -1;
	// Get get count
	prmValue = cgi_param_nget( f, "count");
	cnt = (prmValue)? atol(prmValue->value) : 20;
	rc = 0;		// amount
	CGI_PRINTF( str, ptr, maxlen, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	CGI_PRINTF( str, ptr, maxlen, "<%s>\n",prmProperty->value);
	//CGI_PRINTF( str, ptr, maxlen, "<DCIM>\n");
	if (from == 0 || hinst == NULL) {
		if ((hinst = FindFirstFile(sz, MAX_FILE_LEN, &rlen, &filter)) == NULL)
			goto __ffgetend;
	} else if (from > 0) {
		FindFileSetPosition(hinst, from - 1, &filter);
		if (FindNextFile(hinst, sz, MAX_FILE_LEN, &rlen, &filter) == 0) {
			goto __ffgetend;
		}
	} else {
		if (FindNextFile(hinst, sz, MAX_FILE_LEN, &rlen, &filter) == 0) {
			goto __ffgetend;
		}
	}
	do {
		FILE_GRPINFO ginf;
		MI_INFO		 miInfo;
		
		FindFileAttr(hinst, sz, &flen, &fmt, &rd, &tim);
		FindFileGroup(hinst, &ginf);
		CGI_PRINTF( str, ptr, maxlen, "<file>\n");
#ifdef __CONFIG_SDK_LETV_CDV01__  //remove /SD from real path for customer app
		CGI_PRINTF( str, ptr, maxlen, "\t<name>%s</name>\n", sz);
#else
		CGI_PRINTF( str, ptr, maxlen, "\t<name>/SD%s</name>\n", sz);
#endif
		if (miGetMediaInfo(filename, &miInfo)) {
			if (miInfo.type == 1 /* VIDEO*/) {
				int		fpsX10;
				int		tmlX10;
				
				fpsX10 = (int)(((long long)(miInfo.timescale) * (long long)miInfo.samplecount) * 10 /
								miInfo.duration);
				tmlX10 = miInfo.duration * 10 / miInfo.timescale;
				CGI_PRINTF( str, ptr, maxlen,
							"\t<format size=\"%dx%d\" fps=\"%d\" time=\"%d.%d\">%s</format>\n",
							miInfo.width, miInfo.height, (fpsX10 + 5) / 10, tmlX10 / 10, tmlX10 % 10, fmt);
			} else if (miInfo.type == 2 /* Still Image */) {
				CGI_PRINTF( str, ptr, maxlen, "\t<format size=\"%dx%d\">%s</format>\n",
								miInfo.width, miInfo.height, fmt);
			} else {
				CGI_PRINTF( str, ptr, maxlen, "\t<format>%s</format>\n", fmt);
			}
		} else {
			CGI_PRINTF( str, ptr, maxlen, "\t<format>%s</format>\n", fmt);
		}
		CGI_PRINTF( str, ptr, maxlen, "\t<size>%u</size>\n", flen);
		CGI_PRINTF( str, ptr, maxlen, "\t<attr>%s</attr>\n", (rd & FS_ATTR_READ_ONLY)? "RO" : "RW");
		CGI_PRINTF( str, ptr, maxlen, "\t<time>%02d-%02d-%02d %02d:%02d:%02d</time>\n",
					tim.Year, tim.Month, tim.Day, tim.Hour, tim.Minute, tim.Second);
		#if((defined(DCF_GROUP_FUN) && DCF_GROUP_FUN)||(defined __CONFIG_SDK_LETV_CDV01__))
			#ifdef __CONFIG_SDK_LETV_CDV01__ //set tmp group idx for Demo
				CGI_PRINTF( str, ptr, maxlen, "\t<group mode='%s' key='%d' id='%d' total='%d' no='%d'/>\n",
					netapp_GetUIStateEx(ginf.grpmo), ginf.dkey, /*ginf.grpid*/idx, ginf.grpto, ginf.grpno);
			#else
		CGI_PRINTF( str, ptr, maxlen, "\t<group mode='%s' key='%d' id='%d' total='%d' no='%d'/>\n",
					netapp_GetUIStateEx(ginf.grpmo), ginf.dkey, ginf.grpid, ginf.grpto, ginf.grpno);
		#endif
		#endif
		CGI_PRINTF( str, ptr, maxlen, "</file>\n");
		rc++;
		if (rc == cnt)
			break;
#ifdef __CONFIG_SDK_LETV_CDV01__
		idx ++;
#endif
	} while (FindNextFile(hinst, sz, MAX_FILE_LEN, &rlen, &filter) != 0);
__ffgetend:
	CGI_PRINTF( str, ptr, maxlen, "<amount>%d</amount>\n", rc);
	CGI_PRINTF( str, ptr, maxlen, "</%s>\n",prmProperty->value);
	//CGI_PRINTF( str, ptr, maxlen, "</DCIM>\n");
	resp_code = CGI_ERR__OK;
	ptr = buf + (ptr - str);
__end_dir:
	xmlb->retco = resp_code;
	xmlb->used  = (unsigned int)(ptr - buf);
	*endp       = (char*)ptr;
	return resp_code;
}

/*
 * delete file
 * example: cgi-bin/Config.cgi?action=del&property=$DCIM$100__DSC$MOV_1163.AVI
 *          or
 *          cgi-bin/Config.cgi?action=del&property=key&value=<dkey>&property=group&value=<id>
 */
#define	CGI_DIRSEP			'$'
#define	MAX_PATH_LEN		64
const char	fileroot[] = "SD:\\";
int cgi_delete_file(IN struct cgi_virt_file_t *f, OUT char *buf, OUT char **endp, IN int maxlen)
{
	int		i, len;
	char	*path, *ptr;
	char	file[MAX_PATH_LEN];

	if (!buf) return CGI_ERR__OK;	// dry run
	path = cgi_get_property(f);
	if (strcmp(path, "key") == 0) {
		#if(defined(DCF_GROUP_FUN) && DCF_GROUP_FUN)
		struct cgi_param_t *prmValue;
		unsigned short		dkey, grpid;
		// Delete group
		//   get dir key
		prmValue = cgi_param_nget2( f, "value", cgi_get_param(f));
		dkey = atoi(prmValue->value);
		// 	 get group id
		cgi_inc_param(f);
		prmValue = cgi_param_nget2( f, "value", cgi_get_param(f));
		grpid = atoi(prmValue->value);
		printc("Delete Group File DIRKEY %d GRPID %d\n", dkey, grpid);
		AHC_DCF_DelOneGrp(dkey, grpid);
		#endif
		
		return CGI_ERR__OK;
	}
	len = strlen(fileroot);
	// support $DCIM.. or DCIM...
	// $SD$PATH$FILE
	ptr = path;
	if ((path[0] == CGI_DIRSEP || path[0] == '/') && path[1] == 'S' && path[2] == 'D')
		ptr = path + 3;
	if (*ptr == CGI_DIRSEP || *ptr == '/') len--;
	strncpy(file, fileroot, len);
	for (i = len; ptr[i - len] != 0 && i < MAX_PATH_LEN - 1; i++) {
		file[i] = (ptr[i - len] == CGI_DIRSEP || ptr[i - len] == '/')? '\\' : ptr[i - len];
	}
	file[i] = 0;
	//printc("DELETE %s\r\n", file);
	// Check is wildcard *.*, *.jpg, *.avi
	ptr = strrchr(file, '\\');
	while (ptr) {
		FFFILTER_FN		filter;
		void			*hinst;
		int				rlen;
		int				delcnt;

		ptr++;
		if (*ptr != '*') break;
		// Delete files
		filter.ffn = dcim_filter_fmt;
		if (!stricmp(ptr, "*.*")) {
			filter.arg = (void*)-1;
		} else if (!stricmp(ptr, "*.jpg")) {
			filter.arg = (void*)0;
		} else if (!stricmp(ptr, "*.avi")) {
			filter.arg = (void*)2;
		} else if (!stricmp(ptr, "*.mov")) {
			filter.arg = (void*)3;
		} else if (!stricmp(ptr, "*.mp4")) {
			filter.arg = (void*)4;
		} else if (!stricmp(ptr, "*.3gp")) {
			filter.arg = (void*)5;
		} else {
			return CGI_ERR__OK;
		}
		do {
			// delete all files until no files can be deleted! (read only)
			delcnt = 0;
			strcpy(file, "SD:");
			if ((hinst = FindFirstFile(file + strlen(file), MAX_PATH_LEN, &rlen, &filter)) == NULL)
				break;
			do {
				int		i;
				if (GetFrontFilename(file))
					AHC_UF_SetRearCamPathFlag(AHC_FALSE);
				for (i = 0; *(file + i) != 0; i++)
					if (*(file + i) == '/') *(file + i) = '\\';
				if (netapp_DeleteFile((char*)file) == 0) delcnt++;
				strcpy(file, "SD:");
			} while(FindNextFile(hinst, file + strlen(file), MAX_FILE_LEN, &rlen, &filter) != 0);
		} while (delcnt != 0);
		return CGI_ERR__OK;
	}
	if (GetFrontFilename(file))
		AHC_UF_SetRearCamPathFlag(AHC_FALSE);
	if (netapp_DeleteFile((char*)file))
		return CGI_ERR__;
	return CGI_ERR__OK;
}

/*
 * generate a HTML5 page
 */
static char	html5[] = 
"<!DOCTYPE html>\r\n"
"<html>\r\n"
"<body>\r\n"
"<video width=\"%d\" height=\"%d\" controls>\r\n"
"  <source src=\"%s\" type=\"video/mp4\">\r\n"
"</video>\r\n"
"</body>\r\n"
"</html>\r\n";
/*
 * URL example:
 * /cgi-bin/Config.cgi?action=play&property=DCIM/100__DSC/MOV_0001.MOV
 */
int cgi_play_file(IN struct cgi_virt_file_t *f, OUT char *buf, OUT char **endp, IN int maxlen)
{
	int		i, len;
	char	*path;
	char	vfn[MAX_PATH_LEN];
	MI_INFO	miInfo;
	//int		width, height;

	path = cgi_get_property(f);
	len = strlen(fileroot);
	if (path[0] == CGI_DIRSEP) len--;
	strncpy(vfn, fileroot, len);
	for (i = len; path[i - len] != 0 && i < MAX_PATH_LEN - 1; i++) {
		vfn[i] = (path[i - len] == CGI_DIRSEP || path[i - len] == '/')? '\\' : path[i - len];
	}
	vfn[i] = 0;
	printc("%s %d: Play File %s\r\n", vfn);
	// Get width/height
	if (miGetMediaInfo(vfn, &miInfo) == 0) {
		return CGI_ERR__;
	}
	CGI_PRINTF( buf, *endp, maxlen, html5, miInfo.width, miInfo.height, vfn + len);	// support $DCIM.. or DCIM...
	return CGI_ERR__OK;
}

int cgi_switch_camera(IN struct cgi_virt_file_t *f, OUT char *buf, OUT char **endp, IN int maxlen)
{
	struct cgi_param_t *prmProperty;
	struct cgi_param_t *prmValue;
	
	struct amn_cfgspace_value_t *cfg_val;

	prmProperty = cgi_param_nget2( f, "property", cgi_get_param(f));
	prmValue = cgi_param_nget2( f, "value", cgi_get_param(f));

	if (!stricmp(prmValue->value, "front")) 
		amn_config_set(CONFIG_SPACE_TYPE_CURRENT,prmProperty->value, "front");
	else if (!stricmp(prmValue->value, "rear")) 
		amn_config_set(CONFIG_SPACE_TYPE_CURRENT,prmProperty->value, "rear");

	cfg_val  = amn_config_get( CONFIG_SPACE_TYPE_CURRENT, prmProperty->value);
	printc("prmProperty=%s,value=%s\n",prmProperty->value,cfg_val->v.strVal);
	netapp_switchCamera();

	CGI_PRINTF( buf, *endp, maxlen, "0\nOK\n" );

	return CGI_ERR__OK;
}

