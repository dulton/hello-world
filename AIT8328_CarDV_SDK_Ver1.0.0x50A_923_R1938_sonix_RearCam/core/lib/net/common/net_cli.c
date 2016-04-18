//==============================================================================
//
//  Description : LWIP Network UART Command set
//  Author      : Bernard Shyu
//  Revision    : 1.0
//
//==============================================================================
#include "stdlib.h"
#include "ctype.h"
#include "opt.h"

#include "lwip/init.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"

#include "lwip/stats.h"

#include "lwip/tcp_impl.h"
#include "lwip/inet_chksum.h"

#include "lwip/tcpip.h"
#include "lwip/sockets.h"

#include "ethernetif.h"
#include "netconf.h"
//#include "AHC_OS.h"
#include "os_wrap.h"
#include "amn_sysobjs.h"
#include "netapp.h"
#include "wlan.h"
#include "gb9662if.h"
//#include "gb9619.h"
#include "AHC_General.h"
#include "amn_event.h"
#ifndef LINUX
#include "lib_retina.h" //printc
#include "UartShell.h"
#endif


#define NETCMD_EXEC   0
#define NETCMD_HELP   1

#define NETCMD_NET    0
#define NETCMD_WIFI   1
#define NETCMD_AGENT  2

extern struct netif main_netdev;
extern bool gbWifiConnect; 
extern void NETWORK_init(void);
extern char *strstr(const char *s1, const char *s2);
extern void UartCmd_ASSCommands(char* szParam, int help);  // AIT streaming server

/* ********************************************************************* */
#define MAX_UART_CMD_LEN  128
#define NET_UART_CMD_NUM  4
static char UartNetCmdArray[NET_UART_CMD_NUM][MAX_UART_CMD_LEN];
static int UartNetIdx_push = 0;
static int UartNetIdx_pop  = 0;
static int UartNet_KillBlockedCmd;

#define check_cmd_idx(idx)  (UartNet_KillBlockedCmd)

/*definition for iperf*/
#define IS_WHITE(x)                 ( (x) == ' ' || (x) == '\t' )
#define IS_SPACE(x)                 IS_WHITE(x)
#define EAT_WHITE(x)                while( IS_WHITE(*(x))) x++;
#define EAT_NON_WHITE(x)            while( *(x)&&!IS_WHITE(*(x))) x++;
#define IS_A_DIGIT(x)				((x >='0') && (x <='9'))

#define IPERF_SOCKET_TCP			0
#define IPERF_SOCKET_UDP			1
#define	IPERF_REV_BUF_SIZE			1470
#define IPERF_SERVER_STK_SIZE		1024
#define IPERF_SERVER_PRIO			LWIP_TASK_START_PRIO + LWIP_TASK_MAX - 1

#define TASK_UART_BRUST_PRIO      	(TASK_AHC_INIT_PRIO)
//iperf server/client data structure

//definition from iperf 2.0.5 source code >
struct UDP_datagram {
    u32_t id;
    u32_t tv_sec;
    u32_t tv_usec;
};

typedef struct server_hdr {

    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no 
     * information bits are set then the header is ignored.
     */
    s32_t flags;
    s32_t total_len1;
    s32_t total_len2;
    s32_t stop_sec;
    s32_t stop_usec;
    s32_t error_cnt;
    s32_t outorder_cnt;
    s32_t datagrams;
    s32_t jitter1;
    s32_t jitter2;

} server_hdr;
//<definition from iperf 2.0.5 source code

struct IPF_CLIENT_STATISTICS {
	
	u16_t 	local_port;
	u32_t	rev_seq;
    u32_t	intervel;					//seconds,default is 0	
    u32_t	intervel_offset;			//seconds,default is 0
    u32_t	start_time;					//miliseconds
    u64_t	total_sendbytes;
    u64_t	intervel_sendbytes;
};

struct IPF_client {
	
    struct sockaddr_in 				sockaddr;
    
    s32_t 							sock_fd;
    u32_t 							sotype;						//UDP(1) or TCP(0),default is 0
    
   	u16_t 							tos;
    u16_t 							tcp_window;					//default is TCP_WND(2920Byte),Need TO DO!
    u8_t							bNodelay;					//TCP Nodelay
    
    u64_t							max_bandwidth;				//unit is KByte,0 : disable bandwidth control
    u32_t							trans_bytes;				//in bytes
    u32_t							trans_timeout;				//miliseconds
    u32_t							pktlength;					//default is 1470 bytes
    
    u32_t							snt_seq;
    u32_t							start_time;					//miliseconds
    u32_t							sntbytes;
    	
    struct ammo_evmsg 				*pTimer;
    struct IPF_CLIENT_STATISTICS	sClientStaData;
};

struct IPF_STATISTICS {
	
	u16_t 	remote_port;
	u32_t	rev_seq;
    u32_t	intervel;					//seconds,default is 0	
    u32_t	intervel_offset;			//seconds,default is 0
    u32_t	start_time;					//miliseconds
    u64_t	total_revbytes;
    u64_t	intervel_revbytes;
};

struct IPF_server {
	
	struct ammo_evmsg 		*pTimer;
	struct IPF_server   	*pListNextServer;			//pointer to next UDP or TCP server
	struct IPF_TCP_session	*pListTCPsess;				//pointer to the head of TCP session list. 
    
    s32_t 					bind_fd;
    u32_t 					sotype;						//UDP(1) or TCP(0),default is 0
    u32_t					remote_ip;
    u16_t 					bind_port;					//default port is 5001
    u16_t 					tcp_window;					//default is TCP_WND(2920Byte)
    u8_t					bNodelay;					//TCP Nodelay
    u8_t					reserved;					
   	
    //Below is only used in UDP case.
    struct IPF_STATISTICS	sStaData;
    u32_t					bind_ip;
    
    
};

struct IPF_TCP_session {
	s32_t 					sock_fd;
    struct IPF_TCP_session  *pNextTCPsess;	//pointer to the next of TCP session. 
    struct ammo_evmsg 		*pTimer;
    struct IPF_STATISTICS	sStaData;
};

/*iperf server's data and task's stack*/
OS_STK iPerf_Server_Task_Stk[IPERF_SERVER_STK_SIZE];
static struct IPF_server * gpsiPerfServerData = NULL;
struct ammo_evmsg *gpServerTimer;
MMPF_OS_SEMID giPerfStaSem = -1;

void push_uart_cmd(char *cmd, char *szParam)
{
	int i;
	i = (UartNetIdx_push + 1) % NET_UART_CMD_NUM;
	if (i == UartNetIdx_pop) {
		printc("WARNING: NETWORK-CLI full, for cmd: '%s' \n", szParam);
		return;
	}
	if (((i - UartNetIdx_pop + NET_UART_CMD_NUM) % NET_UART_CMD_NUM) >= 2)
		UartNet_KillBlockedCmd = 1;

	printc("UARTCmd '%s' posted, wait for processing \n\n", szParam);
	strncpy( UartNetCmdArray[UartNetIdx_push],   cmd,  8 );
	strncpy( UartNetCmdArray[UartNetIdx_push]+8, szParam, MAX_UART_CMD_LEN );
	UartNetIdx_push = i;
	netapp_CmdNetwork();
}

static char *pop_uart_cmd()
{
	char *szParam = NULL;
	if (UartNetIdx_push != UartNetIdx_pop) {
		szParam = UartNetCmdArray[UartNetIdx_pop];
		UartNetIdx_pop = (UartNetIdx_pop + 1) % NET_UART_CMD_NUM;
	}
	UartNet_KillBlockedCmd = 0;
	return szParam;
}

/* ********************************************************************* */
#include "lwip/icmp.h"
#include "lwip/raw.h"

static ip_addr_t ping_addr;
static int seq_num;

/* Ping using the socket api */
static int ping_send(int s, ip_addr_t *addr)
{
	struct icmp_echo_hdr *iecho;
	struct sockaddr_in to;
	int ret;

	if ((iecho = (struct icmp_echo_hdr *)mem_malloc(sizeof(struct icmp_echo_hdr))) == NULL)
		return -1;

	ICMPH_TYPE_SET(iecho,ICMP_ECHO);
	iecho->chksum = 0;
	iecho->seqno = htons(seq_num);
	iecho->chksum = inet_chksum((void*)iecho, sizeof(*iecho));

	to.sin_len = sizeof(to);
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = addr->addr;

	ret = net_sendto(s,(void*)iecho,sizeof(*iecho),0,(struct sockaddr*)&to,sizeof(to));

	mem_free((void*)iecho);
	seq_num++;
	return ret;
}

static int ping_recv(int s, ip_addr_t *addr)
{
	char buf[200];
	socklen_t fromlen;
	int len;
	struct sockaddr_in from;
	LWIP_UNUSED_ARG(addr);

	len = net_recvfrom(s, buf,sizeof(buf),0,(struct sockaddr*)&from,&fromlen);
	
	if(len > 0)
		printc("Received %d bytes from %s\n", len, inet_ntoa(from.sin_addr));
	else
		printc("No response\n");
		
	return len;
}

/* ********************************************************************* */
static int iperfchk_next_seq;
int iperf_check_datagram(struct pbuf *p)
{
	u8_t *protDat, *iperfDat;
	int iperf_j;

	protDat = p->payload;
	if (protDat[0x24] == 0x13 && protDat[0x25] == 0x89 && // iperf port 5001
		protDat[0x17] == 0x11)  // Protocol: UDP
	{
		if (p->next)  iperfDat = p->next->payload;
		else          iperfDat = protDat + 0x2a;
		iperf_j = (iperfDat[0] << 24) | (iperfDat[1] << 16) | (iperfDat[2] << 8) | iperfDat[3];
		if (iperfchk_next_seq != iperf_j) {
			printc("WARN!! iperf: pbuf=0x%x %d (%x) - %d (%x)\n", p, iperfchk_next_seq, iperfchk_next_seq, iperf_j, iperf_j);
		}
		iperfchk_next_seq = iperf_j + 1;
	}
	// p->flags &= ~PBUF_FLAG__BSHYU_DBG_ALL;
	return 0;
}

/*
==============================================================================
@PAR
	bIsServer : 
		1 : iperf server,0 : client.
	szParam : 
		string of parameters.
	pSrcParBuf : 
		Pointer to memeroy that parameters will be filled in this function.
@RETURN 
	0  : parsing OK
	-1 : wrong parameter 
================================================================================
*/
static int iPerfParParsing(u8_t bIsServer,char * szParam,void *pSrcParBuf)
{
	if(bIsServer){
		
		struct IPF_server * pSParBuf = pSrcParBuf;
		pSParBuf->bind_port = 5001;//default port
		
		while(*szParam != 0){
			
			if(!strncmp(szParam,"-u",2)){
				pSParBuf->sotype = IPERF_SOCKET_UDP;//UDP
				printc("UDP \n");
			}
			else
			if(!strncmp(szParam,"-N",2)){
				pSParBuf->bNodelay = 1;
				printc("Nodelay \n");
			}
			else
			if(!strncmp(szParam,"-p",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pSParBuf->bind_port = atoi(szParam);
				printc("bind port = %d \n",pSParBuf->bind_port);
			}
			else
			if(!strncmp(szParam,"-i",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pSParBuf->sStaData.intervel = atoi(szParam);
				printc("intervel = %d s\n",pSParBuf->sStaData.intervel);
			}
			else
			if(!strncmp(szParam,"-w",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pSParBuf->tcp_window = atoi(szParam);
				printc("Server tcp_window = %d \n",pSParBuf->tcp_window);
			}
			else
			if(!strncmp(szParam,"-B",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pSParBuf->bind_ip = inet_addr(szParam);
				pSParBuf->sotype = IPERF_SOCKET_UDP;
				printc("multicast_ip = %s \n",inet_ntoa(pSParBuf->bind_ip));
			}
			else{
				printc(FG_RED("Unsupport command : %s\n"),szParam);
				return -1;
			}
			
			EAT_NON_WHITE(szParam)
			EAT_WHITE(szParam)
		};
	}
	else{
		
		struct IPF_client * pCParBuf = pSrcParBuf;
		
		pCParBuf->pktlength	= 1460;//it's suitable for TCP,1470 for UDP
		pCParBuf->sockaddr.sin_family = AF_INET;
		pCParBuf->sockaddr.sin_port = htons(5001);//default port
		pCParBuf->sockaddr.sin_addr.s_addr = inet_addr(szParam);
		
		if(	pCParBuf->sockaddr.sin_addr.s_addr == IPADDR_NONE ||
			pCParBuf->sockaddr.sin_addr.s_addr == IPADDR_ANY){
			printc(FG_RED("Plz assign dst ip\n"));
			return -1;
		}
		printc("dst ip = %s, port = %d\n",
				inet_ntoa(pCParBuf->sockaddr.sin_addr.s_addr),
				htons(pCParBuf->sockaddr.sin_port));
		EAT_NON_WHITE(szParam)
		EAT_WHITE(szParam)
		
		while(*szParam != 0){
			
			if(!strncmp(szParam,"-u",2)){
				pCParBuf->sotype = IPERF_SOCKET_UDP;//UDP
				printc("UDP \n");
			}
			else
			if(!strncmp(szParam,"-i",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pCParBuf->sClientStaData.intervel = atoi(szParam);
				printc("intervel = %d s\n",pCParBuf->sClientStaData.intervel);
			}
			else
			if(!strncmp(szParam,"-N",2)){
				pCParBuf->bNodelay = 1;
				printc("Nodelay \n");
			}
			else
			if(!strncmp(szParam,"-p",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pCParBuf->sockaddr.sin_port = htons(atoi(szParam));//UDP
				printc("src port = %d \n",htons(pCParBuf->sockaddr.sin_port));
			}
			else
			if(!strncmp(szParam,"-t",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pCParBuf->trans_timeout = 1000*atoi(szParam);
				printc("trans_timeout = %d ms\n",pCParBuf->trans_timeout);
			}
			else
			if(!strncmp(szParam,"-w",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pCParBuf->tcp_window = atoi(szParam);
				printc("Client tcp_window = %d \n",pCParBuf->tcp_window);
			}
			else
			if(!strncmp(szParam,"-l",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pCParBuf->pktlength = atoi(szParam);
				printc("pktlength = %d \n",pCParBuf->pktlength);
			}
			else
			if(!strncmp(szParam,"-n",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pCParBuf->trans_bytes = atoi(szParam);
				printc("trans_bytes = %d time\n",pCParBuf->trans_bytes);
			}
			else
			if(!strncmp(szParam,"-b",2)){
				char * pBdwdchar;
				
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pBdwdchar = szParam;
				pCParBuf->max_bandwidth = atoi(szParam);
				
				while(IS_A_DIGIT(*pBdwdchar))
					pBdwdchar++;
				
				if(*pBdwdchar == 'M'){
					pCParBuf->max_bandwidth *= 1000;
				}
				
				pCParBuf->max_bandwidth /= 8;//bits to Bytes
				printc("max_bandwidth = %d KBytes/sec\n",pCParBuf->max_bandwidth);
			}
			else
			if(!strncmp(szParam,"-S",2)){
				EAT_NON_WHITE(szParam)
				EAT_WHITE(szParam)
				pCParBuf->tos = strtoul(szParam,NULL,16);
				printc("TOS = 0x%x\n",pCParBuf->tos);
			}
			else{
				printc(FG_RED("Not support command : %s\n"),szParam);
				return -1;
			}
			
			EAT_NON_WHITE(szParam)
			EAT_WHITE(szParam)
		};
	}
	
	return 0;
}

double Unit2KM(double org,u32_t div,char * unit)
{
	//note 1Kbps = 1000bps,1 KBytes = 1024 Bytes
	u32_t double_div = div*div;
	
	if(org >= double_div){
		*unit = 'M';
		return org/double_div;
	}
	else if(org >= div){
		*unit = 'K';
		return org/div;
	}
	else{
		*unit = '_';
		return org;
	}
}
/*
================================================================================
iPerfOutputStatistic : Show bandwidth result 
@PAR
	SocketType : 
		IPERF_SOCKET_TCP
		IPERF_SOCKET_UFP
	OuputType : 
		0: periodic output
		1: final output
	pSrcData : 
		Pointer to Statistic data.
================================================================================
*/

static void iPerfOutputStatistic(int SocketType ,int OuputType ,void *pSrcData)
{
	struct IPF_STATISTICS *pStatistic;
	double	dBandwidth;
	double	elapseMS;//milisec
	double	dBps,dBytes;
	char 	cBytesUnit,cBpsUnit;
	
	pStatistic = pSrcData;
	
	if(OuputType){
		
		//output final reslut
		elapseMS = OSTimeGet() - pStatistic->start_time;
		dBandwidth = (double)pStatistic->total_revbytes*8*1000/(double)elapseMS;
		
		dBytes 	= Unit2KM(pStatistic->total_revbytes,1024,&cBytesUnit);
		dBps	= Unit2KM(dBandwidth,1000,&cBpsUnit);
		
		printc(FG_PURPLE("\r\n[%s Port %8d]   0-%.1fsec %.1f%cBytes %.1f%cbps\r\n"),
				(SocketType == IPERF_SOCKET_TCP) ? "TCP" : "UDP" ,
				pStatistic->remote_port,
				elapseMS/1000,
				dBytes,cBytesUnit,
				dBps,cBpsUnit);
	}
	else{
		
		//periodic output
		dBandwidth = (double)pStatistic->intervel_revbytes*8/(double)pStatistic->intervel;
		dBytes	= Unit2KM(pStatistic->intervel_revbytes,1024,&cBytesUnit);
		dBps	= Unit2KM(dBandwidth,1000,&cBpsUnit);
		
		printc(FG_BLUE("\r\n[%s Port %8d]   %3d-%3dsec %.1f%cBytes %.1f%cbps\r\n"),
				(SocketType == IPERF_SOCKET_TCP) ? "TCP" : "UDP",
				pStatistic->remote_port,
				pStatistic->intervel_offset,
				pStatistic->intervel_offset + pStatistic->intervel,
				dBytes,cBytesUnit,
				dBps,cBpsUnit);
		
		osal_mutex_lock(&giPerfStaSem);
		pStatistic->intervel_revbytes = 0;
		osal_mutex_unlock(&giPerfStaSem);
		pStatistic->intervel_offset += pStatistic->intervel;
	}
}

static void iPerfClientUpdate(int SocketType,int OuputType ,void *pSrcData)
{
	struct IPF_CLIENT_STATISTICS *pStatistic;
	double	dBandwidth;
	double	elapseMS;//milisec
	double	dBps,dBytes;
	char 	cBytesUnit,cBpsUnit;
	
	OS_CRITICAL_INIT()
	pStatistic = pSrcData;
	
	if(OuputType){
		
		//output final reslut
		elapseMS = OSTimeGet() - pStatistic->start_time;
		dBandwidth = (double)pStatistic->total_sendbytes*8*1000/(double)elapseMS;
		
		dBytes 	= Unit2KM(pStatistic->total_sendbytes,1024,&cBytesUnit);
		dBps	= Unit2KM(dBandwidth,1000,&cBpsUnit);
		
		printc(FG_PURPLE("\r\n[%s Port %8d]   0-%.1fsec %.1f%cBytes %.1f%cbps\r\n"),
				(SocketType == IPERF_SOCKET_TCP) ? "TCP" : "UDP" ,
				pStatistic->local_port,
				elapseMS/1000,
				dBytes,cBytesUnit,
				dBps,cBpsUnit);
	}
	else{
		
		//periodic output
		dBandwidth = (double)pStatistic->intervel_sendbytes*8/(double)pStatistic->intervel;
		dBytes	= Unit2KM(pStatistic->intervel_sendbytes,1024,&cBytesUnit);
		dBps	= Unit2KM(dBandwidth,1000,&cBpsUnit);
		
		printc(FG_BLUE("\r\n[%s Port %8d]   %3d-%3dsec %.1f%cBytes %.1f%cbps\r\n"),
				(SocketType == IPERF_SOCKET_TCP) ? "TCP" : "UDP",
				pStatistic->local_port,
				pStatistic->intervel_offset,
				pStatistic->intervel_offset + pStatistic->intervel,
				dBytes,cBytesUnit,
				dBps,cBpsUnit);
		OS_ENTER_CRITICAL()
		pStatistic->intervel_sendbytes = 0;
		OS_EXIT_CRITICAL()
		pStatistic->intervel_offset += pStatistic->intervel;
	}
}

static void TCPClientTimer( struct ammo_evmsg *e, void *ss )
{
	if(ss)
		iPerfClientUpdate(IPERF_SOCKET_TCP,0,ss);		
}

static void UDPClientTimer( struct ammo_evmsg *e, void *ss )
{
	if(ss)
		iPerfClientUpdate(IPERF_SOCKET_UDP,0,ss);		
}

static void TCPSessionTimer( struct ammo_evmsg *e, void *ss )
{
	if(ss)
		iPerfOutputStatistic(IPERF_SOCKET_TCP,0,ss);
}

static void UDPSessionTimer( struct ammo_evmsg *e, void *ss )
{
	if(ss)
		iPerfOutputStatistic(IPERF_SOCKET_UDP,0,ss);
}

static void iperf_server_routine(void *p_arg)
{
	int max_fd;
	char *prevbuf;
	struct IPF_server *psServerTempData;
	
	osal_mutex_init(&giPerfStaSem);
	prevbuf = mem_malloc(IPERF_REV_BUF_SIZE);
	
	while(1){
		
		int ret;
		fd_set readSet;
		struct timeval timeout;
		struct IPF_server *psServerHeadData;
		
		//just shelter
		psServerHeadData = gpsiPerfServerData;
		//no timeout!
		timeout.tv_sec  = 0; 
		timeout.tv_usec = 0; 
		
		//>==========================Handle listening TCP =============================>
		FD_ZERO( &readSet );
		max_fd = 0;
		
		psServerTempData = psServerHeadData;
		
		//collect binded TCP socket 
		while(psServerTempData){
			if(psServerTempData->sotype == IPERF_SOCKET_TCP){
				FD_SET( psServerTempData->bind_fd, &readSet );
				if(max_fd < psServerTempData->bind_fd)
					max_fd = psServerTempData->bind_fd;
			}
			psServerTempData = (struct IPF_server *)psServerTempData->pListNextServer;
		}
		
		ret = net_select( max_fd+1, &readSet, NULL, NULL, &timeout ); 

		if (ret == 0){
			//printc("Read TCP listening timeout\n");
		}
		else
		if (ret < 0){
			printc("Read TCP listening error\n");
		}
		else{
			
			psServerTempData = psServerHeadData;
		
			while(psServerTempData){
				
				if(psServerTempData->sotype == IPERF_SOCKET_TCP){
					
					if(FD_ISSET( psServerTempData->bind_fd, &readSet )){
						
						s32_t connfd,addrlen;
						struct sockaddr_in remote;
						
						addrlen = sizeof(struct sockaddr_in);
						
						//Link TCP socket to TCP session of Server list
						if( ( connfd = net_accept( psServerTempData->bind_fd, (struct sockaddr *)&remote, (socklen_t *)&addrlen) ) >= 0 ){
							
							struct IPF_TCP_session *pTCPSessNew,**ppTCPSessTemp;
							
							pTCPSessNew = mem_malloc(sizeof(struct IPF_TCP_session));
							
							if(pTCPSessNew){
								
								memset(pTCPSessNew,0,sizeof(struct IPF_TCP_session));
								ppTCPSessTemp = &psServerTempData->pListTCPsess;
								
								while(*ppTCPSessTemp)
									ppTCPSessTemp = &(*ppTCPSessTemp)->pNextTCPsess;
								
								pTCPSessNew->sock_fd				= connfd;
								pTCPSessNew->sStaData.remote_port 	= htons(remote.sin_port);
								pTCPSessNew->sStaData.intervel 		= psServerTempData->sStaData.intervel;
								pTCPSessNew->sStaData.start_time 	= OSTimeGet();
								
								if(pTCPSessNew->sStaData.intervel)
									pTCPSessNew->pTimer = evmsg_new_timer( 1000 * pTCPSessNew->sStaData.intervel, 0, TCPSessionTimer, (void *)&pTCPSessNew->sStaData );
									
								*ppTCPSessTemp = pTCPSessNew;
								
								net_fcntl( connfd, F_SETFL, O_NONBLOCK );
    						
    							if(psServerTempData->bNodelay)
									net_setsockopt( connfd, SOL_TCP, TCP_NODELAY, &psServerTempData->bNodelay, sizeof(int));
							}
							else{
								
								net_close(connfd);
								printc(FG_RED("Fail to allocate memory in TCP accpet step\n"));
							}
						}
						else{
							printc(FG_RED("Fail to accept iperf connection\n"));
						}
					}
				}
				
				psServerTempData = (struct IPF_server *)psServerTempData->pListNextServer;
			}
		}
		//<==========================Handle listening TCP=============================<
		
		if(check_cmd_idx()) break;
		
		//>==========================Pump and statistic TCP/UDP data=============================>
		FD_ZERO( &readSet );
		max_fd = 0;
		
		psServerTempData = psServerHeadData;
		
		//Collect TCP/UDP socket 
		while(psServerTempData){
			if(psServerTempData->sotype == IPERF_SOCKET_TCP){
				//TCP
				struct IPF_TCP_session *pTCPSessTmp;
				pTCPSessTmp = psServerTempData->pListTCPsess;
				
				while(pTCPSessTmp){
					FD_SET( pTCPSessTmp->sock_fd, &readSet );
					if(max_fd < pTCPSessTmp->sock_fd)
						max_fd = pTCPSessTmp->sock_fd;
					
					pTCPSessTmp = pTCPSessTmp->pNextTCPsess;
				};
			}
			else{
				//UDP
				FD_SET( psServerTempData->bind_fd, &readSet );
				if(max_fd < psServerTempData->bind_fd)
					max_fd = psServerTempData->bind_fd;
			}
			psServerTempData = psServerTempData->pListNextServer;
		}
		
		ret = net_select( max_fd+1, &readSet, NULL, NULL, &timeout ); 
		
		if (ret == 0){
			//printc("Read iperf session timeout\n");
		}
		else
		if (ret < 0){
			printc(FG_RED("Read iperf session error\n"));
		}
		else{
			
			psServerTempData = psServerHeadData;
			
			while(psServerTempData){
				
				s32_t rev_len;
				
				if(psServerTempData->sotype == IPERF_SOCKET_TCP){
					//TCP
					struct IPF_TCP_session **ppTCPSessTmp;
					ppTCPSessTmp = &psServerTempData->pListTCPsess;
					
					while(ppTCPSessTmp && *ppTCPSessTmp){
						
						if(FD_ISSET( (*ppTCPSessTmp)->sock_fd, &readSet )){
							//dump TCP data!!
									
							if((rev_len = net_recv((*ppTCPSessTmp)->sock_fd, prevbuf, IPERF_REV_BUF_SIZE, 0)) > 0){
								osal_mutex_lock(&giPerfStaSem);
								(*ppTCPSessTmp)->sStaData.intervel_revbytes += rev_len;
								osal_mutex_unlock(&giPerfStaSem);
								(*ppTCPSessTmp)->sStaData.total_revbytes += rev_len;
							}
							else{
								//interrupt by remote side
								struct IPF_TCP_session * pTCPSessDel;
								
								//free timer
								evmsg_remove((*ppTCPSessTmp)->pTimer);
								(*ppTCPSessTmp)->pTimer = 0;
								//show statistic data
								iPerfOutputStatistic(IPERF_SOCKET_TCP,1,&(*ppTCPSessTmp)->sStaData);
								//close socket
								net_close((*ppTCPSessTmp)->sock_fd);
								//free TCP session memory and link the previous session to next one.
								pTCPSessDel 	= *ppTCPSessTmp;
								*ppTCPSessTmp 	= (*ppTCPSessTmp)->pNextTCPsess;
								ppTCPSessTmp 	= &pTCPSessDel->pNextTCPsess;
								mem_free(pTCPSessDel);
								continue;
							}
						}
						ppTCPSessTmp = &(*ppTCPSessTmp)->pNextTCPsess;
					}
				
				}
				else{
					//UDP
					if(FD_ISSET( psServerTempData->bind_fd, &readSet )){
						//dump UDP data!!
						struct UDP_datagram *pudpbuf;
						struct sockaddr_in from;
						socklen_t fromlen;
						
						fromlen = sizeof(struct sockaddr_in); 
						
						if((rev_len = net_recvfrom(psServerTempData->bind_fd, prevbuf, IPERF_REV_BUF_SIZE, 0,
							(struct sockaddr*) &from, &fromlen)) < 0){
							printc(FG_RED("UDP rev error!!\n"));
						}
						else
						if(rev_len > 0){
							
							pudpbuf = (struct UDP_datagram *)prevbuf;
							
							if(pudpbuf->id == 0){
								psServerTempData->sStaData.start_time 	= OSTimeGet();
								psServerTempData->sStaData.remote_port 	= htons(from.sin_port);
								psServerTempData->remote_ip 			= htonl(from.sin_addr.s_addr);
								if(psServerTempData->sStaData.intervel){
									psServerTempData->pTimer = evmsg_new_timer( psServerTempData->sStaData.intervel*1000,
																				0,
																				UDPSessionTimer, 
																				&psServerTempData->sStaData );
								}
								printc("Rev Start \n");
							}
							else
							if( htonl(pudpbuf->id) == (-(psServerTempData->sStaData.rev_seq + 1)) ||
								htonl(pudpbuf->id) == psServerTempData->sStaData.rev_seq){
								
								//report statistics to client
								struct sockaddr_in to;
								server_hdr *hdr;
								u32_t now;
								
								now	= OSTimeGet();
								
            					hdr 				= (server_hdr*) (pudpbuf+1);
								hdr->flags			= htonl( 0x80000000 );
            					hdr->total_len1		= htonl( (long) (psServerTempData->sStaData.total_revbytes >> 32) );
            					hdr->total_len2		= htonl( (long) (psServerTempData->sStaData.total_revbytes & 0xFFFFFFFF) );
            					hdr->stop_sec		= htonl( (now - psServerTempData->sStaData.start_time)/1000);
            					hdr->stop_usec		= htonl( (now - psServerTempData->sStaData.start_time)%1000 * 1000);
            					hdr->error_cnt		= htonl( 0 );
            					hdr->outorder_cnt	= htonl( 0 );
            					hdr->datagrams		= htonl( psServerTempData->sStaData.rev_seq );
            					hdr->jitter1		= htonl( 0 );
            					hdr->jitter2		= htonl( 0 );
            					
								to.sin_family 		= AF_INET;
								to.sin_port 		= htons(psServerTempData->sStaData.remote_port);
								to.sin_addr.s_addr 	= htonl(psServerTempData->remote_ip);

								ret = net_sendto( psServerTempData->bind_fd, prevbuf, sizeof(server_hdr)+sizeof(struct UDP_datagram), 0, (struct sockaddr*)&to, sizeof(to) );
								
								if(psServerTempData->sStaData.total_revbytes){
									printc("end UDP\n");
									evmsg_remove(psServerTempData->pTimer);
									psServerTempData->pTimer = 0;
									iPerfOutputStatistic(IPERF_SOCKET_UDP,1,&psServerTempData->sStaData);
									psServerTempData->sStaData.intervel_revbytes = 0 ;
									psServerTempData->sStaData.total_revbytes = 0;
									psServerTempData->sStaData.intervel_offset = 0;
									psServerTempData->sStaData.rev_seq = 0;
								}
								
							}
							else{
								osal_mutex_lock(&giPerfStaSem);
								psServerTempData->sStaData.intervel_revbytes += rev_len;
								osal_mutex_unlock(&giPerfStaSem);
								psServerTempData->sStaData.total_revbytes += rev_len;
								psServerTempData->sStaData.rev_seq = htonl(pudpbuf->id);
							}
							
						}
						else{
						}
					}
				}
				psServerTempData = psServerTempData->pListNextServer;
			}
		}
		//<==========================Pump and statistic TCP/UDP data =============================<
		
		if(check_cmd_idx()) break;
	}
	
	//>=========================Free all of server data and close sockets=========================>
	
	psServerTempData = gpsiPerfServerData;
	//Collect TCP/UDP socket 
	while(psServerTempData){
		
		struct IPF_server *pServerDel;
		
		if(psServerTempData->sotype == IPERF_SOCKET_TCP){
			//TCP
			struct IPF_TCP_session *pTCPSessTmp;
			pTCPSessTmp = psServerTempData->pListTCPsess;
			
			while(pTCPSessTmp){
				struct IPF_TCP_session *pTCPSessDel;
				
				if(pTCPSessTmp->pTimer)
					evmsg_remove(pTCPSessTmp->pTimer);
				net_close(pTCPSessTmp->sock_fd);
				pTCPSessDel = pTCPSessTmp;
				pTCPSessTmp = pTCPSessTmp->pNextTCPsess;
				mem_free(pTCPSessDel);
			}
		}
		//In UDP case , just close binded socket and memory
		if(psServerTempData->pTimer)
			evmsg_remove(psServerTempData->pTimer);
		if(IP_MULTICAST(psServerTempData->bind_ip)){
			struct ip_mreq iPerfMcastAddr;
			memset((void *)&iPerfMcastAddr, 0, sizeof(struct ip_mreq));
			iPerfMcastAddr.imr_interface.s_addr = INADDR_ANY;//0
			iPerfMcastAddr.imr_multiaddr.s_addr = psServerTempData->bind_ip;
			if(0 != net_setsockopt(psServerTempData->bind_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,(char *)&iPerfMcastAddr, sizeof(struct ip_mreq)))
				printc("Fail to leave multicast at %s\n",inet_ntoa(psServerTempData->bind_ip));
			else
				printc("Ready to leave multicast at %s\n",inet_ntoa(psServerTempData->bind_ip));
		}
		net_close(psServerTempData->bind_fd);
		pServerDel = psServerTempData;
		psServerTempData = psServerTempData->pListNextServer;
		mem_free(pServerDel);
	}
	
	mem_free(prevbuf);
	osal_mutex_destroy(&giPerfStaSem);
	gpsiPerfServerData = NULL;
	//<=========================Free all of server data and close sockets=========================<
}

static osal_thread_t iperf_client_routine(void *arg)
{
	int i, j ;
	char *send_buf;
	struct IPF_client *psClentData;
	struct UDP_datagram *udpbuf;
	int ttl=1;
	OS_CRITICAL_INIT()
	
	psClentData = arg;
	
	// Create socket
	switch(psClentData->sotype) {
	case IPERF_SOCKET_TCP : // TCP
		if((psClentData->sock_fd = net_socket(PF_INET, SOCK_STREAM, IP_PROTO_TCP)) < 0){ 
			printc("Unable to create TCP socket for iperf\r\n"); 
			return osal_thread_RETURN; 
		}	
		break;
	case IPERF_SOCKET_UDP : // UDP
		if((psClentData->sock_fd = net_socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
			printc("Unable to create UDP socket for iperf\r\n"); 
			return osal_thread_RETURN; 
		}
		break;
	}
    
	if(net_connect(psClentData->sock_fd, (struct sockaddr*)&psClentData->sockaddr, sizeof(struct sockaddr))<0) {
		printc("Unable to connect to iperf server: ip = %s, port = %d\n", 
				inet_ntoa(psClentData->sockaddr.sin_addr.s_addr),
				htons(psClentData->sockaddr.sin_port)); 
		net_close(psClentData->sock_fd);
		return osal_thread_RETURN; 
	}
	
    //In multicast case : set TTL to 1
    if(IP_MULTICAST(psClentData->sockaddr.sin_addr.s_addr))
    	net_setsockopt(psClentData->sock_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    //Set TOS in IP
    if(psClentData->tos)
    	net_setsockopt(psClentData->sock_fd, IPPROTO_IP, IP_TOS, &psClentData->tos, sizeof(int));
    //Set TCP Nodelay ( disable Nagle's algorithm )
    if(psClentData->sotype == IPERF_SOCKET_TCP && psClentData->bNodelay)
    	net_setsockopt(psClentData->sock_fd, IPPROTO_TCP, TCP_NODELAY, &psClentData->bNodelay, sizeof(int));
    //TODO : Set TCP rev window size,it's now unspported in lwip_setsockopt, and TCP_WND = 2920 is too small.
    
    if( (send_buf = mem_malloc(psClentData->pktlength > 1500 ? 1500 : psClentData->pktlength)) == NULL) {
		printc(FG_RED("Failed to allocate %d byte for Client pktlength\n"),psClentData->pktlength > 1500 ? 1500 : psClentData->pktlength);
		net_close(psClentData->sock_fd);
		return osal_thread_RETURN;
	}
	
	udpbuf = (struct UDP_datagram*)send_buf;
	psClentData->start_time = OSTimeGet();
	psClentData->sClientStaData.intervel_offset = 0;
	psClentData->sClientStaData.local_port = psClentData->sockaddr.sin_port;
	psClentData->sClientStaData.start_time = psClentData->start_time;//miliseconds
    psClentData->sClientStaData.total_sendbytes = 0;
    psClentData->sClientStaData.intervel_sendbytes = 0;
    
    if(psClentData->sClientStaData.intervel){
    	if(psClentData->sotype == IPERF_SOCKET_UDP)
    		psClentData->pTimer = evmsg_new_timer( 1000 * psClentData->sClientStaData.intervel, 0, UDPClientTimer, (void *)&psClentData->sClientStaData );
    	else
    		psClentData->pTimer = evmsg_new_timer( 1000 * psClentData->sClientStaData.intervel, 0, TCPClientTimer, (void *)&psClentData->sClientStaData );
    }
    
	while(1){
		
		u32_t now;
		
		now = OSTimeGet();
    	
    	if(psClentData->max_bandwidth != 0){
    		
    		//Bandwidth control,note 1Mbps = 1000Kbps , 1Kbps = 1000 bps
    		u64_t	ulMaxTransferKbps,ulSentKbps;
    		u32_t	elapse;//milisec
    		
			elapse = now - psClentData->start_time;
			ulMaxTransferKbps = psClentData->max_bandwidth*elapse;
			ulSentKbps = (psClentData->sntbytes+psClentData->pktlength);
			//Compare in Kbps
    		if(	ulMaxTransferKbps <= ulSentKbps){
    		//if(ulMaxTransferKBytes <= ulSentBytes)	{
    			if(psClentData->trans_timeout && (now - psClentData->start_time) > psClentData->trans_timeout)
    				break;
    			if(check_cmd_idx()) 
    				break;  // early exit for the coming commands
    			MMPF_OS_Sleep(10);
    			continue;
    		}
    	}
    	
		switch(psClentData->sotype) {
		case IPERF_SOCKET_UDP : // UDP/send
			udpbuf->id = htonl(psClentData->snt_seq); 
			udpbuf->tv_sec  = htonl(now / 1000); 
			udpbuf->tv_usec = htonl((now % 1000) * 1000); 
			goto fall_through_tcp;
		case IPERF_SOCKET_TCP : // TCP
			fall_through_tcp:
			if (net_send(psClentData->sock_fd, send_buf, psClentData->pktlength, 0) < 0)
				psClentData->trans_bytes = 1;  // break out of for-loop, usually due to iperf-server termination
			break;
		}
		
    	psClentData->snt_seq++;
    	psClentData->sntbytes += psClentData->pktlength;
    	
    	if(psClentData->sClientStaData.intervel){
    		psClentData->sClientStaData.total_sendbytes += psClentData->pktlength;
    		OS_ENTER_CRITICAL()
    		psClentData->sClientStaData.intervel_sendbytes += psClentData->pktlength;
    		OS_EXIT_CRITICAL()
    	}
		//if (delay) MMPF_OS_Sleep(delay);
		
		if(check_cmd_idx()) break;  // early exit for the coming commands
		if(psClentData->trans_timeout && (now - psClentData->start_time) > psClentData->trans_timeout) break;
		if(psClentData->trans_bytes && (psClentData->sntbytes >= psClentData->trans_bytes)) break;
		
	};
	
	if(psClentData->pTimer){
		evmsg_remove(psClentData->pTimer);
		
		if(psClentData->sotype == IPERF_SOCKET_UDP)
			iPerfClientUpdate(IPERF_SOCKET_UDP,1,&psClentData->sClientStaData);
		else
			iPerfClientUpdate(IPERF_SOCKET_TCP,1,&psClentData->sClientStaData);
	}
	
	switch(psClentData->sotype) {
	case IPERF_SOCKET_UDP : // UDP/send
		for (i=0,j=0; i<10; i++) {
			
			fd_set readSet;
			struct timeval timeout;
			
			udpbuf->id = htonl(-(psClentData->snt_seq));
			net_send(psClentData->sock_fd, send_buf, psClentData->pktlength, 0); 
			// wait until the socket is readable, or our timeout expires 
			FD_ZERO( &readSet ); 
			FD_SET( psClentData->sock_fd, &readSet ); 
			timeout.tv_sec  = 0; 
			timeout.tv_usec = 250000; // quarter second, 250 ms 
    
			j = net_select( psClentData->sock_fd+1, &readSet, NULL, NULL, &timeout ); 
			if (j == 0) continue; // timeout
			else {
				
				u64_t				ullen;
				double				dBandwidth;
				double				elapseMS;//milisec
				double				dBps,dBytes;
				char 				cBytesUnit,cBpsUnit;
				struct UDP_datagram *pudpbuf;
				server_hdr 			*hdr;
								
				net_read(psClentData->sock_fd, send_buf, psClentData->pktlength);
				
				pudpbuf				= (struct UDP_datagram *)send_buf;
				hdr 				= (server_hdr*) (pudpbuf+1);
            	
				//output server report
				elapseMS 	= htonl(hdr->stop_sec)*1000 + htonl(hdr->stop_usec)/1000;
				ullen	 	= ((u64_t)htonl(hdr->total_len1) << 32) | htonl(hdr->total_len2);
				dBandwidth 	= (double)ullen*8*1000/(double)elapseMS;
				dBytes 		= Unit2KM(ullen,1024,&cBytesUnit);
				dBps		= Unit2KM(dBandwidth,1000,&cBpsUnit);
				
				printc("Server report\r\n");
				printc("[UDP Port %8d]   0-%.1fsec %.1f%cBytes %.1f%cbps\r\n",
						psClentData->sClientStaData.local_port,
						elapseMS/1000,
						dBytes,cBytesUnit,
						dBps,cBpsUnit);
				break;
			}
		}
		break;
	}

	net_close(psClentData->sock_fd);
	mem_free(send_buf);
	mem_free(psClentData);
}

void __uartCmd_iperf(char* szParam, int help)
{
	char 	*pParBuf;
	int 	IsServer = 0;

	//Acoording to iperf usage in version 2.0.5 
	if (help){ 
		printc("net iperf <-c|-s|-r> <ip (client)> ...\r\n");
		printc("-c	Client\r\n ");
		printc("-s	Server\r\n ");
		printc("-i	Interval\r\n ");
		printc("-t	(Client)Transfer timeout\r\n");
		printc("-B	Bind host\r\n ");
		printc("-N	TCP Nodelay\r\n ");
		printc("-w	TCP window\r\n");
		printc("-l	length of packet\r\n");
		printc("-n	(Client)Number of bytes to transmit <instead of -t>\r\n");
		printc("-b	(Client) Max bandwidth\r\n");
		printc("-p	Port\r\n ");
		printc("-u	UDP ,default is TCP\r\n");
		printc("-S	(Client) ToS (hex without \"0x\")\r\n");
		printc("-r	Restore UART and network task's priority\r\n\r\n ");
		return;
	}

	EAT_WHITE(szParam);
	pParBuf = NULL;
	
	if(!strncmp(szParam,"-c",2)){
		if((pParBuf = mem_malloc(sizeof(struct IPF_client))) == NULL){
			printc(FG_RED("allocate mem failed.\r\n"));
			return;
		}
		memset(pParBuf,0,sizeof(struct IPF_client));
	}
	else
	if(!strncmp(szParam,"-s",2)){
		if((pParBuf = mem_malloc(sizeof(struct IPF_server))) == NULL){
			printc(FG_RED("allocate mem failed.\r\n"));
			return;
		}
		memset(pParBuf,0,sizeof(struct IPF_server));
		IsServer = 1;
	}
	else
	if(!strncmp(szParam,"-r",2)){
		//Restore task priority
		OSTaskChangePrio(TASK_NETWORK_BRUST_PRIO , TASK_NETWORK_PRIO);
		OSTaskChangePrio(TASK_UART_BRUST_PRIO , TASK_JOB_DISPATCH_PRIO);
		AHC_SendAHLMessageEnable(1);
		return;
	}
	else{
		printc(FG_RED("Plz enter -c | -s | -r as first parameter\r\n"));
		return;
	}
	
	EAT_NON_WHITE(szParam)//eat "-s" or "-c" or "-r"
	EAT_WHITE(szParam)
	
	if(iPerfParParsing(IsServer,szParam,(void *)pParBuf)){
		mem_free(pParBuf);
		printc(FG_RED("Fail to parsing.\r\n"));
		return;
	}
	
	if(IsServer){
		
		struct IPF_server *psServerMem;
		struct sockaddr_in addr;
		
		psServerMem = (struct IPF_server *) pParBuf;
		
		//Create and bind server's socket 
		if(psServerMem->sotype == IPERF_SOCKET_TCP){
		
			//TCP case
			if( ( psServerMem->bind_fd = net_socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
				printc("Fail to create iperf TCP server's socket\n");
				mem_free(pParBuf);
				return;
			}
			
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = 0;
			addr.sin_port = htons( psServerMem->bind_port );
			
			if( net_bind( psServerMem->bind_fd, (struct sockaddr *)&addr, sizeof( addr ) ) < 0 ) {
				printc("Fail to bind iperf TCP server's socket\n");
				net_close( psServerMem->bind_fd );
				mem_free(pParBuf);
				return;
			}
			
			if(psServerMem->bNodelay)
				net_setsockopt( psServerMem->bind_fd, SOL_TCP, TCP_NODELAY, &psServerMem->bNodelay, sizeof(int));
				
			if( net_listen( psServerMem->bind_fd, 5 ) < 0 ) {
				printc("Fail to setup iperf TCP server's listen");
				net_close( psServerMem->bind_fd );
				mem_free(pParBuf);
				return;
			}
			
		}
		else{
			
			//UDP case
			if( ( psServerMem->bind_fd = net_socket( PF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
				printc("Fail to create iperf UDP server's socket\n");
				mem_free(pParBuf);
				return;
			}

			if(IP_MULTICAST(psServerMem->bind_ip)){
				
				struct ip_mreq iPerfMcastAddr;
				
				memset((void *)&iPerfMcastAddr, 0, sizeof(struct ip_mreq));
				iPerfMcastAddr.imr_interface.s_addr = INADDR_ANY;//0
				iPerfMcastAddr.imr_multiaddr.s_addr = psServerMem->bind_ip;
				
				if(0 != net_setsockopt(psServerMem->bind_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char *)&iPerfMcastAddr, sizeof(struct ip_mreq)))
					printc("Fail to join multicast at %s\n",inet_ntoa(psServerMem->bind_ip));
				else
					printc("Ready to join multicast at %s\n",inet_ntoa(psServerMem->bind_ip));
			}
			
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = psServerMem->bind_ip;
			addr.sin_port = htons(psServerMem->bind_port);
			
			if( net_bind( psServerMem->bind_fd, (struct sockaddr *)&addr, sizeof( addr ) ) < 0 ) {
				printc("Fail to bind iperf UDP server's socket\n");
				net_close( psServerMem->bind_fd );
				mem_free(pParBuf);
				return;
			}
		}
		
		//launch server!
		if(gpsiPerfServerData == NULL){
			
			//the frist time to create server task
			MMPF_TASK_CFG   task_cfg;
			
			AHC_SendAHLMessageEnable(0);
			OSTaskChangePrio(TASK_NETWORK_PRIO, TASK_NETWORK_BRUST_PRIO);
			OSTaskChangePrio(TASK_JOB_DISPATCH_PRIO, TASK_UART_BRUST_PRIO);
			
			gpsiPerfServerData = psServerMem;
			task_cfg.ubPriority = IPERF_SERVER_PRIO;
    		task_cfg.pbos = (MMP_ULONG)&iPerf_Server_Task_Stk[0];
    		task_cfg.ptos = (MMP_ULONG)&iPerf_Server_Task_Stk[IPERF_SERVER_STK_SIZE-1];
    		MMPF_OS_CreateTask(iperf_server_routine, &task_cfg, (void *)0);
    		MMPF_OS_SetTaskName(task_cfg.ubPriority, (INT8U *)"iperf");
    		printc("Create iperf server task priorty = %d \n",IPERF_SERVER_PRIO);
    		
		}
		else{
			
			//server task is running ! just link the list data.
			struct IPF_server * psServerList = gpsiPerfServerData;
			
			while(psServerList->pListNextServer != NULL)
				psServerList = (struct IPF_server *)psServerList->pListNextServer;
					
			psServerList->pListNextServer = psServerMem;
		}
	}
	else{
		
		AHC_SendAHLMessageEnable(0);
		OSTaskChangePrio(TASK_NETWORK_PRIO, TASK_NETWORK_BRUST_PRIO);
		OSTaskChangePrio(TASK_JOB_DISPATCH_PRIO, TASK_UART_BRUST_PRIO);
			
		osal_thread_new( iperf_client_routine, (void *)pParBuf );
	}
	
}

/* ********************************************************************* */
void __uartCmd_ping(char* szParam, int help)
{
	char ip[20];
	int times = 10, s;
	int recv_timeout = 1000;
	
	if (help) {
		printc("net ping <ip> <times>\r\n");
		return;
	}

	sscanfl( szParam, "%s %d", ip, &times);
	ip4_addr_set_u32( &ping_addr, inet_addr(ip) );
	if ((s = net_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0)
		return;
	
    net_fcntl( s, F_SETFL, O_NONBLOCK );
    
	while (times-- > 0) {
		printc("sending ping\n");

		ping_send(s,&ping_addr);
		//1s to wait for reciving,note ping_recv is under nonblocking mode.
		MMPF_OS_Sleep(recv_timeout);
		ping_recv(s,&ping_addr);
		
		if (check_cmd_idx()) break;  // early exit for the coming commands
	}
}

/************************************************************************
 * reset NETWORK
 */
//int Reset_Network(void);

void __uartCmd_reset(char* szParam, int help)
{
	netapp_ResetNetwork();
}

#if LWIP_STATS
struct stats_ lwip_stats_snapshot = {0};
WLAN_STATISTICS_INFO wlan_stats = {0};
WLAN_STATISTICS_INFO wlan_stats_snapshot = {0};

#define _FS_  "\t"
#define _FB_  "\n\t"

void __show_wlan_stats(WLAN_STATISTICS_INFO *w)
{

	#define __show_bcm_stats1( _s, _fs)  printc( "%-18s %8u" _fs, #_s ":", w->dstats._s )
	#define __show_bcm_stats2( _s, _fs)  printc( "%-18s %8u" _fs, #_s ":", w->_s )

	// dngl_stats_t dstats;	/* Stats for dongle-based data */
	//----------------------------------------------------------------------
	printc("<<WLAN-Dongle>>\n\t");
	__show_bcm_stats1( rx_packets, _FS_ );		/* total packets received */
	__show_bcm_stats1( tx_packets, _FS_ );		/* total packets transmitted */
	__show_bcm_stats1( rx_bytes,   _FS_ );		/* total bytes received */
	__show_bcm_stats1( tx_bytes,   _FB_ );		/* total bytes transmitted */
	__show_bcm_stats1( rx_errors,  _FS_ );		/* bad packets received */
	__show_bcm_stats1( tx_errors,  _FS_ );		/* packet transmit problems */
	__show_bcm_stats1( rx_dropped, _FS_ );		/* packets dropped by dongle */
	__show_bcm_stats1( tx_dropped, _FB_ );		/* packets dropped by dongle */
	__show_bcm_stats1( multicast,  "\n" );      /* multicast packets received */

	/* Additional stats for the bus level */
	//----------------------------------------------------------------------
	printc("<<WLAN-LLC>>\n\t");
	__show_bcm_stats2( tx_packets,   _FS_ );	/* Data packets sent to dongle */
	__show_bcm_stats2( tx_multicast, _FS_ );	/* Multicast data packets sent to dongle */
	__show_bcm_stats2( tx_errors,    _FS_ );	/* Errors in sending data to dongle */
	__show_bcm_stats2( tx_ctlpkts,   _FB_ );	/* Control packets sent to dongle */
	__show_bcm_stats2( tx_ctlerrs,   _FS_ );	/* Errors sending control frames to dongle */
	__show_bcm_stats2( rx_packets,   _FS_ );	/* Packets sent up the network interface */
	__show_bcm_stats2( rx_multicast, _FS_ );	/* Multicast packets sent up the network interface */
	__show_bcm_stats2( rx_errors,    _FB_ );	/* Errors processing rx data packets */
	__show_bcm_stats2( rx_ctlpkts,   _FS_ );	/* Control frames processed from dongle */
	__show_bcm_stats2( rx_ctlerrs,   _FS_ );	/* Errors in processing rx control frames */
	__show_bcm_stats2( rx_dropped,   _FS_ );	/* Packets dropped locally (no memory) */
	__show_bcm_stats2( rx_flushed,   _FB_ );	/* Packets flushed due to unscheduled sendup thread */
	__show_bcm_stats2( wd_dpc_sched, _FS_ );	/* Number of times dhd dpc scheduled by watchdog timer */

	__show_bcm_stats2( rx_readahead_cnt, _FS_ );	/* Number of packets where header read-ahead was used. */
	__show_bcm_stats2( tx_realloc,       _FS_ );	/* Number of tx packets we had to realloc for headroom */
	__show_bcm_stats2( fc_packets,       "\n" );	/* Number of flow control pkts recvd */
}

void __uartCmd_show_statistics(char* szParam, int diff, int help)
{
	char subCMD[16];
	int i, len;
	int showstat_wlan = 0;
	int showstat_link = 0;
	int showstat_arp = 0;
	int showstat_ipfrag = 0;
	int showstat_ip = 0;
	int showstat_icmp = 0;
	int showstat_igmp = 0;
	int showstat_udp = 0;
	int showstat_tcp = 0;
	int showstat_mem = 0;
	int showstat_memp = 0;
	int showstat_sys = 0;
	int showstat_sock = 0;
	int showstat_all = 0;
	struct stats_ *pst;
	STAT_COUNTER *pCnt1, *pCnt2;
	WLAN_STATISTICS_INFO *pwlan;
	unsigned long *wlan_pCnt1, *wlan_pCnt2;
	char *sdio_stat_buf = NULL;

	if (help) {
L_help:
		printc("net stat  (all | wlan | link | arp | ipfrag | ip | icmp | igmp | udp | tcp | mem | memp | sys | sock)\n");
		printc("          show the lwip_stats current snapshot values\n");
		printc("net stat2 (all | wlan | link | arp | ipfrag | ip | icmp | igmp | udp | tcp | mem | memp | sys | sock)\n");
		printc("          show the lwip_stats current snapshot values and difference with the last\n");
		return;
	}

	if (diff != -1) {
		sdio_stat_buf = mem_malloc( 8192 );
		WLAN_SYS_GetStatsInfo( &wlan_stats, sdio_stat_buf, 8192 );  // refresh the recent WLAN statistics values
	}

	if (diff == 1) {
		printc("lwip_stats snapshot\n");
		printc("=========================================================================================\n");
		__uartCmd_show_statistics(szParam, -1, 0);  // do a snapshot output first
		printc("\n\nlwip_stats difference\n");
		printc("=========================================================================================\n");

		// calculate the statistics difference for LWIP
		pCnt1 = (STAT_COUNTER*)&lwip_stats_snapshot;
		pCnt2 = (STAT_COUNTER*)&lwip_stats;
		for (i=0; i<sizeof(lwip_stats)/sizeof(STAT_COUNTER); i++) {
			*pCnt1 = *pCnt2 - *pCnt1;
			pCnt1++; pCnt2++;
		}

		// calculate the statistics difference for WLAN
		wlan_pCnt1 = (unsigned long*)&wlan_stats_snapshot;
		wlan_pCnt2 = (unsigned long*)&wlan_stats;
		for (i=0; i<sizeof(WLAN_STATISTICS_INFO)/sizeof(unsigned long); i++) {
			*wlan_pCnt1 = *wlan_pCnt2 - *wlan_pCnt1;
			wlan_pCnt1++; wlan_pCnt2++;
		}

		pst = &lwip_stats_snapshot;
		pwlan = &wlan_stats_snapshot;
	}
	else {
		pst = &lwip_stats;
		pwlan = &wlan_stats;
	}

	len = strlen(szParam);
	while (len > 0) {
		sscanfl( szParam, "%s", subCMD );
		i = strlen(subCMD) + 1;
		szParam += i;
		len -= i;

		if (!strcmp(subCMD, "wlan"))    showstat_wlan = 1;
		else
		if (!strcmp(subCMD, "link"))    showstat_link = 1;
		else
		if (!strcmp(subCMD, "arp"))     showstat_arp = 1;
		else
		if (!strcmp(subCMD, "ipfrag"))  showstat_ipfrag = 1;
		else
		if (!strcmp(subCMD, "ip"))      showstat_ip = 1;
		else
		if (!strcmp(subCMD, "icmp"))    showstat_icmp = 1;
		else
		if (!strcmp(subCMD, "igmp"))    showstat_igmp = 1;
		else
		if (!strcmp(subCMD, "udp"))     showstat_udp = 1;
		else
		if (!strcmp(subCMD, "tcp"))     showstat_tcp = 1;
		else
		if (!strcmp(subCMD, "mem"))     showstat_mem = 1;
		else
		if (!strcmp(subCMD, "memp"))    showstat_memp = 1;
		else
		if (!strcmp(subCMD, "sys"))     showstat_sys = 1;
		else
		if (!strcmp(subCMD, "sock"))    showstat_sock = 1;
		else
		if (!strcmp(subCMD, "all"))     showstat_all = 1;
		else
			goto L_help;
	}

	if (diff == 1) {
		// difference is meaningless for struct stats_mem: they are not monotonically incremental
		showstat_mem  = -1;
		showstat_memp = -1;
	}

	if (showstat_all || showstat_link)     LINK_STATS_DISPLAY(pst);
	if (showstat_all || showstat_arp)      ETHARP_STATS_DISPLAY(pst);
	if (showstat_all || showstat_ipfrag)   IPFRAG_STATS_DISPLAY(pst);
	if (showstat_all || showstat_ip)       IP_STATS_DISPLAY(pst);
	if (showstat_all || showstat_icmp)     ICMP_STATS_DISPLAY(pst);
	if (showstat_all || showstat_igmp)     IGMP_STATS_DISPLAY(pst);
	if (showstat_all || showstat_udp)      UDP_STATS_DISPLAY(pst);
	if (showstat_all || showstat_tcp)      TCP_STATS_DISPLAY(pst);
	if (showstat_all || showstat_sys)      SYS_STATS_DISPLAY(pst);
	if (showstat_all || showstat_sock)     SOCKET_STATS_DISPLAY(pst);

	if ((showstat_all || showstat_mem) && (showstat_mem != -1))
		MEM_STATS_DISPLAY(pst);
	if ((showstat_all || showstat_memp) && (showstat_memp != -1)) {
		for (i = 0; i < MEMP_MAX; i++)
			MEMP_STATS_DISPLAY(pst,i);
	}
	if (showstat_all || showstat_wlan) {
		__show_wlan_stats(pwlan);
		if (sdio_stat_buf) {
			printc("\n-----------------------------------------------------------------------------------------\n");
			printstr( sdio_stat_buf );
		}
	}

	if (diff != -1) {
		memcpy( &lwip_stats_snapshot, &lwip_stats, sizeof(lwip_stats) );
		memcpy( &wlan_stats_snapshot, &wlan_stats, sizeof(wlan_stats) );
		if (sdio_stat_buf)
			mem_free( (void*)sdio_stat_buf );
	}
}
#else
void __uartCmd_show_statistics(char* szParam, int diff, int help) {}
#endif

void __uartCmd_netconfig(char* szParam, int help)
{
	char ip[16]="", gw[16]="";

	if (help) {
		printc("net cfg  <ip> <gw>     set up the network interface to static IP\n");
		printc("net cfg  dhcp          set up the network interface to DHCP\n");
		printc("net cfg                show   the network config\n");
		return;
	}

	sscanfl( szParam, "%s %s", ip, gw );
	printc("Your input is: '%s' '%s' \r\n", ip, gw);

	if (*ip == 0)
		LwIP_show_netconfig();
	else if (!strcmp(ip,"dhcp"))
		LwIP_start_netif( "dhcp", NULL );
	else
		LwIP_start_netif( ip, gw );
}

void __uartCmd_kill(char* szParam, int help)
{
	int i;
	if (help) {
		printc("net kill               kill the outstanding UartCommand\n");
		return;
	}

	for (i=UartNetIdx_pop; i != UartNetIdx_push; i=(i+1)%NET_UART_CMD_NUM)
		printc("UARTCMD[%d]: %s\n", i, UartNetCmdArray[i]);
	UartNet_KillBlockedCmd = 1;
	MMPF_OS_Sleep(1000);
}

/* ********************************************************************* */
void __uartCmd_add_WlanFavAP(char* szParam, int help)
{
	int crypto;
	char ssid[32], key[32];

	if (help) {
		printc("wifi add  <SSID> <crypto> <key>    add the favorite list of WLAN APs, and join automatically \n");
		printc("                                   where the <crypto> can be:  0 none,  1 AES,  2 WEP\n");
		return;
	}
	sscanfl( szParam, "%s %d %s", ssid, &crypto, key );
	AddWlanFavAP( -1, ssid, crypto, key );
	printc("After added new SSID %s :\n", ssid);
	ShowWlanFavAP();
}

void __uartCmd_del_WlanFavAP(char* szParam, int help)
{
	int i;
	char ssid[32];

	if (help) {
		printc("wifi del <SSID>                    delete all APs with this SSID from the favorite list\n");
		printc("wifi del <idx>                     delete the <idx>'th entry from the favorite list of WLAN APs\n");
		return;
	}

	sscanfl( szParam, "%s", ssid );
	if (isdigit(ssid[0])) {
		i = atoi(ssid);
		printc("Will delete AP[%d]\n", i);
		DelWlanFavAP_idx( i );
	}
	else
		DelWlanFavAP_ssid( ssid );

	printc("After deleted SSID/idx %s :\n", ssid);
	ShowWlanFavAP();
}

void __uartCmd_show_wlan(char* szParam, int help)
{
	if (help) {
		printc("wifi show                          display the IP/WLAN config and favorite APs list\n");
		return;
	}

	ShowWlanFavAP();
	LwIP_show_netconfig();
}

void __uartCmd_ssid(char* szParam, int help)
{
	int	show;
	
ssid_help:
	if (help) {
		printc("wifi ssid [1|0] show|hide ssid\n");
		return;
	}
	if (sscanfl(szParam, "%d", &show) == 1) {
		if (netapp_BroadcastSSID(show))
			printc("OK\n");
		else
			printc("FAILED\n");
	} else {
		help = 1;
		goto ssid_help;
	}
}

#ifdef WPS_AP_SUPPORT
int wps_forAPmode(int bEnabled);
#endif

void __uartCmd_wps(char* szParam, int help)
{
	int	enable;
	
wps_help:
	if (help) {
		printc("wifi wps [1|0] enable|disable WPS in AP mode\n");
		return;
	}
	if (sscanfl(szParam, "%d", &enable) == 1) {
	#ifdef WPS_AP_SUPPORT
		if (wps_forAPmode(enable))
			printc("WPS %s\n", (enable == 1)? "Enabled" : "Disabled");
		else
	#endif
			printc("WPS ERROR\n");
	} else {
		help = 1;
		goto wps_help;
	}
}
void __uartCmd_join_wlan(char* szParam, int help)
{
	int crypto;
	char ssid[32]="", key[32], ip[16], gw[16];

	if (help) {
L_help:
		printc("wifi join                                               automatically scan & join to the favorite APs\n");
		printc("wifi join <SSID> <crypto> <key>                         join to the WLAN AP, with default IP/DHCP\n");
		printc("wifi join <SSID> <crypto> <key> [ dhcp | <ip> <gw> ]    join to the WLAN AP, with specified IP/DHCP\n");
		printc("                                                        where the <crypto>:  0 none,  1 AES,  2 WEP\n");
		return;
	}

	sscanfl( szParam, "%s %d %s %s %s", ssid, &crypto, key, ip, gw );

	// the NETWORK may not be started yet
	NETWORK_init();

	if (!*ssid) {
		printc("Joining to scanned AP automatically\n");
		LwIP_join_WLAN_auto(10);
		LwIP_start_netif( NULL, NULL );
		return;
	}

	printc("Joining to AP: %s \t\t", ssid);
	switch (LwIP_join_WLAN_AP( ssid, crypto, key, 20 )) {
	case 1:
		printc("... failed\n");
		return;
	case 0:
		printc("joined sucessful\n");
		break;
	default:
		netif_set_down(&main_netdev);
		printc("Your input is: %s %d %s %s %s \n", ssid, crypto, key, ip, gw );
		goto L_help;
	}

	if (*ip == 0 || !strcmp(ip,"dhcp"))
		LwIP_start_netif( "dhcp", NULL );
	else
		LwIP_start_netif( ip, gw );
}

/* ********************************************************************* */
// return 0 if the command is processed immediately
// return 1 if the command is processed by the task: UartCmd_ProcessAgent
int __cli_process_engine(int cmd_type, char* szParam)
{
	char cmd[16], subCMD[16];
	char *subPARM;

	if (szParam) {
		switch(cmd_type) {
		case NETCMD_NET:
			push_uart_cmd( "net     ", szParam );
			break;
		case NETCMD_WIFI:
			push_uart_cmd( "wifi    ", szParam );
			break;
		}
		return 0;
	}	
	szParam = pop_uart_cmd();
	if (szParam) {
		sscanfl( szParam, "%s %s", cmd, subCMD );
		subPARM = strstr( szParam, subCMD ) + strlen(subCMD);
		if (!strcmp(cmd, "net")) {
			if (!strcmp(subCMD, "iperf"))		__uartCmd_iperf(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "ping"))	__uartCmd_ping(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "stat"))   __uartCmd_show_statistics(subPARM, 0, NETCMD_EXEC);
			else if (!strcmp(subCMD, "stat2"))  __uartCmd_show_statistics(subPARM, 1, NETCMD_EXEC);
			else if (!strcmp(subCMD, "cfg"))    __uartCmd_netconfig(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "kill"))   __uartCmd_kill(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "ass"))    UartCmd_ASSCommands(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "reset"))	netapp_ResetNetwork();
			else if (!strcmp(subCMD, "on"))		netapp_InitNetwork();
			else if (!strcmp(subCMD, "off"))	netapp_StopNetwork();
			else if (!strcmp(subCMD, "brca"))	netapp_UINotifyChanged(NULL);
			else if (!strcmp(subCMD, "help"))   goto L_help;
			else goto L_unknown;
		} else
		if (!strcmp(cmd, "wifi")) {
			if (!strcmp(subCMD, "join"))		__uartCmd_join_wlan(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "show"))   __uartCmd_show_wlan(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "add"))    __uartCmd_add_WlanFavAP(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "del"))    __uartCmd_del_WlanFavAP(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "ssid"))   __uartCmd_ssid(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "wps"))	__uartCmd_wps(subPARM, NETCMD_EXEC);
			else if (!strcmp(subCMD, "help"))   goto L_help;
			else goto L_unknown;
		}
		else {
L_unknown:
			printc("Unknown NET command: '%s'\r\n", cmd);
L_help:
			__uartCmd_show_statistics( NULL, 0, NETCMD_HELP );
			printc("\r\n");
			__uartCmd_iperf( NULL, NETCMD_HELP );
			__uartCmd_ping( NULL, NETCMD_HELP );
			__uartCmd_netconfig( NULL, NETCMD_HELP );
			__uartCmd_kill( NULL, NETCMD_HELP );
			UartCmd_ASSCommands( NULL, NETCMD_HELP );
			printc("\r\n");

			__uartCmd_join_wlan( NULL, NETCMD_HELP );
			printc("\r\n");
			__uartCmd_show_wlan( NULL, NETCMD_HELP );
			__uartCmd_add_WlanFavAP( NULL, NETCMD_HELP );
			__uartCmd_del_WlanFavAP( NULL, NETCMD_HELP );
			printc("\r\n");
		}
	}
	return 0;
}

/* ********************************************************************* */
// The Main interface functions to Firmware/misc/UartCommandSets01.c
/* ********************************************************************* */
void UartCmd_NETCommands(char* szParam)
{
	__cli_process_engine( NETCMD_NET, szParam );
}

void UartCmd_WifiCommands(char* szParam)
{
	__cli_process_engine( NETCMD_WIFI, szParam );
}

void UartCmd_ProcessAgent()
{
	__cli_process_engine( NETCMD_AGENT, NULL );
}

extern int load_wificonfig(int apset, char **pssid, int *pcrypto, char **pckey, int *hidden, int *pchan);

#ifdef WPS_AP_SUPPORT
int wps_forAPmode(int bEnabled)
{
	int			crypto, chan;
	char		*ssid, *ckey;
	int			hidden;
	int			apset;
	wps_config_info_t wps_config_info;

	if (bEnabled == 0) {
		AP_StopWPS((void*)1);
		return 1;
	}
	apset = netapp_GetWiFi_AP_set();
	load_wificonfig(apset, &ssid, &crypto, &ckey, &hidden, &chan);
	memset(&wps_config_info, 0, sizeof(wps_config_info));
	wps_config_info.WscMode = 0x02;	//PBC
	switch (crypto) {
	case WPA_CRYPTO__AES:
		wps_config_info.WscSecurityMode = WPAPSKAES;
		break;
	case WPA_CRYPTO__TKIP:
		wps_config_info.WscSecurityMode = WPAPSKTKIP;
		break;
	case WPA_CRYPTO__WEP:
	default:
		printc("%s %d: ERROR in CRYPTO %d\n", __func__, __LINE__, crypto);
		return 0;
	}
	// KEY
	wps_config_info.WscSecurityMode = WPA2PSKAES;
	strncpy((char*)wps_config_info.WpaPsk, ckey, WPAKEY_LEN);
	//memcpy(wps_config_info.WpaPsk, ckey, strlen("1234567890"));
	wps_config_info.WpaPskLen = strlen((char*)wps_config_info.WpaPsk);
	// SSID
	strncpy((char*)wps_config_info.Ssid.Ssid, ssid, SSID_LEN);
	//memcpy(wps_config_info.Ssid.Ssid, ssid, strlen(ssid));
	wps_config_info.Ssid.SsidLength = strlen((char*)wps_config_info.Ssid.Ssid);
	wps_config_info.channel = chan;
	wps_config_info.ssidBcst = 0;
	AP_StartWPS(&wps_config_info, NULL);
	return 1;
}
#endif