/*
 * Copyright 2012, Ampak Technology Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Ampak Technology Inc.;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Ampak Technology Inc..
 */
//#define	WPS_AP_SUPPORT

#ifndef _INCLUDED_GB9662IF_H_
#define _INCLUDED_GB9662IF_H_


#include "lwip/err.h"

struct pbuf;
struct netif;

/******************************************************
 *             Constants
 ******************************************************/

/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Function declarations
 ******************************************************/

err_t ethernetif_init(struct netif *netif);
err_t ethernet_input(struct pbuf *p, struct netif *netif);
err_t low_level_output(struct netif *netif, struct pbuf *p);
void host_network_process_ethernet_data(struct pbuf *p, struct netif *interface);
int low_level_init(struct netif *netif);

/******************************************************
 *             Global variables
 ******************************************************/


#endif /* define _INCLUDED_GB9662IF_H_ */

