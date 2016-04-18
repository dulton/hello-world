/*
*********************************************************************************************************
*              AIT Multimedia Network -- System Components for Network Layer
*                     (c) Copyright 2012, Alpha Imaging Technology Corp.
*               All rights reserved. Protected by international copyright laws.
*********************************************************************************************************
*
* Description: Dealt here the general system components for the AMN Network Layer, including:
*              File manager: to handle file system related functions, such as POSIX's open(()
*              Session manager: to handle RTSP/HTTP sessions
*
*/

#include "inc/amn_sysobjs.h"
#include "inc/net_serv.h"
#include "inc/amn_event.h"
#include "inc/amnss_mmobjs.h"
#include "UpnpGlobal.h"

#ifndef __AMN_OS_LINUX__
#include "mmpf_rtc.h"
#include "mmpf_fs_api.h"
//#include "AHC_utility.h"
#endif
#include "protocol.h"
#include "netapp.h"
#include <time.h>

extern MMP_FS_FILETIME 	FsOsTime;

LIST_HEAD(all_sessions_list);

#define WEBFILE_OBESITY   100000   // large files will be constrained by the resource: "NET_BANDWIDTH"

//*******************************************************************************************************
#define __opendir(name,dp) do { \
	OSAL_DIR *dirp = osal_malloc( sizeof(OSAL_DIR) ); \
	if (!dirp)  return NULL; \
	dirp->d_dirp = dp; \
	strncpy( dirp->d_path, name, MAX_DIRENT_FILEPATH ); \
	return dirp; \
} while (0)

void __replace_filepath_seperator(char *path)
{
	char *s = path;
	while (*s) {
		if (*s == '/')  *s = '\\';
		s++;
	}
}

//*******************************************************************************************************
// FILE MANAGER
//*******************************************************************************************************
#ifdef __AMN_OS_LINUX__
OSAL_DIR *osal_opendir(const char *name)
{
	OSAL_DIR_O *dp;

	dp = opendir(name);
	if (!dp)  return NULL;

	__opendir( name, dp );
}

int osal_closedir(OSAL_DIR *dirp)
{
	int ret = closedir(dirp->d_dirp);
	osal_free( dirp );
	return ret;
}

struct osal_dirent_t *osal_readdir(OSAL_DIR *dirp)
{
	struct dirent  *dent;
	struct stat     statbuf;
	struct tm      *tm;
	char   buf[256];

	dent = readdir(dirp->d_dirp);
	if (!dent)  return NULL;

	snprintf( buf, 256, "%s/%s", dirp->d_path, dent->d_name );
	if (stat( buf, &statbuf) == -1) {
		osal_dprint(ERR, "Error stat file %s, %s !!", dent->d_name, osal_strerror());
		return NULL;
	}

	dirp->dirent.d_type = dent->d_type;
	strcpy( dirp->dirent.d_name, dent->d_name );
	dirp->dirent.d_fsize = statbuf.st_size;

	tm = localtime(&statbuf.st_mtime);
	dirp->dirent.d_ftime.Year   = tm->tm_year + 1900;
	dirp->dirent.d_ftime.Month  = tm->tm_mon + 1;
	dirp->dirent.d_ftime.Day    = tm->tm_mday;
	dirp->dirent.d_ftime.Hour   = tm->tm_hour;
	dirp->dirent.d_ftime.Minute = tm->tm_min;
	dirp->dirent.d_ftime.Second = tm->tm_sec;

	return &dirp->dirent;
}

#else // __AMN_OS_LINUX__
// this is for ARM-ADS C compiler
//=======================================================================================================
OSAL_FILE *osal_fopen(const char *path, const char *mode)
{
	MMP_ERR 				err;
	MMP_ULONG				ulFileId;

	__replace_filepath_seperator((char*)path);
	err = MMPF_FS_FOpen((char*)path,(char*)mode, &ulFileId);
	
	if(err == MMP_ERR_NONE) {
		return (OSAL_FILE*)ulFileId;
	}
	return NULL;
}

size_t osal_fread(void *ptr, size_t size, size_t nmemb, OSAL_FILE *stream)
{
	MMP_ERR		err;
	MMP_ULONG	ulReadCount;

	err = MMPF_FS_FRead( (MMP_ULONG)stream,(MMP_UBYTE *)ptr, size*nmemb, &ulReadCount );	
	osal_dprint(DEBUG, "FS: size=%d nmemb=%d, err=%x cnt=%d FD=%u",
				size, nmemb, err, ulReadCount, (MMP_ULONG)stream);
	if(err == MMP_ERR_NONE)
		return ulReadCount / size;
	return 0;
}

size_t osal_fwrite(const void *ptr, size_t size, size_t nmemb, OSAL_FILE *stream)
{
	MMP_ERR 	err;
	MMP_ULONG	ulWriteCount;

	err = MMPF_FS_FWrite( (MMP_ULONG)stream, (MMP_UBYTE*)ptr, size*nmemb, &ulWriteCount );
	osal_dprint(DEBUG, "FS: size=%d nmemb=%d, err=%x cnt=%d FD=%u",
				size, nmemb, err, ulWriteCount, (MMP_ULONG)stream);
	if(err == MMP_ERR_NONE)
		return ulWriteCount / size;
	return 0;
}

size_t osal_ftell(OSAL_FILE *stream)
{
	MMP_LONG64 pos;
	
	MMPF_FS_FTell( (MMP_ULONG)stream, &pos);
	return (unsigned int)(pos);
}

int osal_fseek(OSAL_FILE *stream, long offset, int whence)
{
	MMP_LONG64 llOffset = (unsigned long)offset;
	return MMPF_FS_FSeek( (MMP_ULONG)stream, llOffset, whence );
}

int osal_fclose(OSAL_FILE *fp)
{
/*
	MMP_ULONG uiFileSize = 0;
	MMPF_FS_GetFileSize( (MMP_ULONG)fp, &uiFileSize );
	osal_dprint(VERBOSE, "FD=%d, FSZ=%d", (MMP_ULONG)fp, uiFileSize);
	if (uiFileSize >= WEBFILE_OBESITY) {
		resource_free("NET_BANDWIDTH");
	}
*/
	MMPF_FS_FClose( (MMP_ULONG)fp );
	return 0;
}

unsigned int osal_getfilesize(OSAL_FILE *fp)
{
	MMP_ULONG64 uiFileSize = 0;
	MMPF_FS_GetFileSize( (MMP_ULONG)fp, (MMP_ULONG64*)&uiFileSize );
	return (unsigned int) uiFileSize;
}

//------------------------------------------------------------------------------------------
int osal_stat(const char *path, struct osal_stat_t *buf)
{
	MMP_UBYTE  				sFileAttribute;
	//MMP_FS_FILETIME			sFileTime;
	MMP_ULONG				uiFileSize = 0;
	MMP_ULONG 				ulpTimeStamp;
	//MMP_USHORT				Date, Time;

	__replace_filepath_seperator((char*)path);
	
	if (MMPF_FS_GetFileInfo((char*)path,
							&sFileAttribute,
							(MMP_U_LONG*)&ulpTimeStamp,
							(MMP_U_LONG*)&uiFileSize) != MMP_ERR_NONE)
		return -1;

	buf->st_size = uiFileSize;
	buf->st_mtime = 1362641224; // Thu Mar 7 15:27:04 CST 2013
	if (sFileAttribute & FS_ATTR_DIRECTORY)
		buf->st_mode = DT_DIR;
	else
		buf->st_mode = DT_REG;
	osal_dprint(VERBOSE, "FS: filename %s, size %u, mode x%02X", path, buf->st_size, sFileAttribute);

	return 0;
/*
		INT8				chSrcFileName[DCF_MAX_FILE_NAME_SIZE];
		INT8				chTempFileName[DCF_MAX_FILE_NAME_SIZE];
		UINT32				uiNameLength;

	    MEMSET(chRootName, 0, DCF_MAX_FILE_NAME_SIZE);
	    MEMSET(chSrcFileName, 0, DCF_MAX_FILE_NAME_SIZE);
	    
	    STRCPY(chSrcFileName, m_strDcfRootName);
	    STRCAT(chSrcFileName, "\\");
	    
	    MEMSET(chTempFileName, 0, FILE_CHAR_LENGTH + 1);
		AIHC_DcfGetDirFullName(chTempFileName, psDir->uwDirKey, psDir->chDirFreeChar);
	    STRCAT(chSrcFileName, chTempFileName);
	    
	    STRCPY(chRootName, chSrcFileName);
	    
	    MEMSET(chTempFileName, 0, FILE_CHAR_LENGTH + 1);
	    AIHC_DcfGetFileFullName(chTempFileName, psFile->uwFileKey, psFile->chFileFreeChar);
	    STRCAT(chTempFileName, ".");
	    STRCAT(chTempFileName, m_DcfSupportFileType[psFile->byFileType]);
	    
	    STRCAT(chSrcFileName, "\\");
	    STRCAT(chSrcFileName, chTempFileName);
		
		uiNameLength = STRLEN(chSrcFileName);

				AHC_DCF_FileDirGetInfo(	chSrcFileName, 
								uiNameLength, 
								&sFileAttribute, 
								&sFileTime, 
								&uiFileSize);
								
*/
	return 0;
}

//------------------------------------------------------------------------------------------
OSAL_DIR *osal_opendir(const char *name)
{
	MMP_ERR err;
	MMP_ULONG ulDirID;

	__replace_filepath_seperator((char*)name);
	err = MMPF_FS_OpenDir( (char*)name, &ulDirID );
	osal_dprint(VERBOSE, "FS: pathname %s, err=%x", name, err);
	if (err != MMP_ERR_NONE) return NULL;

	__opendir( name, (OSAL_DIR_O*)ulDirID );
}

int osal_closedir(OSAL_DIR *dirp)
{
	int ret = MMPF_FS_CloseDir( (MMP_ULONG)dirp->d_dirp );
	osal_free( dirp );
	return ret;
}

struct osal_dirent_t *osal_readdir(OSAL_DIR *dirp)
{
	struct osal_dirent_t *dent;
	MMP_ERR 			err;
	MMP_UBYTE           byFileMode;
	MMP_UBYTE  			attribute;
    MMP_ULONG64			d_fsize;

	dent = &dirp->dirent;

	byFileMode = MMP_FILE_NAME | MMP_ATTRIBUTE | MMP_ENTRY_SIZE;

	err = MMPF_FS_DirEnt2Info((MMP_ULONG)dirp->d_dirp, (MMP_UBYTE)byFileMode, (MMP_BYTE*) dent->d_name, &attribute, &dent->d_ftime, &d_fsize);
	
	dent->d_fsize = d_fsize;

	osal_dprint(DEBUG, "FS: filename %s size %d, err=%x", dent->d_name, dent->d_fsize, err);
	
	if(err != MMP_ERR_NONE)
	{
		if(err == MMP_FS_ERR_NO_MORE_ENTRY)
			osal_dprint(DEBUG, "GetFileList no more entry.\r\n");
			
		return NULL;
	}
	else
	{
		*(dent->d_name+strlen(dent->d_name)) = '\0';  		//terminating null char //MMPD_FS_GetFileList()
		*(dent->d_name+strlen(dent->d_name)+1) = '\0'; 
		
		if (attribute & FS_ATTR_DIRECTORY)
			dent->d_type = DT_DIR;
		else
			dent->d_type = DT_REG;
		return dent;
	}
	
	return NULL;
}

#endif // __AMN_OS_LINUX__


//*******************************************************************************************************
// SESSION MANAGER
//*******************************************************************************************************
#ifdef __AMN_OS_LINUX__
static int init_random(void)
{
	int fd;
	unsigned int seed;

	if( ( fd = open( "/dev/urandom", O_RDONLY ) ) < 0 ) {
		osal_dprint(ERR, "unable to open /dev/urandom: %s",
				osal_strerror() );
		return -1;
	}
	if( read( fd, &seed, sizeof( seed ) ) < 0 ) {
		osal_dprint(ERR, "unable to read from /dev/urandom: %s",
				osal_strerror() );
		return -1;
	}
	close( fd );
	srandom( seed );
	return 0;
}

#else // __AMN_OS_LINUX__
// this is for ARM-ADS C compiler
//--------------------------------------------------
static int init_random(void)
{
	avChurnRand(NULL, 0);
	return 0;
}

#endif // __AMN_OS_LINUX__

//------------------------------------------------------------------------------------------
void random_bytes( unsigned char *dest, int len )
{
	int i;

	for( i = 0; i < len; ++i )
		dest[i] = random() & 0xff;
}

void random_id( char *dest, int len )
{
	int i;

	for( i = 0; i < len / 2; ++i )
		sprintf( dest + i * 2, "%02X", (unsigned char)( random() & 0xff ) );
	dest[len] = 0;
}

//------------------------------------------------------------------------------------------
static void session_watdog( struct ammo_evmsg *e, void *ss )
{
	struct amn_session_t *s = ss;
	struct timeval now;
	int t;

	int ret = SESSION_WDOG__HEATBEAT;
	
	if(!s) return;
	if (s->heartbeat) ret = s->heartbeat(s);

	switch( ret ) {
	default:
		osal_dprint(WARN, "session %s, unknown heartbeat() return=%d", s->id, ret);
		// no break;

	case SESSION_WDOG__HEATBEAT:
		time_now( &now);
		t = time_diff( &s->refresh_time, &now );
		if ( t <= s->interval*2 ) {
			osal_dprint(DEBUG, "session %s, life=%d", s->id, t );
			return;
		}
		// no heat-beats for more than 2 PINGs
		osal_dprint(VERBOSE, "session %s, force termination !! life=%d", s->id, t );
		// no break;

	case SESSION_WDOG__DEMISE:
		osal_dprint(VERBOSE, "session %s (%p), end of life", s->id, s);
		session_destroy(s, NULL);
		break;
	}
}

// interval: when <= 0  --> no timer
struct amn_session_t *_session_new( void *owner, int interval, int (*heartbeat)(void*),
		int (*teardown)(struct amn_session_t *s, char *reason), char *name )
{
	struct amn_session_t *s;

	s = osal_malloc( sizeof(struct amn_session_t) );
	if (s == NULL) {
	    return s;
	}
	s->owner = owner;
	random_id( s->id, SESSION_ID_LEN );
	memset( s->u.__dummy__, 0, sizeof(s->u.__dummy__) );

	s->name = name;
	s->teardown = teardown;
	s->heartbeat = heartbeat;
	s->interval = interval;
	s->data = NULL;
	time_now( &s->start_time );
	time_now( &s->refresh_time );
	if (interval > 0)
		s->wdog = evmsg_new_timer( interval, 0, session_watdog, s );
	else
		s->wdog = NULL;

	INIT_LIST_HEAD( &s->link );
	list_add_tail( &s->link, &all_sessions_list );
	osal_dprint(VERBOSE, "creating session %s (%p)", s->id, s );

	return s;
}

void session_destroy(struct amn_session_t *s, char *reason)
{
	if (!session_is_valid(s)) {
		osal_dprint(ERR, "Invalid session: %p", s);
		return;
	}
	// if the s is rtsp, it will unregister!!
	unregister_working_rtsp_session(s);
	//
	osal_dprint(VERBOSE, "Kill session %s (%p), by %s", s->id, s, (reason?reason:"WDOG"));

	// The deletion of LIST is before the s->teardown(), so as to hinter re-entry
	// because the check of session_is_valid() will be failed since this moment !!
	list_del( &s->link );

	if (s->wdog)       evmsg_remove( s->wdog );
	if (s->teardown)   s->teardown( s, reason );  // reason == NULL  --> by watchdog
	osal_free( s );
}

// the invocation of this API indicates a message from the Client device => revitalize this Session
struct amn_session_t *session_search_id( char *id )
{
	struct amn_session_t *s;
	struct list_head *l;

	if (!id) return NULL;
	list_for_each( l, &all_sessions_list ) {
		s = list_entry( l, struct amn_session_t, link );
		if (! strcmp( s->id, id )) {
			time_now( &s->refresh_time );   // Session is refreshed again by Client message
			return s;
		}
	}
	return NULL;
}

// i: the i'th iteration. Note that a OWNER may own several sessions at the same time
struct amn_session_t *session_search_owner( void *owner, int i )
{
	struct amn_session_t *s;
	struct list_head *l;
	int j = 0;

	if (!owner) return NULL;
	list_for_each( l, &all_sessions_list ) {
		s = list_entry( l, struct amn_session_t, link );
		if (s->owner == owner && j++ == i)
			return s;
	}
	return NULL;
}

struct amn_session_t *session_search_by_data(void *data )
{
	struct amn_session_t *s;
	struct list_head *l;

	list_for_each( l, &all_sessions_list ) {
		s = list_entry( l, struct amn_session_t, link );
		if (s->data == data) {
			return s;
		}
	}
	return NULL;
}

void session_destroyall(void)
{
	struct amn_session_t *s;
	struct list_head *l;

	list_for_each( l, &all_sessions_list ) {
		if (l == LIST_POISON1)
			return;
		s = list_entry( l, struct amn_session_t, link );
		if (s)
			session_destroy(s,"DestroyAllSession");
	}
}

void session_show(void)
{
	struct amn_session_t *s;
	struct list_head *l;
	int j = 0;

	osal_dprint(VERBOSE, "Show SESSION @%d", osal_jiffies());
	list_for_each( l, &all_sessions_list ) {
		s = list_entry( l, struct amn_session_t, link );
		osal_dprint(VERBOSE, "SESS.%-2d: %s St=%d.%d d=%p o=%p id=%s wdog=%d CB=%p,%p",
			++j, s->name, s->start_time.tv_sec, s->start_time.tv_usec,
			s->data, s->owner, s->id, s->interval, s->heartbeat, s->teardown);
	}
}

void session_refresh( struct amn_session_t *sess)
{
	time_now( &sess->refresh_time );
}

static void handle_session_teardown( struct ammo_evmsg *m, void *arg )
{
	struct amn_session_t *s;
	
	s = session_search_by_data((struct resp_message_t *)arg);
	if (s)
		session_destroy(s, "Event");
}

void session_event_teardown_resp(void* resp)
{
	evmsg_new_message(handle_session_teardown, (void*)resp, 0);
}
//*******************************************************************************************************
// Simple RESOURCE MANAGER
//----------------------------
// The resources are identified by <name>, so that it can be easilly accessed acrosss different
// software modules
//*******************************************************************************************************
static struct amn_session_t *_the_session;
static int					_max_session_num = 1;
void register_working_rtsp_session(struct amn_session_t * sess, int num_sess, int max_sess)
{
	// save the session if it is the first session or max number of session is 1
	if (num_sess == 1 || max_sess == 1)
	_the_session = sess;
	_max_session_num = max_sess;
}

void unregister_working_rtsp_session(struct amn_session_t * sess)
{
	if (sess == _the_session)
		_the_session = NULL;
}
// return 0: RTSP is running and not allow to run HTTP/RTSP for the ip (foreign_addr)
//        1: Allow to do RTSP or HTTP
int check_working_rtsp_session(char *foreign_addr)
{
	struct rtsp_session_t *rtsp_sess;
	
	if (_the_session == NULL || _max_session_num > 1)
		return 1;
	//inet_ntoa(conn_foreign_addr(resp))
	rtsp_sess = cast_protocol_data(_the_session, rtsp_session_t);
	// check the session of foreign address is used
	// printc("rtsp address %s foreign %s\n", rtsp_sess->addr, foreign_addr);
	if (strcmp(rtsp_sess->addr, foreign_addr) == 0) {
		return 1;
	}
	return 0;
}

//*******************************************************************************************************
// Miscellaneous Facilities
//*******************************************************************************************************
void basic_meter_init( struct ammo_basic_meter_t *m, int period, char *name )
{
	int shift, P;

	m->name    = name;
	m->count   = 0;
	m->sum_val = 0;
	m->max_val = 0;
	m->min_val = 0xffffffff;
	m->exp_avg = 0;

	// adjustment of period to 2's POWER, and calculate its shift
	for (shift=1, P=2; P < period; shift++, P <<= 1);
	osal_dprint(INFO, "%s: period=%d -> %d shift=%d", name, period, P, shift);
	m->period = P;
	m->div_shift = shift;
}

void basic_meter_measure( struct ammo_basic_meter_t *m, u32_t val )
{
	m->sum_val += val;                                    // 100*val, for integer division
	m->exp_avg = ((METER_EXP_AVERAGE_ALPHA-1) * m->exp_avg + 100*val) / METER_EXP_AVERAGE_ALPHA;

	if (m->max_val < val)  m->max_val = val;
	if (m->min_val > val)  m->min_val = val;

	if (++m->count >= m->period) {
		osal_dprint(NONE, "%s: avg=%d exp(x100)=%d max=%d min=%d", m->name,
				m->sum_val >> m->div_shift, m->exp_avg, m->max_val, m->min_val);
		m->count   = 0;
		m->sum_val = 0;
		m->max_val = 0;
		m->min_val = 0xffffffff;
	}
}


//*******************************************************************************************************
#include "upnp.h"
int TvDeviceStart(char *ip_address, unsigned short port,
		  const char *desc_doc_name, const char *web_dir_path,
		  void* pfun, int combo);

void init_upnp_system()
{

#ifdef	_ENABLE_UPNP_
	int				ret;
	char			*RootDir;
	char			*desc_doc_name = "dvrdesc.xml";
	char			ip[16];
	unsigned int	uiIP;
	extern struct netif main_netdev;
	#define IP_FIELD(IP,x)		( (IP)>>((x)*8)&0xFF)
		
	uiIP = main_netdev.ip_addr.addr;
	if (uiIP == 0)
		uiIP = 0x010000ac; //172.0.0.1
	snprintf(ip, 32, "%d.%d.%d.%d", IP_FIELD(uiIP,0),  IP_FIELD(uiIP,1),  IP_FIELD(uiIP,2),  IP_FIELD(uiIP,3) );
	RootDir = amn_currConfig_get("Sys.Dir.WWW")->v.strVal;
	printc("==== START DEVICE IP %s:%d ====\r\n", ip, 49152);
	ret = TvDeviceStart(ip, 49152, desc_doc_name, RootDir, (void*)printc, 0);
#endif	// _ENABLE_UPNP_

}


void init_amn_system()
{
	if( init_random() < 0 ) return;
#ifdef __UPNP_DEBUG__
	UpnpInitLog();
#endif
	init_config_manager();
	evmsg_initialize();
}

/*
 * TODO: the function should move to suitable file
 */
typedef	unsigned short UINT16;
typedef struct _AHC_RTC_TIME
{
	UINT16 uwYear;
	UINT16 uwMonth;
	UINT16 uwDay;
	UINT16 uwHour;
	UINT16 uwMinute;
	UINT16 uwSecond;
}AHC_RTC_TIME;

//AHC_RTC_TIME AHC_RTC_GetTime(void); //???
#define YEAR0                   1900
#define EPOCH_YR                1970
#define SECS_DAY                (24L * 60L * 60L)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)

const int _ytab[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};
long _timezone = 0;                 // Difference in seconds between GMT and local time
long _dstbias = 0;                  // Offset for Daylight Saving Time

#define TIME_MAX                4294967295ll
/*
 * return current Unix time, (NO TIME ZONE refer _timezone, AHC_RTC_GetTime())
 */
void AHC_RTC_GetTime(AHC_RTC_TIME *psAHC_RTC_Time);
time_t time(time_t* t)
{
	struct tm		tm;
	struct tm		*tmbuf = &tm;
	AHC_RTC_TIME	rtc;	
	long day, year;
	int tm_year;
	int yday, month;
	long seconds;
	//int overflow;
	long dst;

	AHC_RTC_GetTime(&rtc);
	tmbuf->tm_sec  = rtc.uwSecond;
	tmbuf->tm_min  = rtc.uwMinute;
	tmbuf->tm_hour = rtc.uwHour;
	tmbuf->tm_mday = rtc.uwDay;
	tmbuf->tm_mon  = rtc.uwMonth - 1;
	tmbuf->tm_year = rtc.uwYear - YEAR0;
	tmbuf->tm_isdst= 0;
	year = EPOCH_YR;
	if (tmbuf->tm_year < year - YEAR0) return (time_t) -1;
	seconds = 0;
	day = 0;                      // Means days since day 0 now
	//overflow = 0;

	// Assume that when day becomes negative, there will certainly
	// be overflow on seconds.
	// The check for overflow needs not to be done for leapyears
	// divisible by 400.
	// The code only works when year (1970) is not a leapyear.
	tm_year = tmbuf->tm_year + YEAR0;

	//if (TIME_MAX / 365 < tm_year - year) overflow++;
	day = (tm_year - year) * 365;
	//if (TIME_MAX - day < (tm_year - year) / 4 + 1) overflow++;
	day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
	day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
	day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

	yday = month = 0;
	while (month < tmbuf->tm_mon) {
	yday += _ytab[LEAPYEAR(tm_year)][month];
		month++;
	}
	yday += (tmbuf->tm_mday - 1);
	//if (day + yday < 0) overflow++;
	day += yday;

	tmbuf->tm_yday = yday;
	tmbuf->tm_wday = (day + 4) % 7;               // Day 0 was thursday (4)

	seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;

	seconds += day * SECS_DAY;

	// Now adjust according to timezone and daylight saving time
	seconds += _timezone;

	if (tmbuf->tm_isdst) {
		dst = _dstbias;
	} else {
		dst = 0;
	}

	//if (dst > seconds) overflow++;        // dst is always non-negative
	seconds -= dst;

	if (t) *t = seconds;
	return (time_t) seconds;
}
/* FileTime to time_t */
time_t filetime2time_t(FS_FILETIME* ft)
{
	struct tm		tm;
	struct tm		*tmbuf = &tm;
	long day, year;
	int tm_year;
	int yday, month;
	long seconds;
	//int overflow;
	long dst;

	tmbuf->tm_sec  = ft->Second;	//rtc.uwSecond;
	tmbuf->tm_min  = ft->Minute;	//rtc.uwMinute;
	tmbuf->tm_hour = ft->Hour;		//rtc.uwHour;
	tmbuf->tm_mday = ft->Day;		//rtc.uwDay;
	tmbuf->tm_mon  = ft->Month - 1;	//rtc.uwMonth - 1;
	tmbuf->tm_year = ft->Year - YEAR0;	//rtc.uwYear - YEAR0;
	tmbuf->tm_isdst= 0;
	year = EPOCH_YR;
	if (tmbuf->tm_year < year - YEAR0) return (time_t) -1;
	seconds = 0;
	day = 0;                      // Means days since day 0 now
	//overflow = 0;

	// Assume that when day becomes negative, there will certainly
	// be overflow on seconds.
	// The check for overflow needs not to be done for leapyears
	// divisible by 400.
	// The code only works when year (1970) is not a leapyear.
	tm_year = tmbuf->tm_year + YEAR0;

	//if (TIME_MAX / 365 < tm_year - year) overflow++;
	day = (tm_year - year) * 365;
	//if (TIME_MAX - day < (tm_year - year) / 4 + 1) overflow++;
	day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
	day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
	day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

	yday = month = 0;
	while (month < tmbuf->tm_mon) {
	yday += _ytab[LEAPYEAR(tm_year)][month];
		month++;
	}
	yday += (tmbuf->tm_mday - 1);
	//if (day + yday < 0) overflow++;
	day += yday;

	tmbuf->tm_yday = yday;
	tmbuf->tm_wday = (day + 4) % 7;               // Day 0 was thursday (4)

	seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;

	seconds += day * SECS_DAY;

	// Now adjust according to timezone and daylight saving time
	seconds += _timezone;

	if (tmbuf->tm_isdst) {
		dst = _dstbias;
	} else {
		dst = 0;
	}

	//if (dst > seconds) overflow++;        // dst is always non-negative
	seconds -= dst;

	return (time_t) seconds;
}
/* time_t to struct tm */
struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf) {
  time_t time = *timer;
  unsigned long dayclock, dayno;
  int year = EPOCH_YR;

  dayclock = (unsigned long) time % SECS_DAY;
  dayno = (unsigned long) time / SECS_DAY;

  tmbuf->tm_sec = dayclock % 60;
  tmbuf->tm_min = (dayclock % 3600) / 60;
  tmbuf->tm_hour = dayclock / 3600;
  tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
  while (dayno >= (unsigned long) YEARSIZE(year)) {
    dayno -= YEARSIZE(year);
    year++;
  }
  tmbuf->tm_year = year - YEAR0;
  tmbuf->tm_yday = dayno;
  tmbuf->tm_mon = 0;
  while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon]) {
    dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
    tmbuf->tm_mon++;
  }
  tmbuf->tm_mday = dayno + 1;
  tmbuf->tm_isdst = 0;
  //tmbuf->tm_gmtoff = 0;
  //tmbuf->tm_zone = "UTC";
  return tmbuf;
}

char *asctime_r(const struct tm *tm, char *buf) {
  strftime(buf, ASCTIME_R_BUFFER_SIZE, "%c\n", tm);
  return buf;
}
