/*
**********************************************************************
*                          Micrium, Inc.
*                      949 Crestview Circle
*                     Weston,  FL 33327-1848
*
*                            uC/FS
*
*             (c) Copyright 2001 - 2006, Micrium, Inc.
*                      All rights reserved.
*
***********************************************************************

----------------------------------------------------------------------
File        : FS_Dev.h
Purpose     : Define structures for Device Drivers
---------------------------END-OF-HEADER------------------------------
*/

#ifndef _FS_DEV_H_
#define _FS_DEV_H_

#include "FS_ConfDefaults.h"
#include "FS_Int.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif


/*********************************************************************
*
*             Global data types
*
**********************************************************************
*/

struct FS_DEVICE_TYPE {
  const char *      name;
  int               MaxUnits;
  int               (*pfGetStatus)    (U8 Unit);
  int               (*pfRead)         (U8 Unit, U32 SectorNo, void *pBuffer);
  int               (*pfWrite)        (U8 Unit, U32 SectorNo, const void *pBuffer);
  int               (*pfIoCtl)        (U8 Unit, I32 Cmd, I32 Aux, void *pBuffer);
  int               (*pfReadBurst)    (U8 Unit, U32 SectorNo, U32 NumSectors,       void *pBuffer);
  int               (*pfWriteBurst)   (U8 Unit, U32 SectorNo, U32 NumSectors, const void *pBuffer);
  int               (*pfInitDevice)   (U8 Unit);
  int               (*pfInitMedium)   (U8 Unit);
  int               (*pfWriteMultiple)(U8 Unit, U32 SectorNo, U32 NumSectors, const void *pBuffer);
  //int             (*pfEraseSector)  (U8 Unit, U32 SectorNo, U32 NumSectors);
};

#if defined(__cplusplus)
 }               /* Make sure we have C-declarations in C++ programs */
#endif

#endif  /* _FS_DEV_H_ */

/*************************** End of file ****************************/
	 	 			 		    	 				 	   		   	 		  	 	      	   		 	 	  	  		  	 		 	   		 		      		       	   	 			  	     	   		 		     			    	 		 	    		 					 	 	  	  	 	 		 		 	    		 			  		 	  	 		  			 		 			  		 
