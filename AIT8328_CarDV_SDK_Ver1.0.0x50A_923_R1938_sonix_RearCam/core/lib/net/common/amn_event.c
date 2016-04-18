/*
*********************************************************************************************************
*                         AIT Multimedia Network Streaming Server
*                     (c) Copyright 2012, Alpha Imaging Technology Corp.
*               All rights reserved. Protected by international copyright laws.
*********************************************************************************************************
*
* Description: Event Manager handling the delivery of messages across Nodes & Pipelines
*              The implemenation relies on POSIX message queues on Linux (man mq_overview)
*              and Message Queue on uC/OS-II (OSQPost / OSQPend)
*/
#include "inc/amn_osal.h"
#include "inc/amn_event.h"

#define AMN_EVMAN_TOTAL_EVMSGS     (AMN_EVMAN_MSGQ_DEPTH + 32)

#ifndef EMDBG_VERBOSE_LEVEL
#define EMDBG_VERBOSE_LEVEL        1
#endif

#if EMDBG_VERBOSE_LEVEL == 2
#define DBGL_EVMSG_0               DBGL_VERBOSE
#define DBGL_EVMSG_1               DBGL_DEBUG
#define DBGL_EVMSG_2               DBGL_DEBUG
#elif EMDBG_VERBOSE_LEVEL == 1
#define DBGL_EVMSG_0               DBGL_DEBUG
#define DBGL_EVMSG_1               DBGL_DEBUG
#define DBGL_EVMSG_2               DBGL_NONE
#else
#define DBGL_EVMSG_0               DBGL_NONE
#define DBGL_EVMSG_1               DBGL_NONE
#define DBGL_EVMSG_2               DBGL_NONE
#endif

#ifndef __AMN_OS_LINUX__  // uC/OS-II
void* m_AMN_EvMan_MsgQ[AMN_EVMAN_MSGQ_DEPTH];
#endif

LIST_HEAD(event_list_msg);
LIST_HEAD(event_list_timer);
LIST_HEAD(event_list_FD);
LIST_HEAD(all_event_free_list);
int nr_free_events;
int nr_active_events[AMMO_EVMSG_TYP__TOTAL];

osal_mutex_t     evman_link_list_lock;
osal_mutex_t     evman_exec_lock;
osal_msg_queue_t evman_msgque_ID;

static void evmsg_destroy(struct ammo_evmsg *e);
static struct ammo_evmsg *__evmsg_new_message( evmsg_cback_ft f, void *arg, int prio, int wait );

//*******************************************************************************************************
//set 0 to turn off the debug. Typically set a value which is > 30
#define EVMSG_LATENCY_DBG_PERIOD 480   // 480 -> 512

#if EVMSG_LATENCY_DBG_PERIOD
struct ammo_basic_meter_t m_latency_meter;

#define evmsg_latency_meter( e ) \
		basic_meter_measure( &m_latency_meter, osal_jiffies() - e->time_embarked )

#define evmsg_latency_meter_init() \
		basic_meter_init( &m_latency_meter, EVMSG_LATENCY_DBG_PERIOD, "EVMSG Latency" )

#else
#define evmsg_latency_meter(e) 
#define evmsg_latency_meter_init()
#endif


//*******************************************************************************************************
static void evmsg_execute(struct ammo_evmsg *e)
{
	osal_dprint(EVMSG_2, "EVENT %p t=%d f=%p d=%p, FD=%d ival=%d", e, e->type, e->func, e->data, e->fd, e->ival);
	if (e->flags & AMMO_EVMSG_FLG__REMOVE) {
		// This event was killed already, skip its execution !!
		osal_dprint(EVMSG_0, "EVENT already removed, e=%p f=%p", e, e->func);
		return;
	}

	EVMSG_CALL_TRACE( (u32_t)(e->func) + 1 );  // before acquired the lock
	osal_mutex_lock( &evman_exec_lock );
	EVMSG_CALL_TRACE( (u32_t)(e->func) + 2 );  // after acquired the lock
	e->func( e, e->data );
	osal_mutex_unlock( &evman_exec_lock );
	EVMSG_CALL_TRACE( (u32_t)(e->func) + 3 );  // release the lock
}

static void __reschedule_timer( struct ammo_evmsg *e, struct timeval *tr )
{
	if( tr )
		e->fire = *tr;
	else if( e->flags & AMMO_EVMSG_FLG__ENABLED )
		time_add( &e->fire, e->ival );
	else
		time_future( &e->fire, e->ival );

	e->flags &= ~AMMO_EVMSG_FLG__REMOVE;
	e->flags |= AMMO_EVMSG_FLG__ENABLED;
}

static void __strip_events( struct list_head *list )
{
	struct list_head  *l, *d;
	struct ammo_evmsg *e;

	list_for_each( l, list ) {
		e = list_entry( l, struct ammo_evmsg, link );
		if (e->flags & AMMO_EVMSG_FLG__REMOVE) {
			d = l;
			l = l->next;
			list_del(d);
			evmsg_destroy(e);
		}
	}
}

//*******************************************************************************************************
static struct ammo_evmsg *evmsg_new( u8_t type, evmsg_cback_ft f, void *arg )
{
	struct ammo_evmsg *e;

	if (!list_empty(&all_event_free_list)) {
		nr_free_events--;
		e = list_first_entry( &all_event_free_list, struct ammo_evmsg, link );
		list_del( &e->link );
	}
	else {
		osal_dprint(ERR, "no more free event list in the system");
		return NULL;
	}

	INIT_LIST_HEAD( &e->link );
	e->time_embarked = osal_jiffies();
	e->type = type;
	e->func = f;
	e->data = arg;
	e->flags = 0;
	e->ival  = 0;
	nr_active_events[e->type]++;
	osal_dprint(EVMSG_1, "evmsg %p type=%d  nr_free_events=%d, NAct=%d,%d,%d ", e, type, nr_free_events, nr_active_events[0], nr_active_events[1], nr_active_events[2] );
	return e;
}

static void evmsg_destroy(struct ammo_evmsg *e)
{
	nr_active_events[e->type]--;
	INIT_LIST_HEAD( &e->link );
	list_add_tail( &e->link, &all_event_free_list );
	nr_free_events++;
	osal_dprint(EVMSG_1, "evmsg %p  nr_free_events=%d, NAct=%d,%d,%d ", e, nr_free_events, nr_active_events[0], nr_active_events[1], nr_active_events[2] );
}

struct ammo_evmsg *evmsg_new_FD( int fd, int write, unsigned int flags, evmsg_cback_ft f, void *arg )
{
	struct ammo_evmsg *e;

	osal_mutex_lock( &evman_link_list_lock );
	e = evmsg_new( AMMO_EVMSG_TYP__FD, f, arg );
	list_add_tail( &e->link, &event_list_FD );
	osal_mutex_unlock( &evman_link_list_lock );

	e->flags = flags | AMMO_EVMSG_FLG__ENABLED;
	e->fd = fd;
	e->write = write;
	return e;
}

struct ammo_evmsg *evmsg_new_timer( int msec, unsigned int flags, evmsg_cback_ft f, void *arg )
{
	struct ammo_evmsg *e;

	osal_mutex_lock( &evman_link_list_lock );
	e = evmsg_new( AMMO_EVMSG_TYP__TIMER, f, arg );
	list_add_tail( &e->link, &event_list_timer );
	osal_mutex_unlock( &evman_link_list_lock );

	e->flags = flags;
	e->ival = msec;
	time_now( &e->fire );
	__reschedule_timer( e, NULL );
	return e;
}

struct ammo_evmsg *evmsg_new_alarm( struct timeval *t, unsigned int flags, evmsg_cback_ft f, void *arg )
{
	struct ammo_evmsg *e;

	osal_mutex_lock( &evman_link_list_lock );
	e = evmsg_new( AMMO_EVMSG_TYP__TIMER, f, arg );
	list_add_tail( &e->link, &event_list_timer );
	osal_mutex_unlock( &evman_link_list_lock );

	e->flags = flags | AMMO_EVMSG_FLG__ONESHOT;
	time_now( &e->fire );
	__reschedule_timer( e, t );
	return e;
}

static struct ammo_evmsg *__evmsg_new_message( evmsg_cback_ft f, void *arg, int prio, int wait )
{
	struct ammo_evmsg *e;
	int res;

	osal_mutex_lock( &evman_link_list_lock );
	e = evmsg_new( AMMO_EVMSG_TYP__MESSAGE, f, arg );
	list_add_tail( &e->link, &event_list_msg );
	osal_mutex_unlock( &evman_link_list_lock );
	osal_dprint(EVMSG_1, "evmsg %p  nr_free_events=%d, NAct=%d,%d,%d ", e, nr_free_events, nr_active_events[0], nr_active_events[1], nr_active_events[2] );

	e->flags = 0;
	if (wait) {
		e->flags |= AMMO_EVMSG_FLG__WAIT;
	}
	e->ival  = wait;
	res = osal_mque_post( evman_msgque_ID, e, prio );
	if (osal_mque_check_err(res)) {
		// TODO: mostly rare, but will cause trouble at higher bitrate (6M) under 1080P ....
		osal_dprint(ERR, "MsgQue error %d, %s,  QCnt=%d  cb=%p arg=%p",
				res, osal_strerror(), osal_mque_query(evman_msgque_ID), f, arg);
		osal_mutex_lock( &evman_link_list_lock );
		list_del( &e->link );
		evmsg_destroy( e );
		osal_mutex_unlock( &evman_link_list_lock );
		return NULL;
	}
	return e;
}

struct ammo_evmsg *evmsg_new_message( evmsg_cback_ft f, void *arg, int wait /*in ms*/)
{
	return __evmsg_new_message( f, arg, AMMO_EVMSG_TYP__MESSAGE, wait );
}

struct ammo_evmsg *evmsg_do(evmsg_cback_ft f, void* arg, int wait, int schedule)
{
	if (schedule) return evmsg_new_message(f, arg, wait);
	else f(NULL, arg);
	return NULL;
}

void evmsg_remove( struct ammo_evmsg *e )
{
	e->flags |= AMMO_EVMSG_FLG__REMOVE;
	e->flags &= ~( AMMO_EVMSG_FLG__RUNNING | AMMO_EVMSG_FLG__ENABLED );
	
	osal_mutex_lock( &evman_link_list_lock );
	//Set data to NULL to avoid data abort in evmsg_execute
	//because this data always from mem_alloc 
	//if so ,it will be free after this function returned
	e->data = NULL;
	osal_mutex_unlock( &evman_link_list_lock );
}

void evmsg_show_list( char *name, struct list_head *list )
{
	int i=1;
	struct list_head  *l;//, *d;
	struct ammo_evmsg *e;

	osal_mutex_lock( &evman_link_list_lock );
	list_for_each( l, list ) {
		e = list_entry( l, struct ammo_evmsg, link );
		osal_dprint(VERBOSE, "LIST-%3s.%-2d  EVENT %p f=%p d=%p F=%x T=%9d  val=%d",
				name, i, e, e->func, e->data, e->flags, e->time_embarked,
				(e->type == AMMO_EVMSG_TYP__FD) ? e->fd : e->ival);
		i++;
	}
	osal_mutex_unlock( &evman_link_list_lock );
}

void evmsg_show( void )
{
	osal_dprint(VERBOSE, "Show EVMSG @%d", osal_jiffies());
	evmsg_show_list( "MSG", &event_list_msg );
	evmsg_show_list( "TMR", &event_list_timer );
	evmsg_show_list( "FD", &event_list_FD );
}

void evmsg_set_interval( struct ammo_evmsg *e, int msec )
{
	e->ival = msec;
	if( e->flags & AMMO_EVMSG_FLG__ENABLED ) __reschedule_timer( e, NULL );
}

void evmsg_set_enabled( struct ammo_evmsg *e, int enabled )
{
	e->flags &= ~AMMO_EVMSG_FLG__ENABLED;
	if( enabled ) e->flags |= AMMO_EVMSG_FLG__ENABLED;
}

int evmsg_get_enabled( struct ammo_evmsg *e )
{
	return e->flags & AMMO_EVMSG_FLG__ENABLED ? 1 : 0;
}

void evmsg_initialize()
{
	int i;
	struct ammo_evmsg *e = osal_zmalloc( AMN_EVMAN_TOTAL_EVMSGS * sizeof(struct ammo_evmsg) );
	osal_dprint(VERBOSE, "EVMSG malloc(%d) = %p", (int)(AMN_EVMAN_TOTAL_EVMSGS * sizeof(struct ammo_evmsg)), e);
	for (i=0; i < AMN_EVMAN_TOTAL_EVMSGS; i++) {
		osal_dprint(EVMSG_2, "EVMSG %p %d", e, (int)sizeof(struct ammo_evmsg));
		INIT_LIST_HEAD( &e->link );
		list_add_tail( &e->link, &all_event_free_list );
		e++;
	}
	nr_free_events = AMN_EVMAN_TOTAL_EVMSGS;
	osal_mque_new(&evman_msgque_ID);
	osal_mutex_init( &evman_link_list_lock );
	osal_mutex_init( &evman_exec_lock );

	evmsg_latency_meter_init();
}

osal_thread_t thread_select_loop(void *arg)
{
	struct list_head *l;
	struct ammo_evmsg *eTM;
	struct timeval TVal;
	int diff, nexttime = 0, nTM;
	struct ammo_evmsg *eFD;
	fd_set rfds, wfds;
	int highfd, ret;

	osal_ftrace();
	while(1) {
		nTM = 0;
		nexttime = 2000; // we want this loop to be revisited at least N sec a time
		// find the nearest timer event
		osal_mutex_lock( &evman_link_list_lock );
		list_for_each( l, &event_list_timer ) {
			eTM = list_entry( l, struct ammo_evmsg, link );
			if( eTM->flags & AMMO_EVMSG_FLG__ENABLED ) {
				nTM++;
				diff = -time_ago( &eTM->fire );
				if (diff < 5) diff = 0;
				if (diff < nexttime) nexttime = diff;
				eTM->flags |= AMMO_EVMSG_FLG__RUNNING;
			}
			else
				eTM->flags &= ~AMMO_EVMSG_FLG__RUNNING;
		}
		osal_mutex_unlock( &evman_link_list_lock );
		time_set( &TVal, nexttime );

		FD_ZERO( &rfds );
		FD_ZERO( &wfds );
		highfd = -1;
		/* This is all so ugly...  It should use poll() eventually. */
		osal_mutex_lock( &evman_link_list_lock );
		list_for_each( l, &event_list_FD ) {
			eFD = list_entry( l, struct ammo_evmsg, link );
			if( eFD->flags & AMMO_EVMSG_FLG__ENABLED ) {
				FD_SET( eFD->fd, eFD->write ? &wfds : &rfds );
				if( eFD->fd > highfd ) highfd = eFD->fd;
				eFD->flags |= AMMO_EVMSG_FLG__RUNNING;
			}
			else
				eFD->flags &= ~AMMO_EVMSG_FLG__RUNNING;
		}
		osal_mutex_unlock( &evman_link_list_lock );
		
		osal_timingmark( TIMING_GROUP__THREAD_SEL, "[eSELECT] %d ", 0 );
		//--------------------------------------------------
		ret = net_select( highfd + 1, &rfds, &wfds, NULL, &TVal );
		//--------------------------------------------------
		osal_timingmark( TIMING_GROUP__THREAD_SEL, "[bSELECT] %d ", 1 );

		osal_mutex_lock( &evman_link_list_lock );
		list_for_each( l, &event_list_timer ) {
			eTM = list_entry( l, struct ammo_evmsg, link );
			if (!(eTM->flags & AMMO_EVMSG_FLG__RUNNING)) continue;

			diff = -time_ago( &eTM->fire );
			if( diff < 5 ) {
				if (!(eTM->flags & AMMO_EVMSG_FLG__ONESHOT))
					__reschedule_timer( eTM, NULL );
				osal_mutex_unlock( &evman_link_list_lock );
				evmsg_execute( eTM );
				osal_mutex_lock( &evman_link_list_lock );
				if (eTM->flags & AMMO_EVMSG_FLG__ONESHOT)
					eTM->flags |= AMMO_EVMSG_FLG__REMOVE;
			}
		}
		__strip_events( &event_list_timer );
		osal_mutex_unlock( &evman_link_list_lock );
		
		osal_mutex_lock( &evman_link_list_lock );
		if (ret > 0) list_for_each( l, &event_list_FD ) {
			eFD = list_entry( l, struct ammo_evmsg, link );
			if (!(eFD->flags & AMMO_EVMSG_FLG__RUNNING)) continue;

			if (FD_ISSET( eFD->fd, eFD->write ? &wfds : &rfds )) {
				osal_mutex_unlock( &evman_link_list_lock );
				evmsg_execute( eFD );
				osal_mutex_lock( &evman_link_list_lock );
			}
		}
		__strip_events( &event_list_FD );
		osal_mutex_unlock( &evman_link_list_lock );
	}
}

osal_thread_t thread_message_handler(void *arg)
{
	osal_ftrace();
	while (1) {
		struct ammo_evmsg *e;
		int res;
		osal_timingmark( TIMING_GROUP__THREAD_MQUE, "[eMQEXE] %d ", 0 );
		res = osal_mque_pend( evman_msgque_ID, e );
		osal_timingmark( TIMING_GROUP__THREAD_MQUE, "[bMQEXE] %d ", 1 );
		if (osal_mque_check_err(res) || !e) {
			// some error
			osal_dprint(ERR, "MsgQue error %d, %s. msg=%p", res, osal_strerror(), e);
			continue;
		}
		osal_dprint(EVMSG_2, "MESSAGE %p t=%d f=%p d=%p res=%d", e, e->type, e->func, e->data, res);
		// system("cat /dev/mqueue/amnss_mque");

		if (e->flags & AMMO_EVMSG_FLG__WAIT) {
			// it should raise our attention if wait long here, in particular at streaming path !!!
			osal_dprint(VERBOSE, "MsgQue wait %d for f=%p d=%p", e->ival, e->func, e->data);
			osal_msleep(e->ival);
		}

		//------------------
		evmsg_latency_meter( e );
		evmsg_execute( e );
		//------------------

		osal_mutex_lock( &evman_link_list_lock );
#if 0
		list_del( &e->link );
		evmsg_destroy( e );
#else
		e->flags |= AMMO_EVMSG_FLG__REMOVE;
		__strip_events( &event_list_msg );
#endif
		osal_mutex_unlock( &evman_link_list_lock );
	}
}

//*******************************************************************************************************
