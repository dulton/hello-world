//==============================================================================
//
//  File        : version.h
//  Description : Header File for the Firmware Core Version Control
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _VERSION_H_
#define _VERSION_H_

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

/* Version maintained by human beings:
 * Increase Major number when new chip supported;
 * Increase Minor number when new features added;
 * Increase Patch number when bug-fix committed.
 */
#define VERSION_MAJOR   (2)
#define VERSION_MINOR   (11)
#define VERSION_PATCH   (14)


//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

extern void DVR_Version(MMP_USHORT *major, MMP_USHORT *minor, MMP_USHORT *patch);
extern void DVR_DumpConfig(void);
extern void DVR_DumpSystemCfg(void);
extern void DVR_DumpUartCfg(void);
extern void DVR_DumpFlowCtrlCfg(void);
extern void DVR_DumpMMUCfg(void);
extern void DVR_DumpSensorCfg(void);
extern void DVR_DumpISPCfg(void);
extern void DVR_DumpDisplayCfg(void);
extern void DVR_DumpVidPlayCfg(void);
extern void DVR_DumpVidRecCfg(void);
extern void DVR_DumpStorageCfg(void);
extern void DVR_DumpUSBCfg(void);
extern void DVR_DumpDSCCfg(void);
extern void DVR_DumpAudioCfg(void);

#endif //_VERSION_H_
