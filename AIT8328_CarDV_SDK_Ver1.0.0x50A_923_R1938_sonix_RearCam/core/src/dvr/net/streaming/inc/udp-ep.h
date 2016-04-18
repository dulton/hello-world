/*
*********************************************************************************************************
*                         AIT Multimedia Network Streaming Server
*                     (c) Copyright 2012, Alpha Imaging Technology Corp.
*               All rights reserved. Protected by international copyright laws.
*********************************************************************************************************
*
* Description:
*
*/
#ifndef __UDP_EP_H___
#define __UDP_EP_H___

//*******************************************************************************************************
/* These should probably be an enum */
#define	RTP_TRANS_UDP	1
#define RTP_TRANS_INTER	2

//*******************************************************************************************************
struct udp_endpoint;

//*******************************************************************************************************
typedef int (*udp_media_get_sdp_func)( char *dest, int len, int payload, int port, void *d );
typedef int (*udp_media_get_payload_func)( int payload, void *d );
typedef int (*udp_media_frame_func)( struct ammo_frame *f, void *d );
typedef int (*udp_media_send_func)( struct udp_endpoint *ep, void *d );
typedef int (*udp_media_close_func)( void *d );

struct udp_media {
	udp_media_get_sdp_func     get_sdp;
	udp_media_get_payload_func get_payload;
	udp_media_frame_func       proc_frame;
	udp_media_send_func        send_frame;
	udp_media_close_func       close_frame;
	void *private;
};

struct udp_endpoint {
	struct amn_session_t *session;
	int payload;
	int max_data_size;
	unsigned int ssrc;
	unsigned int start_timestamp;
	unsigned int last_timestamp;
	int seqnum;
	int packet_count;
	int octet_count;
	struct ammo_evmsg *rtcp_send_event;
	int force_rtcp;
	struct timeval last_rtcp_recv;
	int trans_type;
	union {
		struct {
			char sdp_addr[48];
			int sdp_port;
			int rtp_fd;
			struct ammo_evmsg *rtp_event;
			int rtcp_fd;
			struct ammo_evmsg *rtcp_event;
			struct in_addr dest_addr; //for r7770
		} udp;
		struct {
			struct resp_message_t *conn;
			int rtp_chan;
			int rtcp_chan;
		} inter;
	} trans;
};


////////////////////////////////////////////////////////////////////////////////
struct udp_media *new_udp_media( udp_media_get_sdp_func get_sdp,
        udp_media_get_payload_func get_payload, udp_media_frame_func proc_frame,
        udp_media_send_func send_frame, udp_media_close_func close_frame, void *private );
/*struct rtp_media *new_rtp_media_mpeg4(void);
struct rtp_media *new_rtp_media_mpv(void);
struct rtp_media *new_rtp_media_h263_stream( struct ammo_pipe *p );
struct rtp_media *new_rtp_media_jpeg_stream( struct ammo_pipe *p );
struct rtp_media *new_rtp_media_mpeg4gen_aachbr(void);
struct rtp_media *new_rtp_media_mpa(void);
struct rtp_media *new_rtp_media_h264(void);
struct rtp_media *new_rtp_media_rawaudio_stream( struct ammo_pipe *p );
*/
////////////////////////////////////////////////////////////////////////////////

#endif

