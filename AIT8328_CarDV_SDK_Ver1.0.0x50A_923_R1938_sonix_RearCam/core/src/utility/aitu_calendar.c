//==============================================================================
//
//  File        : aitu_calendar.c
//  Description : Generic Calendar Utility
//  Author      : CZ
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "aitu_calendar.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

static const MMP_SHORT m_usDaysOfMonAcc[12] = {
    31,  59,  90,  120,  151,  181,  212,  243,  273,  304, 334, 365
};
static const MMP_SHORT m_usDaysOfLeapMonAcc[12] = {
    31,  60,  91,  121,  152,  182,  213,  244,  274,  305, 335, 366
};
/* The base is 31 days per month */
static const MMP_BYTE m_usMonOfstAcc[12] = {
    0,   0,   3,    3,    4,    4,    5,    5,    5,    6,   6,   7
};

//==============================================================================
//
//                              LOCAL FUNCTIONS
//
//==============================================================================
//------------------------------------------------------------------------------
//  Function    : _IsLeapYear
//------------------------------------------------------------------------------
/** 
    @brief  Check the year is leap year or not.
    @param[in] year : Indicate the year value.
    @return MMP_TRUE: leap year, MMP_FALSE: not leap year.
*/
static MMP_BOOL _IsLeapYear(MMP_LONG year)
{
    return ((year & 0x3) == 0) && ((year % 100) || ((year % 400) == 0));
}

//------------------------------------------------------------------------------
//  Function    : _DateToDaysAbs
//------------------------------------------------------------------------------
/** 
    @brief  Use year/month/day information to calculate the number of days.
    @param[in] year : Indicate the year.
    @param[in] mon : Indicate the month.
    @param[in] day : Indicate the day.
    @return number of days.
*/
static MMP_LONG _DateToDaysAbs(MMP_LONG year, MMP_LONG mon, MMP_LONG day)
{
    MMP_LONG  leap;
    MMP_LONG  d;

    leap = (_IsLeapYear(year) && (mon > 2)) ? (1) : (0);

    year -= 1;

	d = (year*365)+(year/4)-(year/100)+(year/400) +
        (((mon - 1) * 31) - m_usMonOfstAcc[mon - 1] + leap) + day;

    return d;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_Calendar_DateToDays
//------------------------------------------------------------------------------
/** 
    @brief  Use date time information and base date information to calculate
            offset days.
    @param[in] pTime : Indicate the date information to be calculated.
    @param[in] pBaseTime : Indicate base date time.
    @return number of days.
*/
MMP_LONG AUTL_Calendar_DateToDays(AUTL_DATETIME *pTime, AUTL_DATETIME *pBaseTime)
{
    MMP_LONG offset;

    offset = _DateToDaysAbs(pTime->usYear, pTime->usMonth, pTime->usDay) -
             _DateToDaysAbs(pBaseTime->usYear, pBaseTime->usMonth, pBaseTime->usDay);

    return offset;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_Calendar_DaysToDate
//------------------------------------------------------------------------------
/** 
    @brief  Set days and calculate the date information.
    @param[in] days : Indicate number of days.
    @param[out] pTime : return result date information.
    @param[in] pBaseTime : Indicate date information of base time.
    @return MMP_TRUE: success. MMP_FALSE: fail.
*/
MMP_BOOL AUTL_Calendar_DaysToDate(MMP_LONG days, AUTL_DATETIME *pTime,
                                  AUTL_DATETIME *pBaseTime)
{
    MMP_LONG 		y = (days * 400 / 146097) + pBaseTime->usYear;
    MMP_LONG 		t, m, d;
    const MMP_SHORT *p_day_of_mon;
    AUTL_DATETIME 	tempTime0, tempTime1;
    MMP_LONG 		usDays0, usDays1;

    tempTime0.usMonth 	= 1;
    tempTime0.usDay 	= 1;
    tempTime1.usMonth 	= 12;
    tempTime1.usDay 	= 31;

    // Get the year
    while(1) {
        tempTime0.usYear = y;
        tempTime1.usYear = y;

    	usDays0 = AUTL_Calendar_DateToDays(&tempTime0, pBaseTime);
    	usDays1 = AUTL_Calendar_DateToDays(&tempTime1, pBaseTime);

        if (days < usDays0)
            y -= 1;
        else if (days > usDays1)
            y += 1;
        else
            break;
    }

    t = days - usDays0;

    if (_IsLeapYear(y))
        p_day_of_mon = m_usDaysOfLeapMonAcc;
    else
        p_day_of_mon = m_usDaysOfMonAcc;

    for (m = t/31; m < 12; m++)
    {
        if (t < p_day_of_mon[m])
            break;
    }

    d = t - ((m > 0) ? (p_day_of_mon[m-1]) : 0);

    pTime->usYear  = y;
    pTime->usMonth = m + 1;
    pTime->usDay   = d + 1;

    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_Calendar_DateToSeconds
//------------------------------------------------------------------------------
/** 
    @brief  Set date information to calculate the offset seconds.
    @param[in] pTime : Indicate the date information to be calculated.
    @param[in] pBaseTime : Indicate base date time.
    @return number of seconds.
*/
MMP_ULONG AUTL_Calendar_DateToSeconds(AUTL_DATETIME *pTime,
                                      AUTL_DATETIME *pBaseTime)
{
    MMP_ULONG days;
    MMP_ULONG sec;
    MMP_ULONG hour;

    hour 	= (pTime->b_12FormatEn && pTime->ubAmOrPm) ?
                pTime->usHour + 12 : pTime->usHour;

    days	= AUTL_Calendar_DateToDays(pTime, pBaseTime);
    
    sec     = (days * SEC_PER_DAY) + (hour * SEC_PER_HOUR) +
              (pTime->usMinute * SEC_PER_MIN) + pTime->usSecond;

    return sec;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_Calendar_SecondsToDate
//------------------------------------------------------------------------------
/** 
    @brief  Set number of seconds information to calculate date information.
    @param[in] ulSeconds : Indicate the number of seconds.
    @param[out] pTime : return the date information.
    @param[in] pBaseTime : Indicate base date time.
    @return number of seconds.
*/
MMP_BOOL AUTL_Calendar_SecondsToDate(MMP_ULONG ulSeconds, AUTL_DATETIME *pTime,
                                     AUTL_DATETIME *pBaseTime)
{
    MMP_ULONG days;
    MMP_ULONG sec, hour, min;

    sec         = ulSeconds;
    days        = sec / SEC_PER_DAY;

    AUTL_Calendar_DaysToDate(days, pTime, pBaseTime);

    sec        -= days * SEC_PER_DAY;   // sec  of the day
    min         = sec  / 60;            // min  of the day
    hour        = min  / 60;            // hour of the day
    sec        -= min  * 60;            // sec  of the hour
    min        -= hour * 60;            // min  of the hour

    pTime->usHour       = hour;
    pTime->usMinute     = min;
    pTime->usSecond     = sec;
    pTime->ubAmOrPm     = (hour >= 12) ? 1 : 0;
    pTime->b_12FormatEn = MMP_FALSE;

    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_Calendar_ValidateTime
//------------------------------------------------------------------------------
/** 
    @brief   The function validate the value of time
    @param[in] pTime : Indicate date time information to be calculated.
    @param[in] pBaseTime : Indicate base date time.
    @return It reports if the time is validate.
*/
MMP_BOOL AUTL_Calendar_ValidateTime(AUTL_DATETIME *pTime,
                                    AUTL_DATETIME *pBaseTime)
{
    MMP_LONG    y, m, d;
    const MMP_SHORT *p_day_of_mon;

    y = pTime->usYear;
    m = pTime->usMonth;
    d = pTime->usDay;

    // Check date correction
    if (y < pBaseTime->usYear) {
        return MMP_FALSE;
    }
    else if (y == pBaseTime->usYear) {
        if (m < pBaseTime->usMonth)
            return MMP_FALSE;
        else if ((m == pBaseTime->usMonth) && (d < pBaseTime->usDay))
            return MMP_FALSE;
    }
    
    if ((m > 12) || (m < 1) || (d < 0)) {
        return MMP_FALSE;
	}

    if (_IsLeapYear(y))
        p_day_of_mon = m_usDaysOfLeapMonAcc;
    else
        p_day_of_mon = m_usDaysOfMonAcc;

    if (m == 1) {
        if (d > 31)
            return MMP_FALSE;
    }
    else if ((d + p_day_of_mon[m - 2]) > p_day_of_mon[m - 1]) {
        return MMP_FALSE;
    }

    // Check time correction
    if (pTime->b_12FormatEn && (pTime->usHour > 12))
        return MMP_FALSE;
    else if (!pTime->b_12FormatEn && (pTime->usHour > 23))
        return MMP_FALSE;
    else if ((pTime->usMinute > 59) || (pTime->usSecond > 59))
        return MMP_FALSE;

    return MMP_TRUE;
}

