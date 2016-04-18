/**************************************************************************
 *
 * Copyright (c) 2000-2003 Intel Corporation 
 * All rights reserved. 
 * Copyright (c) 2012 France Telecom All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met: 
 *
 * - Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer. 
 * - Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution. 
 * - Neither name of Intel Corporation nor the names of its contributors 
 * may be used to endorse or promote products derived from this software 
 * without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************/

/*!
 * \file
 *
 * \brief Defines the Web Server and has functions to carry out
 * operations of the Web Server.
 */

#include "config.h"

#if EXCLUDE_WEB_SERVER == 0

#include "webserver.h"

#include "httpparser.h"
#include "httpreadwrite.h"
#include "ithread.h"
#include "membuffer.h"
#ifdef	_ENABLE_UPNP_
#define	__AMN_ORIGINAL_LIBUPNP_INCLUDE__
#endif
#ifdef __AMN_ORIGINAL_LIBUPNP_INCLUDE__
#include "ssdplib.h"
#else
#define X_USER_AGENT "redsonic"
#endif
#include "statcodes.h"
#include "strintmap.h"
#include "unixutil.h"
#include "upnp.h"
#include "upnpapi.h"
#include "UpnpIntTypes.h"
#include "UpnpStdInt.h"
#include "upnputil.h"
#include "VirtualDir.h"
#include "osl_ext.h"
#include "netapp.h"
#include "net_serv.h"
#include <assert.h>
#ifdef __AMN_OS_LINUX__
#include <fcntl.h>
#include <sys/stat.h>
#endif
#include "mmp_lib.h"
#ifdef WIN32
	 #define snprintf _snprintf
#endif

#ifndef	_ENABLE_UPNP_
#define	pthread_mutex_lock		osal_mutex_lock
#define	pthread_mutex_unlock	osal_mutex_unlock
#define	pthread_mutex_destroy	osal_mutex_destroy
#define	pthread_mutex_lockwait	osal_mutex_lockwait
int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr)
{
	return (osal_mutex_init(mutex) >= 0xFE);
}
#endif
/*!
 * Response Types.
 */
enum resp_type {
	RESP_FILEDOC,
	RESP_XMLDOC,
	RESP_HEADERS,
	RESP_WEBDOC,
	RESP_POST
};

/* mapping of file extension to content-type of document */
struct document_type_t {
	/*! . */
	const char *file_ext;
	/*! . */
	const char *content_type;
	/*! . */
	const char *content_subtype;
};

struct xml_alias_t {
	/*! name of DOC from root; e.g.: /foo/bar/mydesc.xml */
	membuffer name;
	/*! the XML document contents */
	membuffer doc;
	/*! . */
	time_t last_modified;
	/*! . */
	int *ct;
};

static const char *gMediaTypes[] = {
	/*! 0. */
	NULL,
	/*! 1. */
	"audio",
	/*! 2. */
	"video",
	/*! 3. */
	"image",
	/*! 4. */
	"application",
	/*! 5. */
	"text"
};

struct symlink_t {
	char	*symbol;
	char	*target;
};
static osl_ext_sem_t	_cgi_sem, _cgi_cb_sem;
static struct symlink_t _symlink[] = {
							{"/SD",				"SD:"},
							{"/DCIM",			"SD:/DCIM"},
							{"/cgi-bin/DCIM",	"SD:/DCIM"},
							{NULL, NULL}
						};
/*
 * Defines.
 */

/* index into 'gMediaTypes' */
#define AUDIO_STR        "\1"
#define VIDEO_STR        "\2"
#define IMAGE_STR        "\3"
#define APPLICATION_STR  "\4"
#define TEXT_STR         "\5"

/* int index */
#define APPLICATION_INDEX 4
#define TEXT_INDEX        5

/* general */
#define NUM_MEDIA_TYPES       72
#define NUM_HTTP_HEADER_NAMES 33

#ifdef WIN32
static char *web_server_asctime_r(const struct tm *tm, char *buf)
{
	if (tm == NULL || buf == NULL)
		return NULL;

	asctime_s(buf, ASCTIME_R_BUFFER_SIZE, tm);
	return buf;
}
#else
#define web_server_asctime_r asctime_r
#endif

/* sorted by file extension; must have 'NUM_MEDIA_TYPES' extensions */
static const char *gEncodedMediaTypes =
	"aif\0" AUDIO_STR "aiff\0"
	"aifc\0" AUDIO_STR "aiff\0"
	"aiff\0" AUDIO_STR "aiff\0"
	"asf\0" VIDEO_STR "x-ms-asf\0"
	"asx\0" VIDEO_STR "x-ms-asf\0"
	"au\0" AUDIO_STR "basic\0"
	"avi\0" VIDEO_STR "msvideo\0"
	"bmp\0" IMAGE_STR "bmp\0"
	"css\0" TEXT_STR "css\0"
	"dcr\0" APPLICATION_STR "x-director\0"
	"dib\0" IMAGE_STR "bmp\0"
	"dir\0" APPLICATION_STR "x-director\0"
	"dxr\0" APPLICATION_STR "x-director\0"
	"gif\0" IMAGE_STR "gif\0"
	"hta\0" TEXT_STR "hta\0"
	"htm\0" TEXT_STR "html\0"
	"html\0" TEXT_STR "html\0"
	"ico\0" IMAGE_STR "x-icon\0"
	"jar\0" APPLICATION_STR "java-archive\0"
	"jfif\0" IMAGE_STR "pjpeg\0"
	"jpe\0" IMAGE_STR "jpeg\0"
	"jpeg\0" IMAGE_STR "jpeg\0"
	"jpg\0" IMAGE_STR "jpeg\0"
	"js\0" APPLICATION_STR "x-javascript\0"
	"json\0" APPLICATION_STR "json\0"
	"kar\0" AUDIO_STR "midi\0"
	"m3u\0" AUDIO_STR "mpegurl\0"
	"mid\0" AUDIO_STR "midi\0"
	"midi\0" AUDIO_STR "midi\0"
	"mov\0" VIDEO_STR "quicktime\0"
	"mp2v\0" VIDEO_STR "x-mpeg2\0"
	"mp3\0" AUDIO_STR "mpeg\0"
	"mpe\0" VIDEO_STR "mpeg\0"
	"mpeg\0" VIDEO_STR "mpeg\0"
	"mpg\0" VIDEO_STR "mpeg\0"
	"mpv\0" VIDEO_STR "mpeg\0"
	"mpv2\0" VIDEO_STR "x-mpeg2\0"
	"pdf\0" APPLICATION_STR "pdf\0"
	"pjp\0" IMAGE_STR "jpeg\0"
	"pjpeg\0" IMAGE_STR "jpeg\0"
	"plg\0" TEXT_STR "html\0"
	"pls\0" AUDIO_STR "scpls\0"
	"png\0" IMAGE_STR "png\0"
	"qt\0" VIDEO_STR "quicktime\0"
	"ram\0" AUDIO_STR "x-pn-realaudio\0"
	"rmi\0" AUDIO_STR "mid\0"
	"rmm\0" AUDIO_STR "x-pn-realaudio\0"
	"rtf\0" APPLICATION_STR "rtf\0"
	"shtml\0" TEXT_STR "html\0"
	"smf\0" AUDIO_STR "midi\0"
	"snd\0" AUDIO_STR "basic\0"
	"spl\0" APPLICATION_STR "futuresplash\0"
	"ssm\0" APPLICATION_STR "streamingmedia\0"
	"swf\0" APPLICATION_STR "x-shockwave-flash\0"
	"tar\0" APPLICATION_STR "tar\0"
	"tcl\0" APPLICATION_STR "x-tcl\0"
	"text\0" TEXT_STR "plain\0"
	"tif\0" IMAGE_STR "tiff\0"
	"tiff\0" IMAGE_STR "tiff\0"
	"txt\0" TEXT_STR "plain\0"
	"ulw\0" AUDIO_STR "basic\0"
	"wav\0" AUDIO_STR "wav\0"
	"wax\0" AUDIO_STR "x-ms-wax\0"
	"wm\0" VIDEO_STR "x-ms-wm\0"
	"wma\0" AUDIO_STR "x-ms-wma\0"
	"wmv\0" VIDEO_STR "x-ms-wmv\0"
	"wvx\0" VIDEO_STR "x-ms-wvx\0"
	"xbm\0" IMAGE_STR "x-xbitmap\0"
	"xml\0" TEXT_STR "xml\0"
	"xsl\0" TEXT_STR "xml\0"
	"z\0" APPLICATION_STR "x-compress\0"
	"zip\0" APPLICATION_STR "zip\0" "\0";
/* *** end *** */

/*!
 * module variables - Globals, static and externs.
 */

#ifndef __AMN_ORIGINAL_LIBUPNP_INCLUDE__
DOMString ixmlCloneDOMString(const DOMString src)
{
        if (src == NULL) return NULL;
        return strdup(src);
}

void ixmlFreeDOMString(DOMString buf)
{
        if (buf != NULL) osal_free(buf);
}
#endif

static struct document_type_t gMediaTypeList[NUM_MEDIA_TYPES];

/*! Global variable. A local dir which serves as webserver root. */
membuffer gDocumentRootDir;

/*! XML document. */
static struct xml_alias_t gAliasDoc;
static ithread_mutex_t gWebMutex;
extern str_int_entry Http_Header_Names[NUM_HTTP_HEADER_NAMES];

/*!
 * \brief Decodes list and stores it in gMediaTypeList.
 */
static UPNP_INLINE void media_list_init(void)
{
	int i;
	const char *s = gEncodedMediaTypes;
	struct document_type_t *doc_type;

	for (i = 0; *s != '\0'; i++) {
		doc_type = &gMediaTypeList[i];
		doc_type->file_ext = s;
		/* point to type. */
		s += strlen(s) + 1;
		doc_type->content_type = gMediaTypes[(int)*s];
		/* point to subtype. */
		s++;
		doc_type->content_subtype = s;
		/* next entry. */
		s += strlen(s) + 1;
	}
	assert(i == NUM_MEDIA_TYPES);
}

/*!
 * \brief Based on the extension, returns the content type and content
 * subtype.
 *
 * \return
 * \li \c 0 on success
 * \li \c -1 on error
 */
static UPNP_INLINE int search_extension(
	/*! [in] . */
	const char *extension,
	/*! [out] . */
	const char **con_type,
	/*! [out] . */
	const char **con_subtype)
{
	int top, mid, bot;
	int cmp;

	top = 0;
	bot = NUM_MEDIA_TYPES - 1;

	while (top <= bot) {
		mid = (top + bot) / 2;
		cmp = strcasecmp(extension, gMediaTypeList[mid].file_ext);
		if (cmp > 0) {
			/* look below mid. */
			top = mid + 1;
		} else if (cmp < 0) {
			/* look above mid. */
			bot = mid - 1;
		} else {
			/* cmp == 0 */
			*con_type = gMediaTypeList[mid].content_type;
			*con_subtype = gMediaTypeList[mid].content_subtype;
			return 0;
		}
	}

	return -1;
}

/*!
 * \brief Based on the extension, clones an XML string based on type and
 * content subtype. If content type and sub type are not found, unknown
 * types are used.
 *
 * \return
 * \li \c 0 on success.
 * \li \c UPNP_E_OUTOF_MEMORY - on memory allocation failures.
 */
static UPNP_INLINE int get_content_type(
	/*! [in] . */
	const char *filename,
	/*! [out] . */
	DOMString *content_type)
{
	const char *extension;
	const char *type;
	const char *subtype;
	int ctype_found = FALSE;
	char *temp = NULL;
	size_t length = 0;
	int rc = 0;

	(*content_type) = NULL;
	/* get ext */
	extension = strrchr(filename, '.');
	if (extension != NULL)
		if (search_extension(extension + 1, &type, &subtype) == 0)
			ctype_found = TRUE;
	if (!ctype_found) {
		/* unknown content type */
		type = gMediaTypes[APPLICATION_INDEX];
		subtype = "octet-stream";
	}
	length = strlen(type) + strlen("/") + strlen(subtype) + 1;
	temp = osal_malloc(length);
	if (!temp)
		return UPNP_E_OUTOF_MEMORY;
	rc = snprintf(temp, length, "%s/%s", type, subtype);
	if (rc < 0 || (unsigned int) rc >= length) {
		osal_free(temp);
		return UPNP_E_OUTOF_MEMORY;
	}
	(*content_type) = ixmlCloneDOMString(temp);
	osal_free(temp);
	if (!(*content_type))
		return UPNP_E_OUTOF_MEMORY;

	return 0;
}

/*!
 * \brief Initialize the global XML document. Allocate buffers for the XML
 * document.
 */
static UPNP_INLINE void glob_alias_init(void)
{
	struct xml_alias_t *alias = &gAliasDoc;

	membuffer_init(&alias->doc);
	membuffer_init(&alias->name);
	alias->ct = NULL;
	alias->last_modified = 0;
}

/*!
 * \brief Check for the validity of the XML object buffer.
 *
 * \return BOOLEAN.
 */
static UPNP_INLINE int is_valid_alias(
	/*! [in] XML alias object. */
	const struct xml_alias_t *alias)
{
	return alias->doc.buf != NULL;
}

/*!
 * \brief Copy the contents of the global XML document into the local output
 * parameter.
 */
static void alias_grab(
	/*! [out] XML alias object. */
	struct xml_alias_t *alias)
{
	ithread_mutex_lock(&gWebMutex);
	assert(is_valid_alias(&gAliasDoc));
	memcpy(alias, &gAliasDoc, sizeof(struct xml_alias_t));
	*alias->ct = *alias->ct + 1;
	ithread_mutex_unlock(&gWebMutex);
}

/*!
 * \brief Release the XML document referred to by the input parameter. Free
 * the allocated buffers associated with this object.
 */
static void alias_release(
	/*! [in] XML alias object. */
	struct xml_alias_t *alias)
{
	ithread_mutex_lock(&gWebMutex);
	/* ignore invalid alias */
	if (!is_valid_alias(alias)) {
		ithread_mutex_unlock(&gWebMutex);
		return;
	}
	assert(*alias->ct > 0);
	*alias->ct -= 1;
	if (*alias->ct <= 0) {
		membuffer_destroy(&alias->doc);
		membuffer_destroy(&alias->name);
		osal_free(alias->ct);
	}
	ithread_mutex_unlock(&gWebMutex);
}

int web_server_set_alias(const char *alias_name, 
	const char *alias_content, size_t alias_content_length,
	time_t last_modified)
{
	int ret_code;
	struct xml_alias_t alias;

	alias_release(&gAliasDoc);
	if (alias_name == NULL) {
		/* don't serve aliased doc anymore */
		return 0;
	}
	assert(alias_content != NULL);
	membuffer_init(&alias.doc);
	membuffer_init(&alias.name);
	alias.ct = NULL;
	do {
		/* insert leading /, if missing */
		if (*alias_name != '/')
			if (membuffer_assign_str(&alias.name, "/") != 0)
				break;	/* error; out of mem */
		ret_code = membuffer_append_str(&alias.name, alias_name);
		if (ret_code != 0)
			break;	/* error */
		if ((alias.ct = (int *)osal_malloc(sizeof(int))) == NULL)
			break;	/* error */
		*alias.ct = 1;
		membuffer_attach(&alias.doc, (char *)alias_content,
				 alias_content_length);
		alias.last_modified = last_modified;
		/* save in module var */
		ithread_mutex_lock(&gWebMutex);
		gAliasDoc = alias;
		ithread_mutex_unlock(&gWebMutex);

		return 0;
	} while (FALSE);
	/* error handler */
	/* free temp alias */
	membuffer_destroy(&alias.name);
	membuffer_destroy(&alias.doc);
	osal_free(alias.ct);

	return UPNP_E_OUTOF_MEMORY;
}

int web_server_init()
{
	int ret = 0;

	if (bWebServerState == WEB_SERVER_DISABLED) {
		/* decode media list */
		media_list_init();
		membuffer_init(&gDocumentRootDir);
		glob_alias_init();
		pVirtualDirList = NULL;

		/* Initialize callbacks */
		virtualDirCallback.get_info = NULL;
		virtualDirCallback.open = NULL;
		virtualDirCallback.read = NULL;
		virtualDirCallback.write = NULL;
		virtualDirCallback.seek = NULL;
		virtualDirCallback.close = NULL;
		if (osl_ext_sem_create(NULL, 1, &_cgi_cb_sem) != OSL_EXT_SUCCESS ||
			osl_ext_sem_create(NULL, 0, &_cgi_sem) != OSL_EXT_SUCCESS)
			ret = UPNP_E_OUTOF_MEMORY;
		if (ithread_mutex_init(&gWebMutex, NULL) == -1)
			ret = UPNP_E_OUTOF_MEMORY;
		else
			bWebServerState = WEB_SERVER_ENABLED;
	}

	return ret;
}

void web_server_destroy(void)
{
	if (bWebServerState == WEB_SERVER_ENABLED) {
		membuffer_destroy(&gDocumentRootDir);
		alias_release(&gAliasDoc);

		ithread_mutex_lock(&gWebMutex);
		memset(&gAliasDoc, 0, sizeof(struct xml_alias_t));
		ithread_mutex_unlock(&gWebMutex);

		ithread_mutex_destroy(&gWebMutex);
		osl_ext_mutex_delete(&_cgi_sem);
		osl_ext_mutex_delete(&_cgi_cb_sem);
		bWebServerState = WEB_SERVER_DISABLED;
	}
}

/*!
 * \brief Release memory allocated for the global web server root directory
 * and the global XML document. Resets the flag bWebServerState to
 * WEB_SERVER_DISABLED.
 *
 * \return Integer.
 */
static int get_file_info(
	/*! [in] Filename having the description document. */
	const char *filename,
	/*! [out] File information object having file attributes such as filelength,
	 * when was the file last modified, whether a file or a directory and
	 * whether the file or directory is readable. */
	struct File_Info *info)
{
	int code;
	struct osal_stat_t s;
	OSAL_FILE *fp;
	int rc = 0;
	struct tm date;
	char buffer[ASCTIME_R_BUFFER_SIZE];

	ixmlFreeDOMString(info->content_type);	
	info->content_type = NULL;
	code = osal_stat(filename, &s);
	if (code == -1)
		return -1;
	if (S_ISDIR(s.st_mode))
		info->is_directory = TRUE;
	else if (S_ISREG(s.st_mode))
		info->is_directory = FALSE;
	else
		return -1;
	if (info->is_directory == FALSE) {
		fp = osal_fopen(filename, "r");
		if (fp == NULL) return -1;
		osal_fclose(fp);
		/* check readable */
		info->is_readable = 1;
		info->file_length = s.st_size;
		info->last_modified = s.st_mtime;
		rc = get_content_type(filename, &info->content_type);
		UpnpPrintf(UPNP_INFO, HTTP, __FILE__, __LINE__,
			"file info: %s, length: %lld, last_mod=%s readable=%d\n",
			filename, (long long)info->file_length,
			asctime_r(http_gmtime_r(&info->last_modified, &date), buffer),
			info->is_readable);
	}
	return rc;
}

int web_server_set_root_dir(const char *root_dir)
{
	size_t index;
	int ret;

	ret = membuffer_assign_str(&gDocumentRootDir, root_dir);
	if (ret != 0)
		return ret;
	/* remove trailing '/', if any */
	if (gDocumentRootDir.length > 0) {
		index = gDocumentRootDir.length - 1;	/* last char */
		if (gDocumentRootDir.buf[index] == '/')
			membuffer_delete(&gDocumentRootDir, index, 1);
	}

	return 0;
}

/*!
 * \brief Compare the files names between the one on the XML alias the one
 * passed in as the input parameter. If equal extract file information.
 *
 * \return
 * \li \c TRUE - On Success
 * \li \c FALSE if request is not an alias
 */
static UPNP_INLINE int get_alias(
	/*! [in] request file passed in to be compared with. */
	const char *request_file,
	/*! [out] xml alias object which has a file name stored. */
	struct xml_alias_t *alias,
	/*! [out] File information object which will be filled up if the file
	 * comparison succeeds. */
	struct File_Info *info)
{
	int cmp = strcmp(alias->name.buf, request_file);
	if (cmp == 0) {
		/* fill up info */
		info->file_length = (off_t)alias->doc.length;
		info->is_readable = TRUE;
		info->is_directory = FALSE;
		info->last_modified = alias->last_modified;
	}

	return cmp == 0;
}

/*!
 * \brief Compares filePath with paths from the list of virtual directory
 * lists.
 *
 * \return BOOLEAN.
 */
static int isFileInVirtualDir(
	/*! [in] Directory path to be tested for virtual directory. */
	char *filePath)
{
	virtualDirList *pCurVirtualDir;
	size_t webDirLen;

	pCurVirtualDir = pVirtualDirList;
	while (pCurVirtualDir != NULL) {
		webDirLen = strlen(pCurVirtualDir->dirName);
		if (webDirLen) {
			if (pCurVirtualDir->dirName[webDirLen - 1] == '/') {
				if (strncmp(pCurVirtualDir->dirName, filePath,
						webDirLen) == 0)
					return !0;
			} else {
				if (strncmp(pCurVirtualDir->dirName, filePath,
						webDirLen) == 0 &&
				    (filePath[webDirLen] == '/' ||
				     filePath[webDirLen] == '\0' ||
				     filePath[webDirLen] == '?'))
					return !0;
			}
		}
		pCurVirtualDir = pCurVirtualDir->next;
	}

	return 0;
}

/*!
 * \brief Converts input string to upper case.
 */
static void ToUpperCase(
	/*! Input string to be converted. */
	char *s)
{
	while (*s) {
		*s = (char)toupper(*s);
		++s;
	}
}

/*!
 * \brief Finds a substring from a string in a case insensitive way.
 *
 * \return A pointer to the first occurence of s2 in s1.
 */
static char *StrStr(
	/*! Input string. */
	char *s1,
	/*! Input sub-string. */
	const char *s2)
{
	char *Str1;
	char *Str2;
	const char *Ptr;
	char *ret = NULL;

	Str1 = strdup(s1);
	if (!Str1)
		goto error1;
	Str2 = strdup(s2);
	if (!Str2)
		goto error2;
	ToUpperCase(Str1);
	ToUpperCase(Str2);
	Ptr = strstr(Str1, Str2);
	if (!Ptr)
		ret = NULL;
	else
		ret = s1 + (Ptr - Str1);

	osal_free(Str2);
error2:
	osal_free(Str1);
error1:
	return ret;
}

/*!
 * \brief Finds next token in a string.
 *
 * \return Pointer to the next token.
 */
static char *StrTok(
	/*! String containing the token. */
	char **Src,
	/*! Set of delimiter characters. */
	const char *Del)
{
	char *TmpPtr;
	char *RetPtr;

	if (*Src != NULL) {
		RetPtr = *Src;
		TmpPtr = strstr(*Src, Del);
		if (TmpPtr != NULL) {
			*TmpPtr = '\0';
			*Src = TmpPtr + strlen(Del);
		} else
			*Src = NULL;

		return RetPtr;
	}

	return NULL;
}

/*!
 * \brief Returns a range of integers from a string.
 *
 * \return Always returns 1.
 */
static int GetNextRange(
	/*! string containing the token / range. */
	char **SrcRangeStr,
	/*! gets the first byte of the token. */
	off_t *FirstByte,
	/*! gets the last byte of the token. */
	off_t *LastByte)
{
	char *Ptr;
	char *Tok;
	int i;
	int64_t F = -1;
	int64_t L = -1;
	int Is_Suffix_byte_Range = 1;

	if (*SrcRangeStr == NULL)
		return -1;
	Tok = StrTok(SrcRangeStr, ",");
	if ((Ptr = strstr(Tok, "-")) == NULL)
		return -1;
	*Ptr = ' ';
	sscanf(Tok, "%" SCNd64 "%" SCNd64, &F, &L);
	if (F == -1 || L == -1) {
		*Ptr = '-';
		for (i = 0; i < (int)strlen(Tok); i++) {
			if (Tok[i] == '-') {
				break;
			} else if (isdigit(Tok[i])) {
				Is_Suffix_byte_Range = 0;
				break;
			}
		}
		if (Is_Suffix_byte_Range) {
			*FirstByte = (off_t) L;
			*LastByte = (off_t) F;
			return 1;
		}
	}
	*FirstByte = (off_t) F;
	*LastByte = (off_t) L;

	return 1;
}

/*!
 * \brief Fills in the Offset, read size and contents to send out as an HTTP
 * Range Response.
 *
 * \return
 * \li \c HTTP_BAD_REQUEST
 * \li \c HTTP_INTERNAL_SERVER_ERROR
 * \li \c HTTP_REQUEST_RANGE_NOT_SATISFIABLE
 * \li \c HTTP_OK
 */
static int CreateHTTPRangeResponseHeader(
	/*! String containing the range. */
	char *ByteRangeSpecifier,
	/*! Length of the file. */
	off_t FileLength,
	/*! [out] SendInstruction object where the range operations will be stored. */
	struct SendInstruction *Instr)
{
	unsigned long FirstByte, LastByte;
	char *RangeInput;
	char *Ptr;
	int rc = 0;

	Instr->IsRangeActive = 1;
	Instr->ReadSendSize = FileLength;
	if (!ByteRangeSpecifier)
		return HTTP_BAD_REQUEST;
	RangeInput = osal_malloc(strlen(ByteRangeSpecifier) + 1);
	if (!RangeInput)
		return HTTP_INTERNAL_SERVER_ERROR;
	strcpy(RangeInput, ByteRangeSpecifier);
	/* CONTENT-RANGE: bytes 222-3333/4000  HTTP_PARTIAL_CONTENT */
	if (StrStr(RangeInput, "bytes") == NULL ||
	    (Ptr = StrStr(RangeInput, "=")) == NULL) {
		osal_free(RangeInput);
		Instr->IsRangeActive = 0;
		return HTTP_BAD_REQUEST;
	}
	/* Jump = */
	Ptr = Ptr + 1;
	/*
	 * TODO: FileLength is an unsigned long, support ~4G (except -4(UPNP_UNTIL_CLOSE))
	if ((int)FileLength < 0) {
		osal_free(RangeInput);
		printc("BAD REQUEST: %s %d\r\n", __func__, __LINE__);
		return HTTP_REQUEST_RANGE_NOT_SATISFIABLE;
	}
	*/
	if (GetNextRange(&Ptr, (off_t*)&FirstByte, (off_t*)&LastByte) != -1) {
		if (FileLength < FirstByte) {
			osal_free(RangeInput);
			return HTTP_REQUEST_RANGE_NOT_SATISFIABLE;
		}
		if (LastByte >= FirstByte) {
			if (LastByte >= FileLength)
				LastByte = FileLength - 1;
			Instr->RangeOffset = FirstByte;
			Instr->ReadSendSize = LastByte - FirstByte + 1;
			/* Data between two range. */
			rc = snprintf(Instr->RangeHeader,
				sizeof(Instr->RangeHeader),
				"CONTENT-RANGE: bytes %" PRId64
				"-%" PRId64 "/%" PRId64 "\r\n",
				(int64_t)FirstByte,
				(int64_t)LastByte,
				(int64_t)FileLength);
			if (rc < 0 || (unsigned int) rc >= sizeof(Instr->RangeHeader)) {
				osal_free(RangeInput);
				return HTTP_INTERNAL_SERVER_ERROR;
			}
		} else {
			osal_free(RangeInput);
			return HTTP_REQUEST_RANGE_NOT_SATISFIABLE;
		}
	} else {
		osal_free(RangeInput);
		return HTTP_REQUEST_RANGE_NOT_SATISFIABLE;
	}

	osal_free(RangeInput);
	return HTTP_OK;
}

/*!
 * \brief Get header id from the request parameter and take appropriate
 * action based on the ids as an HTTP Range Response.
 *
 * \return
 * \li \c HTTP_BAD_REQUEST
 * \li \c HTTP_INTERNAL_SERVER_ERROR
 * \li \c HTTP_REQUEST_RANGE_NOT_SATISFIABLE
 * \li \c HTTP_OK
 */
static int CheckOtherHTTPHeaders(
	/*! [in] HTTP Request message. */
	http_message_t *Req,
	/*! [out] Send Instruction object to data for the response. */
	struct SendInstruction *RespInstr,
	/*! Size of the file containing the request document. */
	off_t FileSize)
{
	http_header_t *header;
	ListNode *node;
	/*NNS: dlist_node* node; */
	int index, RetCode = HTTP_OK;
	char *TmpBuf;
	size_t TmpBufSize = LINE_SIZE;

	TmpBuf = (char *)osal_malloc(TmpBufSize);
	if (!TmpBuf)
		return HTTP_INTERNAL_SERVER_ERROR;
	node = ListHead(&Req->headers);
	while (node != NULL) {
		header = (http_header_t *) node->item;
		/* find header type. */
		index = map_str_to_int((const char *)header->name.buf,
				header->name.length, Http_Header_Names,
				NUM_HTTP_HEADER_NAMES, FALSE);
		if (header->value.length >= TmpBufSize) {
			osal_free(TmpBuf);
			TmpBufSize = header->value.length + 1;
			TmpBuf = (char *)osal_malloc(TmpBufSize);
			if (!TmpBuf)
				return HTTP_INTERNAL_SERVER_ERROR;
		}
		memcpy(TmpBuf, header->value.buf, header->value.length);
		TmpBuf[header->value.length] = '\0';
		if (index >= 0) {
			switch (Http_Header_Names[index].id) {
			case HDR_TE: {
				/* Request */
				RespInstr->IsChunkActive = 1;

				if (strlen(TmpBuf) > strlen("gzip")) {
					/* means client will accept trailer. */
					if (StrStr(TmpBuf, "trailers") != NULL) {
						RespInstr->IsTrailers = 1;
					}
				}
				break;
			}
			case HDR_CONTENT_LENGTH:
				RespInstr->RecvWriteSize = atoi(TmpBuf);
				break;
			case HDR_RANGE:
				if (FileSize == UPNP_UNTIL_CLOSE) {
					/* not a real file, nothing to do */
					break;
				}
				RetCode = CreateHTTPRangeResponseHeader(TmpBuf,
					FileSize, RespInstr);
				if (RetCode != HTTP_OK) {
					osal_free(TmpBuf);
					return RetCode;
				}
				break;
			case HDR_ACCEPT_LANGUAGE:
				if (header->value.length + 1 > sizeof(RespInstr->AcceptLanguageHeader)) {
					size_t length = sizeof(RespInstr->AcceptLanguageHeader) - 1;
					memcpy(RespInstr->AcceptLanguageHeader, TmpBuf, length);
					RespInstr->AcceptLanguageHeader[length] = '\0';
				} else {
					memcpy(RespInstr->AcceptLanguageHeader, TmpBuf,
						header->value.length + 1);
				}
				break;
			default:
				// UpnpPrintf(UPNP_WARN, HTTP, __FILE__, __LINE__, "HTTP: REQUEST not SUPPORT %s\r\n", header->name.buf);
				/*
				   TODO
				 */
				/*
				   header.value is the value.
				 */
				/*
				   case HDR_CONTENT_TYPE: return 1;
				   case HDR_CONTENT_LANGUAGE:return 1;
				   case HDR_LOCATION: return 1;
				   case HDR_CONTENT_LOCATION:return 1;
				   case HDR_ACCEPT: return 1;
				   case HDR_ACCEPT_CHARSET: return 1;
				   case HDR_USER_AGENT: return 1;
				 */

				/*Header check for encoding */
				/*
				   case HDR_ACCEPT_RANGE:
				   case HDR_CONTENT_RANGE:
				   case HDR_IF_RANGE:
				 */

				/*Header check for encoding */
				/*
				   case HDR_ACCEPT_ENCODING:
				   if(StrStr(TmpBuf, "identity"))
				   {
				   break;
				   }
				   else return -1;
				   case HDR_CONTENT_ENCODING:
				   case HDR_TRANSFER_ENCODING:
				 */
				break;
			}
		}
		node = ListNext(&Req->headers, node);
	}
	osal_free(TmpBuf);

	return RetCode;
}

/*!
 * \brief Processes the request and returns the result in the output parameters.
 *
 * \return
 * \li \c HTTP_BAD_REQUEST
 * \li \c HTTP_INTERNAL_SERVER_ERROR
 * \li \c HTTP_REQUEST_RANGE_NOT_SATISFIABLE
 * \li \c HTTP_OK
 */
static int process_request(
	/*! [in] HTTP Request message. */
	http_message_t *req,
	/*! [out] Type of response. */
	enum resp_type *rtype,
	/*! [out] Headers. */
	membuffer *headers,
	/*! [out] Get filename from request document. */
	membuffer *filename,
	/*! [out] Xml alias document from the request document. */
	struct xml_alias_t *alias,
	/*! [out] Send Instruction object where the response is set up. */
	struct SendInstruction *RespInstr)
{
	int code;
	int err_code;

	char *request_doc;
	struct File_Info finfo;
	int using_alias;
	int using_virtual_dir;
	uri_type *url;
	const char *temp_str;
	int resp_major;
	int resp_minor;
	int alias_grabbed;
	size_t dummy;
	char *extra_headers = NULL;
	int extra_func; //chrison

	print_http_headers(req);
	url = &req->uri;
	assert(req->method == HTTPMETHOD_GET ||
	       req->method == HTTPMETHOD_HEAD ||
	       req->method == HTTPMETHOD_POST ||
	       req->method == HTTPMETHOD_SIMPLEGET);
	/* init */
	memset(&finfo, 0, sizeof(finfo));
	request_doc = NULL;
	finfo.content_type = NULL;
	alias_grabbed = FALSE;
	err_code = HTTP_INTERNAL_SERVER_ERROR;	/* default error */
	using_virtual_dir = FALSE;
	using_alias = FALSE;

	http_CalcResponseVersion(req->major_version, req->minor_version,
				 &resp_major, &resp_minor);
	/* */
	/* remove dots */
	/* */
	request_doc = osal_malloc(url->pathquery.size + 1);
	if (request_doc == NULL) {
		goto error_handler;	/* out of mem */
	}
	memcpy(request_doc, url->pathquery.buff, url->pathquery.size);
	request_doc[url->pathquery.size] = '\0';
	dummy = url->pathquery.size;
	remove_escaped_chars(request_doc, &dummy);
	code = remove_dots(request_doc, url->pathquery.size);
	if (code != 0) {
		err_code = HTTP_FORBIDDEN;
		goto error_handler;
	}
	if (*request_doc != '/') {
		/* no slash */
		err_code = HTTP_BAD_REQUEST;
		goto error_handler;
	}
	if (isFileInVirtualDir(request_doc)) {
		using_virtual_dir = TRUE;
		RespInstr->IsVirtualFile = 1;
		if (membuffer_assign_str(filename, request_doc) != 0) {
			goto error_handler;
		}
	} else {
		/* try using alias */
		if (is_valid_alias(&gAliasDoc)) {
			alias_grab(alias);
			alias_grabbed = TRUE;
			using_alias = get_alias(request_doc, alias, &finfo);
			if (using_alias == TRUE) {
				finfo.content_type =
				    ixmlCloneDOMString("text/xml");

				if (finfo.content_type == NULL) {
					goto error_handler;
				}
			}
		}
	}
	if (using_virtual_dir) {
		if (req->method != HTTPMETHOD_POST) {
			/* get file info */
			int		ret;
			if ((ret = virtualDirCallback.
			    get_info(filename->buf, req, &extra_headers, &finfo, &extra_func)) != 0) {
			    switch(ret) {
			    case VHAND_ERR__OUT_OF_SERVICE:
			    	err_code = HTTP_SERVICE_UNAVAILABLE;
			    	break;
			    default:
					err_code = HTTP_NOT_FOUND;
				}
				goto error_handler;
			}
			/* try index.html if req is a dir */
			if (finfo.is_directory) {
				if (filename->buf[filename->length - 1] == '/') {
					temp_str = "index.html";
				} else {
					temp_str = "/index.html";
				}
				if (membuffer_append_str(filename, temp_str) !=
				    0) {
					goto error_handler;
				}
				/* get info */
				if ((virtualDirCallback.
				     get_info(filename->buf, req, &extra_headers, &finfo,&extra_func) != UPNP_E_SUCCESS)
				    || finfo.is_directory) {
					err_code = HTTP_NOT_FOUND;
					goto error_handler;
				}
			}
			/* not readable */
			if (!finfo.is_readable) {
				err_code = HTTP_FORBIDDEN;
				goto error_handler;
			}
			/* finally, get content type */
			/* if ( get_content_type(filename->buf, &content_type) != 0 ) */
			/*{ */
			/*  goto error_handler; */
			/* } */
		}
	} else if (!using_alias) {
		int i;
		if (gDocumentRootDir.length == 0) {
			goto error_handler;
		}
		/* */
		/* get file name */
		/* */

		/* filename str */
		UpnpPrintf(UPNP_WARN, HTTP, __FILE__, __LINE__,"%d GET filename %s\r\n", __LINE__, request_doc);
		// Find symbolic link path
		for (i = 0; _symlink[i].symbol != NULL; i++) {
			if (!strncmp(request_doc, _symlink[i].symbol, strlen(_symlink[i].symbol))) {
				// replace symbol by target
				membuffer_assign_str(filename, request_doc);
				membuffer_delete(filename, 0, strlen(_symlink[i].symbol));
				if (membuffer_insert(filename, _symlink[i].target, strlen(_symlink[i].target), 0) != 0)
					goto error_handler;	// out of mem
				break;
			}
		}
		if (!_symlink[i].symbol) {
			// No match, append default root path
			if (membuffer_assign_str(filename, gDocumentRootDir.buf) != 0 ||
			    membuffer_append_str(filename, request_doc) != 0) {
				goto error_handler;	/* out of mem */
			}
		}
		UpnpPrintf(UPNP_WARN, HTTP, __FILE__, __LINE__,"%d GET filename %s\r\n", __LINE__, filename->buf);
		/* remove trailing slashes */
		while (filename->length > 0 &&
		       filename->buf[filename->length - 1] == '/') {
			membuffer_delete(filename, filename->length - 1, 1);
		}
		if (req->method != HTTPMETHOD_POST) {
			/* get info on file */
			if (get_file_info(filename->buf, &finfo) != 0) {
				err_code = HTTP_NOT_FOUND;
				goto error_handler;
			}
			/* try index.html if req is a dir */
			if (finfo.is_directory) {
				if (filename->buf[filename->length - 1] == '/') {
					temp_str = "index.html";
				} else {
					temp_str = "/index.html";
				}
				if (membuffer_append_str(filename, temp_str) !=
				    0) {
					goto error_handler;
				}
				/* get info */
				if (get_file_info(filename->buf, &finfo) != 0 ||
				    finfo.is_directory) {
					err_code = HTTP_NOT_FOUND;
					goto error_handler;
				}
			}
			/* not readable */
			if (!finfo.is_readable) {
				err_code = HTTP_FORBIDDEN;
				goto error_handler;
			}
		}
		/* finally, get content type */
		/*      if ( get_content_type(filename->buf, &content_type) != 0 ) */
		/*      { */
		/*          goto error_handler; */
		/*      } */
	}
	RespInstr->ReadSendSize = finfo.file_length;
	/* Check other header field. */
	if ((code =
	     CheckOtherHTTPHeaders(req, RespInstr,
				   finfo.file_length)) != HTTP_OK) {
		err_code = code;
		goto error_handler;
	}
	if (req->method == HTTPMETHOD_POST) {
		*rtype = RESP_POST;
		err_code = HTTP_OK;
		goto error_handler;
	}
	/*extra_headers = UpnpFileInfo_get_ExtraHeaders(finfo); */
	if (!extra_headers) {
		extra_headers = "";
	}

	/* Check if chunked encoding should be used. */
	if (using_virtual_dir && finfo.file_length == UPNP_USING_CHUNKED) {
		/* Chunked encoding is only supported by HTTP 1.1 clients */
		if (resp_major == 1 && resp_minor == 1) {
			RespInstr->IsChunkActive = 1;
		} else {
			/* The virtual callback indicates that we should use
			 * chunked encoding however the client doesn't support
			 * it. Return with an internal server error. */
			err_code = HTTP_NOT_ACCEPTABLE;
			goto error_handler;
		}
	}
	if (RespInstr->IsRangeActive && RespInstr->IsChunkActive) {
		/* Content-Range: bytes 222-3333/4000  HTTP_PARTIAL_CONTENT */
		/* Transfer-Encoding: chunked */
		if (http_MakeMessage(headers, resp_major, resp_minor,
		    "R" "T" "GKLD" "s" "tcS" "Xc" "sCc",
		    HTTP_PARTIAL_CONTENT,	/* status code */
		    finfo.content_type,	/* content type */
		    RespInstr,	/* range info */
		    RespInstr,	/* language info */
		    "LAST-MODIFIED: ",
		    &finfo.last_modified,
		    X_USER_AGENT, extra_headers) != 0) {
			goto error_handler;
		}
	} else if (RespInstr->IsRangeActive && !RespInstr->IsChunkActive) {
		/* Content-Range: bytes 222-3333/4000  HTTP_PARTIAL_CONTENT */
		/* Transfer-Encoding: chunked */
		if (http_MakeMessage(headers, resp_major, resp_minor,
		    "R" "N" "T" "GLD" "s" "tcS" "Xc" "sCc",
		    HTTP_PARTIAL_CONTENT,	/* status code */
		    RespInstr->ReadSendSize,	/* content length */
		    finfo.content_type,	/* content type */
		    RespInstr,	/* range info */
		    RespInstr,	/* language info */
		    "LAST-MODIFIED: ",
		    &finfo.last_modified,
		    X_USER_AGENT, extra_headers) != 0) {
			goto error_handler;
		}
	} else if (!RespInstr->IsRangeActive && RespInstr->IsChunkActive) {
		/* Content-Range: bytes 222-3333/4000  HTTP_PARTIAL_CONTENT */
		/* Transfer-Encoding: chunked */
		if (http_MakeMessage(headers, resp_major, resp_minor,
		    "RK" "TLD" "s" "tcS" "Xc" "sCc",
		    HTTP_OK,	/* status code */
		    finfo.content_type,	/* content type */
		    RespInstr,	/* language info */
		    "LAST-MODIFIED: ",
		    &finfo.last_modified,
		    X_USER_AGENT, extra_headers) != 0) {
			goto error_handler;
		}
	} else {
		/* !RespInstr->IsRangeActive && !RespInstr->IsChunkActive */
		if (RespInstr->ReadSendSize != 0) {
			/* Content-Range: bytes 222-3333/4000  HTTP_PARTIAL_CONTENT */
			/* Transfer-Encoding: chunked */
			if (http_MakeMessage(headers, resp_major, resp_minor,
			    "R" "N" "TLD" "s" "tcS" "Xc" "sCc",
			    HTTP_OK,	/* status code */
			    RespInstr->ReadSendSize,	/* content length */
			    finfo.content_type,	/* content type */
			    RespInstr,	/* language info */
			    "LAST-MODIFIED: ",
			    &finfo.last_modified,
			    X_USER_AGENT,
			    extra_headers) != 0) {
				goto error_handler;
			}
		} else {
			/* Content-Range: bytes 222-3333/4000  HTTP_PARTIAL_CONTENT */
			/* Transfer-Encoding: chunked */
			if (http_MakeMessage(headers, resp_major, resp_minor,
			    "R" "TLD" "s" "tcS" "Xc" "sCc",
			    HTTP_OK,	/* status code */
			    finfo.content_type,	/* content type */
			    RespInstr,	/* language info */
			    "LAST-MODIFIED: ",
			    &finfo.last_modified,
			    X_USER_AGENT,
			    extra_headers) != 0) {
				goto error_handler;
			}
		}
	}
	if (req->method == HTTPMETHOD_HEAD) {
		*rtype = RESP_HEADERS;
	} else if (using_alias) {
		/* GET xml */
		*rtype = RESP_XMLDOC;
	} else if (using_virtual_dir) {
		*rtype = RESP_WEBDOC;
	} else {
		/* GET filename */
		*rtype = RESP_FILEDOC;
	}
	/* simple get http 0.9 as specified in http 1.0 */
	/* don't send headers */
	if (req->method == HTTPMETHOD_SIMPLEGET) {
		membuffer_destroy(headers);
	}
	err_code = HTTP_OK;

 error_handler:
	osal_free(request_doc);
	ixmlFreeDOMString(finfo.content_type);
	if (err_code != HTTP_OK && alias_grabbed) {
		alias_release(alias);
	}

	return err_code;
}

/*!
 * \brief Receives the HTTP post message.
 *
 * \return
 * \li \c HTTP_INTERNAL_SERVER_ERROR
 * \li \c HTTP_UNAUTHORIZED
 * \li \c HTTP_REQUEST_RANGE_NOT_SATISFIABLE
 * \li \c HTTP_OK
 */
static int http_RecvPostMessage(
	/*! HTTP Parser object. */
	http_parser_t *parser,
	/*! [in] Socket Information object. */
	SOCKINFO *info,
	/*! File where received data is copied to. */
	char *filename,
	/*! Send Instruction object which gives information whether the file
	 * is a virtual file or not. */
	struct SendInstruction *Instr)
{
	char *Buf = NULL;   // char Buf[1024];
	int Timeout = 0;
	OSAL_FILE *Fp;
	//parse_status_t status = PARSE_OK;
	int ok_on_close = FALSE;
	int num_read = 0;
	int ret_code = HTTP_OK;
	long	wrcb;

	if (Instr && Instr->IsVirtualFile) {
		struct File_Info finfo;
		/*
		 * give parser into extra_head of get_info to parsing continually
		 */
		if (parser->scanner.msg->length - parser->scanner.cursor < 512) {
			// the buffer run out, and all of header may not receive yet.
			// try to read again.
			char	*buffer;
			buffer = (char*)osal_malloc(2048);
			if (!buffer) {printc(BG_RED("%s %d:ERROR\n"), __func__, __LINE__);return 0;}
			num_read = sock_read(info, buffer, 2048, &Timeout);
			if (num_read > 0) {
				membuffer_append(&parser->msg.msg, buffer, (size_t)num_read);
			}
			osal_free(buffer);
		}
		virtualDirCallback.get_info(filename, &parser->msg, (char**)parser, &finfo, NULL); // to tell our HTTP Request Message
		Fp = (virtualDirCallback.open) (filename, UPNP_WRITE);
		if (Fp == NULL)
			return HTTP_INTERNAL_SERVER_ERROR;
	} else {
		Fp = osal_fopen(filename, "wb");
		if (Fp == NULL)
			return HTTP_UNAUTHORIZED;
		parser->position = POS_ENTITY;
	}
	wrcb = 0;
	/* read more if necessary entity */
	if (Instr && Instr->IsVirtualFile) {
		// get current total written size
		wrcb = virtualDirCallback.write(Fp, NULL, 0);
		// the wrcb is total size of data that had been written after the file opend
		// Instr->RecvWriteSize is Content-Length
		if (wrcb == Instr->RecvWriteSize) {
			// got all of data
			ret_code = HTTP_OK;
			parser->position = POS_COMPLETE;
		}
	}
	Buf = osal_malloc(1024);
	while (wrcb < Instr->RecvWriteSize) {
			num_read = sock_read(info, Buf, 1024, &Timeout);
		if (num_read > 0) {
			if (Instr && Instr->IsVirtualFile) {
				// 
				wrcb = virtualDirCallback.write(Fp, Buf, num_read);
				if (wrcb < 0) {
					ret_code = HTTP_INTERNAL_SERVER_ERROR;
					goto ExitFunction;
				}
				// the wrcb is total size of data that had been written after the file opend
				// Instr->RecvWriteSize is Content-Length
				if (wrcb == Instr->RecvWriteSize) {
					// got all of data
					ret_code = HTTP_OK;
					parser->position = POS_COMPLETE;
					}
				} else {
					size_t n = osal_fwrite(Buf, 1, num_read, Fp);
				if (n != num_read) {
					ret_code = HTTP_INTERNAL_SERVER_ERROR;
					goto ExitFunction;
				}
				}
			} else if (num_read == 0) {
				if (ok_on_close) {
					UpnpPrintf(UPNP_INFO, HTTP, __FILE__, __LINE__,
						"<<< (RECVD) <<<\n%s\n-----------------\n",
						parser->msg.msg.buf);
					print_http_headers(&parser->msg);
					parser->position = POS_COMPLETE;
				} else {
					/* partial msg or response */
					parser->http_error_code = HTTP_BAD_REQUEST;
					ret_code = HTTP_BAD_REQUEST;
					goto ExitFunction;
				}
			} else {
				//ret_code = num_read;
			printc("TIME OUT total read %d\n", wrcb);
			ret_code = HTTP_PARTIAL_CONTENT;
			goto ExitFunction;
		}
	}
ExitFunction:
	if (Instr && Instr->IsVirtualFile) {
		virtualDirCallback.close(Fp);
	} else {
		osal_fclose(Fp);
	}

	if (Buf) osal_free(Buf);
	return ret_code;
}

void web_server_callback(http_parser_t *parser, INOUT http_message_t *req,
	SOCKINFO *info)
{
	int ret;
	int timeout = 5;
	enum resp_type rtype = 0;
	membuffer headers;
	membuffer filename;
	struct xml_alias_t xmldoc;
#if 0
	struct SendInstruction *pRespInstr;
	struct SendInstruction RespInstr;

	/*Initialize instruction header. */
	RespInstr.IsVirtualFile = 0;
	RespInstr.IsChunkActive = 0;
	RespInstr.IsRangeActive = 0;
	RespInstr.IsTrailers = 0;
	memset(RespInstr.AcceptLanguageHeader, 0,
	       sizeof(RespInstr.AcceptLanguageHeader));

	pRespInstr = &RespInstr;
#else
	struct SendInstruction *pRespInstr = osal_zmalloc( sizeof(struct SendInstruction) );
#endif
	/* init */
	membuffer_init(&headers);
	membuffer_init(&filename);

	/*Process request should create the different kind of header depending on the */
	/*the type of request. */
	ret = process_request(req, &rtype, &headers, &filename, &xmldoc,
		pRespInstr);
	if (ret != HTTP_OK) {
		/* send error code */
		http_SendStatusResponse(info, ret, req->major_version,
			req->minor_version);
	} else {
		/* send response */
		switch (rtype) {
		case RESP_FILEDOC:
			// The timeout need consider as transfer rate is high but consumption is slow and recevie buffer
			// is small in recevier. It will cause receiver pauses to receive data a while. If timeout is short,
			// the socket will be closed by timeout!!
			// HTTP file playback may get a timeout issue when media (bitrate 15Mbits) sent fast (26Mbits or above)
			// to Android device.
			timeout = 20;	// The value 20 is by experiment,as video is 15Mbits,
							// tcp has 26Mbits in transfer to NEXUS 7 video player!
			http_SendMessage(info, &timeout, "Ibf",
					 pRespInstr,
					 headers.buf, headers.length,
					 filename.buf);
			UpnpPrintf(UPNP_WARN, HTTP, __FILE__, __LINE__,"%d FILEDOC %s\r\n", __LINE__, filename.buf);
			break;
		case RESP_XMLDOC:
			http_SendMessage(info, &timeout, "Ibb",
				pRespInstr,
				headers.buf, headers.length,
				xmldoc.doc.buf, xmldoc.doc.length);
			alias_release(&xmldoc);
			UpnpPrintf(UPNP_WARN, HTTP, __FILE__, __LINE__,"%d XMLDOC %s\r\n", __LINE__, xmldoc.doc.buf);
			break;
		case RESP_WEBDOC:
			/*http_SendVirtualDirDoc(info, &timeout, "Ibf",
				pRespInstr,
				headers.buf, headers.length,
				filename.buf);*/
			http_SendMessage(info, &timeout, "Ibf",
				pRespInstr,
				headers.buf, headers.length,
				filename.buf);
			UpnpPrintf(UPNP_WARN, HTTP, __FILE__, __LINE__,"%d WEBDOC %s\r\n", __LINE__, filename.buf);
			break;
		case RESP_HEADERS:
			/* headers only */
			http_SendMessage(info, &timeout, "b",
				headers.buf, headers.length);
			UpnpPrintf(UPNP_WARN, HTTP, __FILE__, __LINE__,"%d HEADERS %s\r\n", __LINE__, headers.buf);
			break;
		case RESP_POST:
			/* headers only */
			ret = http_RecvPostMessage(parser, info, filename.buf,
				pRespInstr);
			if (ret != HTTP_OK){
				req->status_code = HTTP_BAD_REQUEST;
			}
			/* Send response. */
			http_MakeMessage(&headers, 1, 1,
				"RTLSXcCc",
				ret, "text/html", pRespInstr, X_USER_AGENT, "WebServer by Alpha Imaging Technology Corp.");
			http_SendMessage(info, &timeout, "b",
				headers.buf, headers.length);
			UpnpPrintf(UPNP_WARN, HTTP, __FILE__, __LINE__,"%d POST %s\r\n", __LINE__, headers.buf);
			break;
		default:
			UpnpPrintf(UPNP_INFO, HTTP, __FILE__, __LINE__,
				"webserver: Invalid response type received.\n");
			assert(0);
			break;
		}
	}
	UpnpPrintf(UPNP_INFO, HTTP, __FILE__, __LINE__,
		   "webserver: request processed...\n");
	membuffer_destroy(&headers);
	membuffer_destroy(&filename);
	osal_free(pRespInstr)
}

/* return 0: successful
 *        1: failed
 *        2: timeout
 */
static int	_uiret;
int webwait_uiproc(void *handle, void *arg)
{
	printc("%s %d\r\n", __func__, __LINE__);
	if (osl_ext_sem_take(&_cgi_sem, (osl_ext_time_ms_t)arg) == OSL_EXT_SUCCESS) {
		return _uiret;
	} else {
		netapp_CGIOpFeedback(handle, -1);
		osl_ext_sem_set(&_cgi_sem, 0);
		return -100;	// timeout!!
	}
}

/*
 * arg is 1 process failed!
 * arg is 0 process successfully
 */
void webwait_uisignal(void *handle, void *arg)
{
	printc("%s %d\r\n", __func__, __LINE__);
	_uiret = (int)arg;
	osl_ext_sem_give(&_cgi_sem);
}
//
void acquire_cgi_Callback()
{
	osl_ext_sem_take(&_cgi_cb_sem, 0);
}
//
void release_cgi_Callback()
{
	osl_ext_sem_give(&_cgi_cb_sem);
}
#endif /* EXCLUDE_WEB_SERVER */

