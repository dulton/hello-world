//==============================================================================
//
//  File        : GPS_NMEA0183_ANA.h
//  Description : Firmware Sensor Control File
//  Author      : Andy Liu
//  Revision    : 1.0
//
//==============================================================================

#ifndef	_GPS_NMEA0183_ANA_H_
#define	_GPS_NMEA0183_ANA_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

//#include "mmpf_sensor.h"
//#include "mmpf_vif.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================
MMP_ERR GPS_SetCommand_ByModule(struct _3RD_PARTY_GPS *pthis, void *pCommAry);
MMP_ERR GPS_Initiate_ANA_ByModule(struct _3RD_PARTY_GPS *pthis);
MMP_ERR GPS_SetHotStart_ByModule(struct _3RD_PARTY_GPS *pthis);
MMP_ERR GPS_SetWarmStart_1_ByModule(struct _3RD_PARTY_GPS *pthis);
MMP_ERR GPS_SetWarmStart_2_ByModule(struct _3RD_PARTY_GPS *pthis);
MMP_ERR GPS_SetColdStart_ByModule(struct _3RD_PARTY_GPS *pthis);
MMP_ERR GPS_SetBR57600_ByModule(struct _3RD_PARTY_GPS *pthis);
int GPS_Module_Init(void);

struct _3RD_PARTY_GPS  GPS_Module_Obj = 
{
    GPS_SetCommand_ByModule,          //00
    GPS_Initiate_ANA_ByModule,        //01
    GPS_SetHotStart_ByModule,         //02
    GPS_SetWarmStart_1_ByModule,      //03
    GPS_SetWarmStart_2_ByModule,      //04
    GPS_SetColdStart_ByModule,        //05
    GPS_SetBR57600_ByModule           //06
};

#endif // _GPS_NMEA0183_ANA_H_
