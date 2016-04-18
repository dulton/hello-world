//==============================================================================
//
//  File        : AHC_RTC.c
//  Description : AHC RTC control function
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "customer_config.h"
#include "mmp_lib.h"
#include "mmp_err.h"
#include "lib_retina.h"
#include "ait_utility.h"
#include "mmps_rtc.h"
#include "AHC_General.h"
#include "AHC_Config_SDK.h"
#include "AHC_DateTime.h"
#include "AHC_Utility.h"
#include "AHC_USBHost.h"
#include "Gps_ctl.h"

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

#if (RTC_FORCE_DEFAULT_TIME == 1) // It's used when external RTC without driver, but we still want to test video record with DateTime file name.
static AHC_RTC_TIME m_sRTCTime = {2011, 1, 1, 0, 0, 0, 0, 0, 0};
#else
static AHC_RTC_TIME m_sRTCTime = {0};
#endif

static AHC_BOOL     m_bUpdateRelativeTime = AHC_FALSE;

DSTDuration DST_GMT_P00[7] ={
                                {{13, 3, 31, 1}, {13, 10, 27, 1}},
                                {{14, 3, 30, 1}, {14, 10, 26, 1}},
                                {{15, 3, 29, 1}, {15, 10, 25, 1}},
                                {{16, 3, 27, 1}, {16, 10, 30, 1}},
                                {{17, 3, 26, 1}, {17, 10, 29, 1}},
                                {{18, 3, 25, 1}, {18, 10, 28, 1}},
                                {{19, 3, 31, 1}, {19, 10, 27, 1}}
                            };

DSTDuration DST_GMT_P10[7] ={
                                {{13, 10, 6, 2}, {14, 4, 6, 2}},
                                {{14, 10, 5, 2}, {15, 4, 5, 2}},
                                {{15, 10, 4, 2}, {16, 4, 3, 2}},
                                {{16, 10, 2, 2}, {17, 4, 2, 2}},
                                {{17, 10, 1, 2}, {18, 4, 1, 2}},
                                {{18, 10, 7, 2}, {19, 4, 7, 2}},
                                {{19, 10, 6, 2}, {20, 4, 1, 2}}
                            };

DSTDuration DST_GMT_P02[7] ={
                                {{13, 3, 29, 2}, {13, 10, 27, 1}},
                                {{14, 3, 28, 2}, {14, 10, 26, 1}},
                                {{15, 3, 27, 2}, {15, 10, 25, 1}},
                                {{16, 3, 25, 2}, {16, 10, 30, 1}},
                                {{17, 3, 24, 2}, {17, 10, 29, 1}},
                                {{18, 3, 23, 2}, {18, 10, 28, 1}},
                                {{19, 3, 29, 2}, {19, 10, 27, 1}}
                            };

/*===========================================================================
 * Main body
 *===========================================================================*/

#if 0
void _____RTC_Function_________(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_RTC_UpdateTime
//  Description : 
//------------------------------------------------------------------------------
AHC_ERR AHC_RTC_UpdateTime(void)
{
    MMP_ERR status;
    AUTL_DATETIME scur_rtc_time;

    status = MMPS_RTC_GetTime(&scur_rtc_time);

    if (status != MMP_ERR_NONE) {
        return AHC_FALSE;
    }

    MEMCPY((void *)&m_sRTCTime, (void *)&scur_rtc_time, sizeof(AHC_RTC_TIME));
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_RTC_DSTCheck_GMT
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_RTC_DSTCheck_GMT(const UINT32 uiYear, const UINT32 uiMon, const UINT32 uiDay, const UINT32 uiHour)
{
    UINT8   ubSize = 0;
    UINT8   i = 0;
    UINT32  uiCurTime = 0;
    UINT32  uiStartTime = 0;
    UINT32  uiEndTime = 0;
    pDSTDuration DSTZone = NULL;

    if (uiYear >= 2000)
    {
        uiCurTime = ((uiYear - 2000 & 0xFF) << 24   )   | \
                    ((uiMon         & 0xFF) << 16   )   | \
                    ((uiDay         & 0xFF) << 8    )   | \
                    ((uiHour        & 0xFF) << 0    );
    }
    else
    {
        uiCurTime = ((uiYear        & 0xFF) << 24   )   | \
                    ((uiMon         & 0xFF) << 16   )   | \
                    ((uiDay         & 0xFF) << 8    )   | \
                    ((uiHour        & 0xFF) << 0    );
    }

    switch(DST_TIMEZONE)
    {
        case 0:
            DSTZone = DST_GMT_P00;
        break;
        case 8:
            DSTZone = DST_GMT_P02;
        break;
        case 40:
            DSTZone = DST_GMT_P10;
        break;
    }

    if (DSTZone == NULL) //No need DST
        return 0;

    ubSize = sizeof(DST_GMT_P00)/sizeof(DSTDuration);

    for (i = 0; i < ubSize; i++)
    {
        uiStartTime =   ((DSTZone[i].sStart.uiYear  & 0xFF) << 24   )   | \
                        ((DSTZone[i].sStart.uiMon   & 0xFF) << 16   )   | \
                        ((DSTZone[i].sStart.uiDay   & 0xFF) << 8    )   | \
                        ((DSTZone[i].sStart.uiHour  & 0xFF) << 0    );

        uiEndTime =     ((DSTZone[i].sEnd.uiYear    & 0xFF) << 24   )   | \
                        ((DSTZone[i].sEnd.uiMon     & 0xFF) << 16   )   | \
                        ((DSTZone[i].sEnd.uiDay     & 0xFF) << 8    )   | \
                        ((DSTZone[i].sEnd.uiHour    & 0xFF) << 0    );

        if (uiCurTime < uiEndTime && uiCurTime >= uiStartTime)
        {
            return 1;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
//  Function    : AHC_RTC_DSTCheck_UserSet
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_RTC_DSTCheck_UserSet(const UINT32 uiYear, const UINT32 uiMon, const UINT32 uiDay, const UINT32 uiHour)
{
    UINT8   ubSize = 0;
    UINT8   i = 0;
    UINT32  uiCurTime = 0;
    UINT32  uiStartTime = 0;
    UINT32  uiEndTime = 0;
    pDSTDuration DSTZone = NULL;

    if (uiYear >= 2000)
    {
        uiCurTime = ((uiYear - 2000 & 0xFF) << 24   )   | \
                    ((uiMon         & 0xFF) << 16   )   | \
                    ((uiDay         & 0xFF) << 8    )   | \
                    ((uiHour        & 0xFF) << 0    );
    }
    else
    {
        uiCurTime = ((uiYear        & 0xFF) << 24   )   | \
                    ((uiMon         & 0xFF) << 16   )   | \
                    ((uiDay         & 0xFF) << 8    )   | \
                    ((uiHour        & 0xFF) << 0    );
    }

    switch(DST_TIMEZONE)
    {
        case 0:
            DSTZone = DST_GMT_P00;
        break;
        case 8:
            DSTZone = DST_GMT_P02;
        break;
        case 40:
            DSTZone = DST_GMT_P10;
        break;
    }

    if (DSTZone == NULL)//No need DST
        return 0;

    ubSize = sizeof(DST_GMT_P00)/sizeof(DSTDuration);

    for (i = 0; i < ubSize; i++)
    {
        uiStartTime =   ((DSTZone[i].sStart.uiYear  & 0xFF) << 24   )   | \
                        ((DSTZone[i].sStart.uiMon   & 0xFF) << 16   )   | \
                        ((DSTZone[i].sStart.uiDay   & 0xFF) << 8    )   | \
                        ((DSTZone[i].sStart.uiHour  & 0xFF) << 0    );

        uiEndTime =     ((DSTZone[i].sEnd.uiYear    & 0xFF) << 24   )   | \
                        ((DSTZone[i].sEnd.uiMon     & 0xFF) << 16   )   | \
                        ((DSTZone[i].sEnd.uiDay     & 0xFF) << 8    )   | \
                        ((DSTZone[i].sEnd.uiHour    & 0xFF) << 0    );

        if (uiCurTime <= uiEndTime && uiCurTime > uiStartTime)
        {
            return 1;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
//  Function    : AHC_RTC_DSTTimeOffset
//  Description : 
//------------------------------------------------------------------------------
void AHC_RTC_DSTTimeOffset(DSTFullTime *sBackup, DSTFullTime *sRes, signed char sbOffset)
{
    INT lBackupYear,lBackupDay,lBackupHour,lBackupMin;
    signed char byHour = sbOffset/4;
    signed char byMinute = (sbOffset%4)*15;

    if (sBackup->iYear == 0 ||  sBackup->iMon == 0 ||  sBackup->iDay == 0) {
        return;
    }
    
    if (byHour < -12)
        byHour = -12;
    else if (byHour > 13)
        byHour = 13;

    lBackupYear = sBackup->iYear;
    lBackupDay  = sBackup->iDay;
    lBackupHour = sBackup->iHour;
    lBackupMin  = sBackup->iMin;

    if (byMinute != 0)
    {
        sBackup->iMin = ((sBackup->iMin + byMinute)%60);

        if ((lBackupMin + byMinute) > 60)
        {
            if (sBackup->iHour != 23)
            {
                sBackup->iHour +=1;
            }
            else
            {
                sBackup->iHour = 0;
                
                if (sBackup->iMon != 2)
                {
                    switch(sBackup->iDay)
                    {
                        case 30:
                            switch(sBackup->iMon)
                            {
                                case 4:
                                case 6:
                                case 9:
                                case 11:
                                    sBackup->iDay = 1;
                                    sBackup->iMon += 1;
                                break;
                                default:
                                    sBackup->iDay += 1;
                                break;
                            }
                        break;
                        case 31:
                            switch(sBackup->iMon)
                            {
                                case 12:
                                    sBackup->iDay = 1;
                                    sBackup->iMon = 1;
                                    sBackup->iYear += 1;
                                break;
                                default:
                                    sBackup->iDay = 1;
                                    sBackup->iMon += 1;
                                break;
                            }
                        break;
                        default:
                            sBackup->iDay += 1;
                        break;
                    }
                }
                else
                {
                    switch(sBackup->iDay)
                    {
                        case 29:
                            sBackup->iDay = 1;
                            sBackup->iMon += 1;
                        break;
                        case 28:
                            if((sBackup->iYear%400==0))
                            {
                                sBackup->iDay += 1;
                            }
                            else if((sBackup->iYear%4 == 0)&&(sBackup->iYear%100 != 0))
                            {
                                sBackup->iDay += 1;
                            }
                            else
                            {
                                sBackup->iDay = 1;
                                sBackup->iMon += 1;
                            }
                        break;
                        default:
                            sBackup->iDay += 1;
                        break;
                    }
                }
            }
        }
        else if ((lBackupMin + byMinute)<0)
        {
            if (sBackup->iHour > 0)
            {
                sBackup->iHour -=1;
            }
            else if (sBackup->iHour == 0)
            {
                sBackup->iHour = 23;

                if (sBackup->iDay != 1)
                {
                    sBackup->iDay -= 1;
                }
                else if (sBackup->iDay == 1)
                {
                    if (sBackup->iMon == 3)
                    {
                        if ((sBackup->iYear%400==0))
                        {
                            sBackup->iMon = 2;
                            sBackup->iDay = 29;
                        }
                        else if ((sBackup->iYear%4 == 0) && (sBackup->iYear%100 != 0))
                        {
                            sBackup->iMon = 2;
                            sBackup->iDay = 29;
                        }
                        else
                        {
                            sBackup->iMon = 2;
                            sBackup->iDay = 28;
                        }
                    }
                    else
                    {
                        switch(sBackup->iMon)
                        {
                            case 1:
                                sBackup->iYear -= 1;
                                sBackup->iMon = 12;
                                sBackup->iDay = 31;
                            break;
                            case 3:
                            case 5:
                            case 7:
                            case 8:
                            case 10:
                            case 12:
                                sBackup->iMon -= 1;
                                sBackup->iDay = 30;
                            break;
                            case 2:
                            case 4:
                            case 6:
                            case 9:
                            case 11:
                                sBackup->iMon -= 1;
                                sBackup->iDay = 31;
                            break;

                        }
                    }
                }
            }
        }
    }

    if ((sBackup->iHour + byHour) >= 24)
    {
        sBackup->iHour = ((sBackup->iHour + byHour)%24);

        if (sBackup->iMon != 2)
        {
            switch(sBackup->iDay)
            {
                case 30:
                    switch(sBackup->iMon)
                    {
                        case 4:
                        case 6:
                        case 9:
                        case 11:
                            sBackup->iDay = 1;
                            sBackup->iMon += 1;
                        break;
                        default:
                            sBackup->iDay += 1;
                        break;
                    }
                break;
                case 31:
                    switch(sBackup->iMon)
                    {
                        case 12:
                            sBackup->iDay = 1;
                            sBackup->iMon = 1;
                            sBackup->iYear += 1;
                        break;
                        default:
                            sBackup->iDay = 1;
                            sBackup->iMon += 1;
                        break;
                    }
                break;
                default:
                    sBackup->iDay += 1;
                break;
            }
        }
        else
        {
            switch(sBackup->iDay)
            {
                case 29:
                    sBackup->iDay = 1;
                    sBackup->iMon += 1;
                break;
                case 28:
                    if ((sBackup->iYear%400==0))
                    {
                        sBackup->iDay += 1;
                    }
                    else if ((sBackup->iYear%4 == 0)&&(sBackup->iYear%100 != 0))
                    {
                        sBackup->iDay += 1;
                    }
                    else
                    {
                        sBackup->iDay = 1;
                        sBackup->iMon += 1;
                    }
                break;
                default:
                    sBackup->iDay += 1;
                break;
            }
        }
    }
    else if ((sBackup->iHour + byHour) < 0)
    {
        sBackup->iHour = ((sBackup->iHour + 24 + byHour));

        if (sBackup->iDay != 1)
        {
            sBackup->iDay -= 1;
        }
        else if(sBackup->iDay == 1)
        {
            if (sBackup->iMon == 3)
            {
                if ((sBackup->iYear%400==0))
                {
                    sBackup->iMon = 2;
                    sBackup->iDay = 29;
                }
                else if ((sBackup->iYear%4 == 0)&&(sBackup->iYear%100 != 0))
                {
                    sBackup->iMon = 2;
                    sBackup->iDay = 29;
                }
                else
                {
                    sBackup->iMon = 2;
                    sBackup->iDay = 28;
                }
            }
            else
            {
                switch(sBackup->iMon)
                {
                    case 1:
                        sBackup->iYear -= 1;
                        sBackup->iMon = 12;
                        sBackup->iDay = 31;
                    break;
                    case 3:
                    case 5:
                    case 7:
                    case 10:
                    case 12:
                        sBackup->iMon -= 1;
                        sBackup->iDay = 30;
                    break;
                    case 2:
                    case 4:
                    case 6:
                    case 8:
                    case 9:
                    case 11:
                        sBackup->iMon -= 1;
                        sBackup->iDay = 31;
                    break;
                }
            }
        }
    }
    else
    {
        sBackup->iHour += byHour;
    }

    sRes->iYear = sBackup->iYear;
    sRes->iMon  = sBackup->iMon ;
    sRes->iDay  = sBackup->iDay;
    sRes->iHour = sBackup->iHour;
    sRes->iMin  = sBackup->iMin ;
    sRes->iSec  = sBackup->iSec ;

    sBackup->iYear  = lBackupYear;
    sBackup->iDay   = lBackupDay;
    sBackup->iHour  = lBackupHour;
    sBackup->iMin   = lBackupMin;
}

//------------------------------------------------------------------------------
//  Function    : AHC_RTC_DayLightSavingEnable
//  Description : 
//------------------------------------------------------------------------------
void AHC_RTC_DayLightSavingEnable(AHC_BOOL bEnable)
{
    if (bEnable) {
        m_bUpdateRelativeTime = AHC_TRUE;
    }
    else {
        m_bUpdateRelativeTime = AHC_FALSE;
    }
}

//------------------------------------------------------------------------------
//  Function    : AHC_RTC_GetTimeEx
//  Description : 
//------------------------------------------------------------------------------
void AHC_RTC_GetTimeEx(AHC_RTC_TIME *psAHC_RTC_Time, AHC_BOOL bUpdateRelativeTime)
{
    AHC_RTC_TIME sAHC_RTC_TempTime = {0};
    
    if (!bUpdateRelativeTime)
    {
        *psAHC_RTC_Time = m_sRTCTime;
    }

    if (AHC_RTC_DSTCheck_GMT(m_sRTCTime.uwYear, m_sRTCTime.uwMonth, m_sRTCTime.uwDay, m_sRTCTime.uwHour))
    {
        DSTFullTime sTime = {0};
        DSTFullTime sOffsetTime = {0};

        sTime.iYear = m_sRTCTime.uwYear;
        sTime.iMon  = m_sRTCTime.uwMonth;
        sTime.iDay  = m_sRTCTime.uwDay;
        sTime.iHour = m_sRTCTime.uwHour;
        sTime.iMin  = m_sRTCTime.uwMinute;
        sTime.iSec  = m_sRTCTime.uwSecond;

        AHC_RTC_DSTTimeOffset(&sTime, &sOffsetTime, 4); //Add one hour

        sAHC_RTC_TempTime.uwYear   = sOffsetTime.iYear;
        sAHC_RTC_TempTime.uwMonth  = sOffsetTime.iMon;
        sAHC_RTC_TempTime.uwDay    = sOffsetTime.iDay;
        sAHC_RTC_TempTime.uwHour   = sOffsetTime.iHour;
        sAHC_RTC_TempTime.uwMinute = sOffsetTime.iMin;
        sAHC_RTC_TempTime.uwSecond = sOffsetTime.iSec;
    }
    else
    {
        sAHC_RTC_TempTime = m_sRTCTime;
    }

    *psAHC_RTC_Time = sAHC_RTC_TempTime;
}

//------------------------------------------------------------------------------
//  Function    : AHC_RTC_GetTime
//  Description : 
//------------------------------------------------------------------------------
void AHC_RTC_GetTime(AHC_RTC_TIME *psAHC_RTC_Time)
{
    AHC_RTC_TIME sAHC_RTC_TempTime = {0};
    
    if (!m_bUpdateRelativeTime) {
        *psAHC_RTC_Time = m_sRTCTime;
    }
    else
    {
        printc(" for daytime saveing please use AHC_RTC_GetTimeEx() !!\n");
    }
    
    #ifdef CUSTOMER_RESET_YEAR
	if (m_sRTCTime.uwYear > RTC_MAX_YEAR){
		m_sRTCTime.uwYear = RTC_DEFAULT_YEAR;
		printc("#@#@RESET m_sRTCTime.uwYear = %d \r\n",m_sRTCTime.uwYear);
		AHC_SetClock(m_sRTCTime.uwYear, m_sRTCTime.uwMonth, m_sRTCTime.uwDay, m_sRTCTime.uwDayInWeek, m_sRTCTime.uwHour, m_sRTCTime.uwMinute, m_sRTCTime.uwSecond, m_sRTCTime.ubAmOrPm, m_sRTCTime.b_12FormatEn);
	}
    #endif

    // TBD:: Need to remove here.
    if (AHC_RTC_DSTCheck_GMT(m_sRTCTime.uwYear, m_sRTCTime.uwMonth, m_sRTCTime.uwDay, m_sRTCTime.uwHour))
    {
        DSTFullTime sTime = {0};
        DSTFullTime sOffsetTime = {0};

        sTime.iYear = m_sRTCTime.uwYear;
        sTime.iMon  = m_sRTCTime.uwMonth;
        sTime.iDay  = m_sRTCTime.uwDay;
        sTime.iHour = m_sRTCTime.uwHour;
        sTime.iMin  = m_sRTCTime.uwMinute;
        sTime.iSec  = m_sRTCTime.uwSecond;

        AHC_RTC_DSTTimeOffset(&sTime, &sOffsetTime, 4); //Add one hour

        sAHC_RTC_TempTime.uwYear   = sOffsetTime.iYear;
        sAHC_RTC_TempTime.uwMonth  = sOffsetTime.iMon;
        sAHC_RTC_TempTime.uwDay    = sOffsetTime.iDay;
        sAHC_RTC_TempTime.uwHour   = sOffsetTime.iHour;
        sAHC_RTC_TempTime.uwMinute = sOffsetTime.iMin;
        sAHC_RTC_TempTime.uwSecond = sOffsetTime.iSec;
    }
    else
    {
        sAHC_RTC_TempTime = m_sRTCTime;
    }

    *psAHC_RTC_Time = sAHC_RTC_TempTime;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetClock
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set RTC

 Set RTC value.
 Parameters:
 @param[in] uiYear Set year
 @param[in] uiMonth Set Month
 @param[in] uiDay Set Day
 @param[in] uiHour Set Hour
 @param[in] uiMinute Set Minute
 @param[in] uiSecond Set Second
 @retval TRUE or FALSE. // TRUE: Success, FALSE: Fail
*/
AHC_BOOL AHC_SetClock(UINT16 uiYear, UINT16 uiMonth, UINT16 uiDay, UINT16 uiDayInWeek, UINT16 uiHour, UINT16 uiMinute, UINT16 uiSecond,  UINT8 ubAmOrPm, UINT8 b_12FormatEn)
{
    MMP_ERR status;
    AUTL_DATETIME sTime;

    sTime.usYear         = uiYear;
    sTime.usMonth        = uiMonth;
    sTime.usDay          = uiDay;
    sTime.usDayInWeek    = uiDayInWeek;
    sTime.usHour         = uiHour;
    sTime.usMinute       = uiMinute;
    sTime.usSecond       = uiSecond;
    sTime.ubAmOrPm       = ubAmOrPm;
    sTime.b_12FormatEn   = b_12FormatEn;

    status = MMPS_RTC_SetTime(&sTime);

    if (status != MMP_ERR_NONE) {
        printc("%s,%d error!\r\n", __func__, __LINE__);
        return AHC_FALSE;
    }

	#if (UVC_HOST_VIDEO_ENABLE)
    AHC_HostUVCResetTime();
    #endif
    
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetClock
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get RTC

 Get RTC value.
 Parameters:
 @param[out] puwYear Get year
 @param[out] puwMonth Get Month
 @param[out] puwDay Get Day
 @param[out] puwHour Get Hour
 @param[out] puwMinute Get Minute
 @param[out] puwSecond Get Second
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_GetClock(UINT16* puwYear, UINT16* puwMonth, UINT16* puwDay, UINT16* puwDayInWeek, UINT16* puwHour, UINT16* puwMinute, UINT16* puwSecond, UINT8* ubAmOrPm, UINT8* b_12FormatEn)
{
    *puwYear 		= m_sRTCTime.uwYear;
    *puwMonth 		= m_sRTCTime.uwMonth;
    *puwDay 		= m_sRTCTime.uwDay;
    *puwDayInWeek 	= m_sRTCTime.uwDayInWeek;
    *puwHour 		= m_sRTCTime.uwHour;
    *puwMinute 		= m_sRTCTime.uwMinute;
    *puwSecond 		= m_sRTCTime.uwSecond;
    *ubAmOrPm		= m_sRTCTime.ubAmOrPm;
    *b_12FormatEn 	= m_sRTCTime.b_12FormatEn;

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_RtcSetWakeUpEn
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set wake up counter of RTC unit

 Set wake up counter of RTC unit.
 Parameters:
 @param[in] bEnable Enable/Disable RTC wake up.
 @param[in] uiYear Set year
 @param[in] uiMonth Set Month
 @param[in] uiDay Set Day
 @param[in] uiHour Set Hour
 @param[in] uiMinute Set Minute
 @param[in] uiSecond Set Second
 @param[in] phHandleFunc Handler function
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_RtcSetWakeUpEn(AHC_BOOL bEnable, UINT16 uiYear, UINT16 uiMonth, UINT16 uiDay, UINT16 uiHour, UINT16 uiMinute, UINT16 uiSecond, void *phHandleFunc)
{
    AUTL_DATETIME sTime;

    sTime.usYear         = uiYear;
    sTime.usMonth        = uiMonth;
    sTime.usDay          = uiDay;
    sTime.usDayInWeek    = 0;
    sTime.usHour         = uiHour;
    sTime.usMinute       = uiMinute;
    sTime.usSecond       = uiSecond;
    sTime.ubAmOrPm       = 0;
    sTime.b_12FormatEn   = 1;

    if (MMPS_RTC_SetWakeUpEn(bEnable, &sTime, (RtcAlarmCallBackFunc *)phHandleFunc) != MMP_ERR_NONE) {
        printc("%s,%d error!\r\n", __func__, __LINE__);
        return AHC_FALSE;
    }

    return AHC_TRUE;
}