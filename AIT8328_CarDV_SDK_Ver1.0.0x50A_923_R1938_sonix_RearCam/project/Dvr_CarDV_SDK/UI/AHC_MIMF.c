//==============================================================================
/**
 @file AHC_MIMF.c
 @brief Middleware of Index Mapping Functions
 @author Jerry Li
 @version 1.0
*/
/*===========================================================================
 * Include files
 *===========================================================================*/
#include "AHC_FS.h"
#include "AHC_UF.h"
#include "AHC_DCFDT.h"
#include "AHC_MACRO.h"
#include "AHC_Utility.h"

UINT32 m_uiBrowserMask = 0x1f;
UINT32 m_uiBrowserFileTypeMask = 0x3;
UINT32 m_uiMIMFCurIndex = 0;
//do not set mask in browser mode
void AHC_MIMF_SetBrowserMask(UINT32 uiBrowserMask)
{
    m_uiBrowserMask = uiBrowserMask;
}

void AHC_MIMF_GetBrowserMask(UINT32 *puiBrowserMask)
{
    *puiBrowserMask = m_uiBrowserMask;
}

void AHC_MIMF_SetBrowserTypeMask(UINT32 uiBrowserMask)
{
    m_uiBrowserFileTypeMask = uiBrowserMask;
}

void AHC_MIMF_GetBrowserTypeMask(UINT32 *puiBrowserMask)
{
    *puiBrowserMask = m_uiBrowserFileTypeMask;
}

//customized function
//------------------------------------------------------------------------------
//  Function    : AHC_MIMF_GetTotalFileCount
//  Description : get total filecount in spec. DB which UI want to browse
//  param[out]  total file count
//  retval AHC_TRUE Success.
//------------------------------------------------------------------------------
AHC_BOOL AHC_MIMF_GetTotalFileCountByDb(UINT32 *puiImgIndex)
{
    UINT32 uiFileCount = 0; 
    UINT32 uiCurTotalFileCount = 0;
    UINT32 i;
    UINT32 ret;
    UINT32 uiCurDB;
    
    uiCurDB = AHC_DCFDT_GetDB();
    if(m_uiBrowserMask == AHC_MIMF_BROWSER_MASK_REAR_CAM){
        printc("Not support only rear cam browser!!\r\n");
    }

    for( i = DCF_DB_TYPE_1ST_DB; i < DCF_DB_COUNT; i++)
    {
        if(m_uiBrowserMask & (0x01 << i))
        {
            AHC_DCFDT_SelectDB(i);
            ret = AHC_DCFDT_GetTotalFileCount(&uiFileCount);
            uiCurTotalFileCount += uiFileCount;
            if(ret == AHC_FALSE){
                printc("%s,%d  return False\r\n",__func__, __LINE__);
                return AHC_FALSE;
            }
            if(m_uiBrowserMask & AHC_MIMF_BROWSER_MASK_REAR_CAM)
            {
                ret = AHC_DCFDT_GetTotalRearFileCount(&uiFileCount);
                uiCurTotalFileCount += uiFileCount;
                if(ret == AHC_FALSE){
                    printc("%s,%d  return False\r\n",__func__, __LINE__);
                    return AHC_FALSE;
                }
            }
        }
    }
    
    *puiImgIndex = uiCurTotalFileCount;
    AHC_DCFDT_SelectDB(uiCurDB);
    return AHC_TRUE;
}


AHC_BOOL AHC_MIMF_GetTotalFileCountByType(UINT32 *puiImgIndex)
{
    UINT32 uiFileCount = 0; 
    UINT32 uiCurTotalFileCount = 0;
    UINT32 i,j;
    UINT32 ret;
    UINT32 uiCurDB;
    AHC_BOOL bCurRearCam;
	UINT8 ubFileType;
	
    uiCurDB = AHC_DCFDT_GetDB();
    if(m_uiBrowserMask == AHC_MIMF_BROWSER_MASK_REAR_CAM){
        printc("Not support only rear cam browser!!\r\n");
    }

	i = DCF_DB_TYPE_1ST_DB;
    {
        {
            AHC_DCFDT_SelectDB(i);
            ret = AHC_DCFDT_GetTotalFileCount(&uiFileCount);
            //uiCurTotalFileCount += uiFileCount;
            if(ret == AHC_FALSE){
                printc("%s,%d  return False\r\n",__func__, __LINE__);
                return AHC_FALSE;
            }

			//
			for(j=0;j<uiFileCount;j++)
			{
				ret = AHC_DCFDT_GetFileTypebyIndex(j, &ubFileType);
				if((m_uiBrowserFileTypeMask & AHC_MIMF_BROWSER_MASK_MOV) &&
					( ( ubFileType == DCF_OBG_MOV ) ||
                      ( ubFileType == DCF_OBG_MP4 ) ||
                      ( ubFileType == DCF_OBG_AVI ) ||
                      ( ubFileType == DCF_OBG_3GP ) ||
                      ( ubFileType == DCF_OBG_WMV ) ))
				{
					uiCurTotalFileCount++;
				}
				else if((m_uiBrowserFileTypeMask & AHC_MIMF_BROWSER_MASK_JPG) &&
					( ubFileType == DCF_OBG_JPG ))
				{
					uiCurTotalFileCount++;
				}
			}

            if(m_uiBrowserMask & AHC_MIMF_BROWSER_MASK_REAR_CAM)
            {
                //ret = AHC_DCFDT_GetTotalRearFileCount(&uiFileCount);
				ret = AHC_DCFDT_GetTotalFileCount(&uiFileCount);//Files in folder "F" more than folder "R" always.
                if(ret == AHC_FALSE){
                    printc("%s,%d  return False\r\n",__func__, __LINE__);
                    return AHC_FALSE;
                }

				AHC_DCFDT_GetRearCamPathFlag(&bCurRearCam);
        		AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);

				for(j=0;j<uiFileCount;j++)
				{
					ret = AHC_DCFDT_GetFileTypebyIndex(j, &ubFileType);
					if((m_uiBrowserFileTypeMask & AHC_MIMF_BROWSER_MASK_MOV) &&
						( ( ubFileType == DCF_OBG_MOV ) ||
	                      ( ubFileType == DCF_OBG_MP4 ) ||
	                      ( ubFileType == DCF_OBG_AVI ) ||
	                      ( ubFileType == DCF_OBG_3GP ) ||
	                      ( ubFileType == DCF_OBG_WMV ) ))
					{
						uiCurTotalFileCount++;
					}
					else if((m_uiBrowserFileTypeMask & AHC_MIMF_BROWSER_MASK_JPG) &&
						( ubFileType == DCF_OBG_JPG ))
					{
						uiCurTotalFileCount++;
					}
				}

				AHC_DCFDT_SetRearCamPathFlag(bCurRearCam);
            }
        }
    }
    
    *puiImgIndex = uiCurTotalFileCount;
    AHC_DCFDT_SelectDB(uiCurDB);
    return AHC_TRUE;
}

AHC_BOOL AHC_MIMF_GetTotalFileCount(UINT32 *puiImgIndex)
{
#if (DCF_DB_COUNT > 2)
	return AHC_MIMF_GetTotalFileCountByDb(puiImgIndex);
#else
	return AHC_MIMF_GetTotalFileCountByType(puiImgIndex);
#endif
}

//------------------------------------------------------------------------------
//  Function    : AHC_MIMF_IndexReverseMapping
//  Description : Thumb Index inverse mapping to dcf Index
//  param[in]  Thumb Index which from UF level
//  param[out] DCF Index in DB
//  param[out] DB type
//  param[out] IsRearCam
//  retval AHC_TRUE Success.
//------------------------------------------------------------------------------
AHC_BOOL AHC_MIMF_IndexReverseMappingByDB(UINT32 uiImgIndex, UINT32 *puiDcfIndex, UINT32 *sDB, AHC_BOOL *bIsRearCam)
{
    UINT32   i, j;
    UINT32   uiCurFileCount      = 0;  //filecount of current DB
    UINT32   uiRearFileCount     = 0;  //rear cam filecount of current DB
    UINT32   uiTotalCount        = 0;  //TotalCount of All DB
    UINT32   uiPreviousFileCount = 0;  //
    AHC_BOOL bRearCamEnable      = AHC_FALSE;
    UINT32   uiCurDB;
    UINT32   ret;
    AHC_BOOL bfound = AHC_FALSE;
    
    uiCurDB = AHC_DCFDT_GetDB();
    if(!AHC_MIMF_GetTotalFileCount(&uiTotalCount))
    {
        return AHC_FALSE;
    }
    if(uiImgIndex >= uiTotalCount)
    {
        printc("ImgIndex %d >= TotalCount %d %s\r\n", uiImgIndex, uiTotalCount, __func__);
        return AHC_FALSE;
    }
    if(m_uiBrowserMask == AHC_MIMF_BROWSER_MASK_REAR_CAM){
        printc("Not support only rear cam browser!!\r\n");
    }
    //check rearcam enable
    if(m_uiBrowserMask & AHC_MIMF_BROWSER_MASK_REAR_CAM){
        bRearCamEnable = AHC_TRUE;
    }

    for( i = DCF_DB_TYPE_1ST_DB; i < DCF_DB_COUNT; i++)
    {
        if(bfound){
            break;
        }
        if(m_uiBrowserMask & (0x01 << i))
        {
            AHC_DCFDT_SelectDB(i);
            ret = AHC_DCFDT_GetTotalFileCount(&uiCurFileCount);
            if(ret == AHC_FALSE){
                printc("%s,%d  return False\r\n",__func__, __LINE__);
                return AHC_FALSE;
            }
            if(bRearCamEnable){
                ret = AHC_DCFDT_GetTotalRearFileCount(&uiRearFileCount);
                if(ret == AHC_FALSE){
                printc("%s,%d  return False\r\n",__func__, __LINE__);
                return AHC_FALSE;
                }
            }
            //if ImgIndex >= uiPreviousFileCount, Change to next DB
            uiPreviousFileCount += (uiCurFileCount + uiRearFileCount);
            if(uiImgIndex >= uiPreviousFileCount){
                continue;
            }else{   
                //find dcf Index in this DB 
                AHC_BOOL bRearFileExist;
                UINT32   uiFileCountPerNode = 1;
                INT32    iCounter = -1;
                INT32    iIndexInCurDB;
                //calculate Index in Current DB , iIndexInCurDB = index - (totalpreviousDBfilecount - curDBfilecount)  
                iIndexInCurDB = uiImgIndex - (uiPreviousFileCount - (uiCurFileCount + uiRearFileCount));
                //check if iIndexInCurDB out of range
                if(iIndexInCurDB < 0){
                    printc("uiImgIndex %d\r\n",uiImgIndex);
                    printc("uiPreviousFileCount %d\r\n",uiPreviousFileCount);
                    printc("uiCurFileCount %d\r\n",uiCurFileCount);
                    printc("uiRearFileCount %d\r\n",uiRearFileCount);
                    printc("Calculate Index error iIndexInCurDB %d %s\r\n", iIndexInCurDB, __func__);
                }
                //for each file node, find the Dcf Index 
                for( j = 0; j < (uiCurFileCount + uiRearFileCount); j++)
                {
                    //check filecount per node
                    if(bRearCamEnable)
                    {
                        AHC_DCFDT_IsRearCamFileExistbyIndex( j, &bRearFileExist);
                        if(bRearFileExist){
                            uiFileCountPerNode = 2;
                        }else{
                            uiFileCountPerNode = 1;
                        }
                    }
                    iCounter += uiFileCountPerNode;
                    if(uiFileCountPerNode == 1)
                    {
                        //this node have only front cam file
                        if(iIndexInCurDB == iCounter)
                        {
                            *puiDcfIndex = j;
                            *sDB         = i;
                            *bIsRearCam  = AHC_FALSE;
                            bfound = AHC_TRUE;
                            break;
                        }
                    }else if(uiFileCountPerNode == 2)
                    {
                        //this node have both front/rear cam file
                        if(iIndexInCurDB == iCounter)
                        {
                            *puiDcfIndex = j;
                            *sDB         = i;
                            *bIsRearCam  = AHC_TRUE;
                            bfound = AHC_TRUE;
                            break;
                        }else if(iIndexInCurDB == (iCounter - 1))
                        {
                            *puiDcfIndex = j;
                            *sDB         = i;
                            *bIsRearCam  = AHC_FALSE;
                            bfound = AHC_TRUE;
                            break;
                        }
                    }                    
                }
            }            
        }
    }
    if(bfound == AHC_FALSE){
        printc("Error!!! IndexReverseMapping not found! %d %s\r\n", uiImgIndex, __func__);
    }
    AHC_DCFDT_SelectDB(uiCurDB);
    return bfound;
}

AHC_BOOL AHC_MIMF_IndexReverseMappingByFileType(UINT32 uiImgIndex, UINT32 *puiDcfIndex, UINT32 *sDB, AHC_BOOL *bIsRearCam)
{
    UINT32   i, j;
    UINT32   uiCurFileCount      = 0;  //filecount of current DB
    UINT32   uiRearFileCount     = 0;  //rear cam filecount of current DB
    UINT32   uiTotalCount        = 0;  //TotalCount of All DB
    UINT32   uiPreviousFileCount = 0;  //
    AHC_BOOL bRearCamEnable      = AHC_FALSE;
    UINT32   uiCurDB;
    UINT32   ret;
    AHC_BOOL bfound = AHC_FALSE;
	AHC_BOOL bCurRearCam;
    UINT8	 ubFileType;
	UINT32	 uiCurTotalFileCount = 0;

	
    uiCurDB = AHC_DCFDT_GetDB();
    if(!AHC_MIMF_GetTotalFileCount(&uiTotalCount))
    {
        return AHC_FALSE;
    }
    if(uiImgIndex >= uiTotalCount)
    {
        printc("ImgIndex %d >= TotalCount %d %s\r\n", uiImgIndex, uiTotalCount, __func__);
        return AHC_FALSE;
    }
    if(m_uiBrowserMask == AHC_MIMF_BROWSER_MASK_REAR_CAM){
        printc("Not support only rear cam browser!!\r\n");
    }
	if(m_uiBrowserFileTypeMask == 0){
        printc("Not support, Must be set file type at first!!\r\n");
    }
	
    //check rearcam enable
    if(m_uiBrowserMask & AHC_MIMF_BROWSER_MASK_REAR_CAM){
        bRearCamEnable = AHC_TRUE;
    }

	i = DCF_DB_TYPE_1ST_DB;
    {
        AHC_DCFDT_SelectDB(i);
        ret = AHC_DCFDT_GetTotalFileCount(&uiCurFileCount);
        if(ret == AHC_FALSE){
            printc("%s,%d  return False\r\n",__func__, __LINE__);
            return AHC_FALSE;
        }

		for(j=0;j<uiCurFileCount;j++)
		{
			ret = AHC_DCFDT_GetFileTypebyIndex(j, &ubFileType);
			if((m_uiBrowserFileTypeMask & AHC_MIMF_BROWSER_MASK_MOV) &&
				( ( ubFileType == DCF_OBG_MOV ) ||
                  ( ubFileType == DCF_OBG_MP4 ) ||
                  ( ubFileType == DCF_OBG_AVI ) ||
                  ( ubFileType == DCF_OBG_3GP ) ||
                  ( ubFileType == DCF_OBG_WMV ) ))
			{
				uiCurTotalFileCount++;
			}
			else if((m_uiBrowserFileTypeMask & AHC_MIMF_BROWSER_MASK_JPG) &&
				( ubFileType == DCF_OBG_JPG ))
			{
				uiCurTotalFileCount++;
			}

			if((uiImgIndex+1) == uiCurTotalFileCount)
			{
				*puiDcfIndex = j;
				*sDB         = i;
				*bIsRearCam  = AHC_FALSE;
				bfound = AHC_TRUE;
				goto END_MAPPING;
			}
		}


        if(bRearCamEnable && bfound == AHC_FALSE)
		{
            //ret = AHC_DCFDT_GetTotalRearFileCount(&uiRearFileCount);
			ret = AHC_DCFDT_GetTotalFileCount(&uiRearFileCount);
            if(ret == AHC_FALSE){
            	printc("%s,%d  return False\r\n",__func__, __LINE__);
            	return AHC_FALSE;
            }
			AHC_DCFDT_GetRearCamPathFlag(&bCurRearCam);
    		AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
			
			for(j=0;j<uiRearFileCount;j++)
			{
				ret = AHC_DCFDT_GetFileTypebyIndex(j, &ubFileType);
				if((m_uiBrowserFileTypeMask & AHC_MIMF_BROWSER_MASK_MOV) &&
					( ( ubFileType == DCF_OBG_MOV ) ||
                      ( ubFileType == DCF_OBG_MP4 ) ||
                      ( ubFileType == DCF_OBG_AVI ) ||
                      ( ubFileType == DCF_OBG_3GP ) ||
                      ( ubFileType == DCF_OBG_WMV ) ))
				{
					uiCurTotalFileCount++;
				}
				else if((m_uiBrowserFileTypeMask & AHC_MIMF_BROWSER_MASK_JPG) &&
					( ubFileType == DCF_OBG_JPG ))
				{
					uiCurTotalFileCount++;
				}

				if((uiImgIndex+1) == uiCurTotalFileCount)//index[0~9], total=10
				{
					*puiDcfIndex = j;
					*sDB         = i;
					*bIsRearCam  = AHC_TRUE;
					bfound = AHC_TRUE;
					break;
				}
			}
			AHC_DCFDT_SetRearCamPathFlag(bCurRearCam);
        }
    }

END_MAPPING:	
    if(bfound == AHC_FALSE){
        printc("Error!!! IndexReverseMapping not found! %d %s\r\n", uiImgIndex, __func__);
    }
    AHC_DCFDT_SelectDB(uiCurDB);
    return bfound;
}

AHC_BOOL AHC_MIMF_IndexReverseMapping(UINT32 uiImgIndex, UINT32 *puiDcfIndex, UINT32 *sDB, AHC_BOOL *bIsRearCam)
{
#if (DCF_DB_COUNT > 2)
	return AHC_MIMF_IndexReverseMappingByDB(uiImgIndex, puiDcfIndex, sDB, bIsRearCam);
#else
	return AHC_MIMF_IndexReverseMappingByFileType(uiImgIndex, puiDcfIndex, sDB, bIsRearCam);
#endif
}
//------------------------------------------------------------------------------
//  Function    : AHC_MIMF_IndexMapping
//  Description : dcf Index mapping to Thumb Index
//  param[in] DCF Index in DB
//  param[in] DB type
//  param[in] IsRearCam
//  param[out]  Thumb Index which from UF level
//  retval AHC_TRUE Success.
//------------------------------------------------------------------------------
AHC_BOOL AHC_MIMF_IndexMapping(UINT32 puiDcfIndex, UINT32 sDB, AHC_BOOL bIsRearCam, UINT32 *uiImgIndex)
{
    UINT32   uiCurDB;
    UINT32   i;
    UINT32   uiCurFileCount      = 0;
    UINT32   uiRearFileCount     = 0;  //rear cam filecount of current DB
    UINT32   uiPreviousFileCount = 0;
    INT32    iCounter            = -1;
    AHC_BOOL bRearCamEnable;
    AHC_BOOL ret;
    
    uiCurDB = AHC_DCFDT_GetDB();
    if(sDB >= DCF_DB_COUNT){
        printc("Illegal parameter sDB %d\r\n", sDB);
        return AHC_FALSE;
    }
    
    if(m_uiBrowserMask == AHC_MIMF_BROWSER_MASK_REAR_CAM){
        printc("Not support only rear cam browser!!\r\n");
    }
    //check rearcam enable
    if(m_uiBrowserMask & AHC_MIMF_BROWSER_MASK_REAR_CAM){
        bRearCamEnable = AHC_TRUE;
    }
    //calculate total file count in previous DB
    for( i = DCF_DB_TYPE_1ST_DB; i < sDB; i++)
    {
        if(m_uiBrowserMask & (0x01 << i))
        {
            AHC_DCFDT_SelectDB(i);
            ret = AHC_DCFDT_GetTotalFileCount(&uiCurFileCount);
            if(ret == AHC_FALSE){
                printc("%s,%d  return False\r\n",__func__, __LINE__);
                return AHC_FALSE;
            }

            if(bRearCamEnable){
                ret = AHC_DCFDT_GetTotalRearFileCount(&uiRearFileCount);
                if(ret == AHC_FALSE){
                printc("%s,%d  return False\r\n",__func__, __LINE__);
                return AHC_FALSE;
                }
            }
            uiPreviousFileCount += (uiCurFileCount + uiRearFileCount);
        }
    }
    //calculate Index in current DB
    AHC_DCFDT_SelectDB(sDB);
    for( i = 0; i <= puiDcfIndex; i++)
    {
        AHC_BOOL bRearFileExist;
        UINT32   uiFileCountPerNode = 1;
        
        //check filecount per node
        if(bRearCamEnable)
        {
            AHC_DCFDT_IsRearCamFileExistbyIndex( i, &bRearFileExist);
            if(bRearFileExist){
                uiFileCountPerNode = 2;
            }else{
                uiFileCountPerNode = 1;
            }
        }
        
        if(i == puiDcfIndex){
            if(bIsRearCam){
                iCounter += uiFileCountPerNode;
                if(uiFileCountPerNode == 1){
                    printc("Warning!!! This node do not have rear cam file! Given front cam Index!, %s\r\n",__func__);
                }
            }else{
                iCounter += 1;
            }    
        }else{
            iCounter += uiFileCountPerNode;
        }
    }
    
    *uiImgIndex = uiPreviousFileCount + iCounter;
    AHC_DCFDT_SelectDB(uiCurDB);
    return AHC_TRUE;
}
AHC_BOOL AHC_MIMF_SetCurrentIndex(UINT32 uiImgIndex)
{
    UINT32 uiTotalCount; 
    if( !AHC_MIMF_GetTotalFileCount(&uiTotalCount))
    {
        return AHC_FALSE;
    }
    if(uiImgIndex >= uiTotalCount)
    {
        printc("ImgIndex %d >= TotalCount %d %s\r\n", uiImgIndex, uiTotalCount, __func__);
        return AHC_FALSE;
    }
    m_uiMIMFCurIndex = uiImgIndex;
        
    return AHC_TRUE; 
}

AHC_BOOL AHC_MIMF_GetCurrentIndex(UINT32 *puiImgIndex)
{
    UINT32 uiTotalCount; 
    if( !AHC_MIMF_GetTotalFileCount(&uiTotalCount))
    {
        return AHC_FALSE;
    }
    
    *puiImgIndex = m_uiMIMFCurIndex;
    if(*puiImgIndex >= uiTotalCount)
    {
        printc("*puiImgIndex %d >= TotalCount %d %s\r\n", *puiImgIndex, uiTotalCount, __func__);
        return AHC_FALSE;
    }

    return AHC_TRUE;
}

AHC_BOOL AHC_MIMF_GetFilePathNamebyIndex(UINT32 uiIndex, char* FilePathName) 
{
    UINT32   uiDcfIndex;
    UINT32   sDB;
    AHC_BOOL bIsRearCam;
    UINT32   uiCurDB;
    AHC_BOOL ret;
    AHC_BOOL bCurRearCam;
    
    uiCurDB = AHC_DCFDT_GetDB();
    AHC_MIMF_IndexReverseMapping(uiIndex, &uiDcfIndex, &sDB, &bIsRearCam);
    AHC_DCFDT_SelectDB(sDB);
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_GetRearCamPathFlag(&bCurRearCam);
        AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
    }
    
	ret = AHC_DCFDT_GetFilePathNamebyIndex(uiDcfIndex, FilePathName); 
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_SetRearCamPathFlag(bCurRearCam);
    }
    AHC_DCFDT_SelectDB(uiCurDB);
    return ret;
}

AHC_BOOL AHC_MIMF_GetFileSizebyIndex(UINT32 uiIndex, UINT32* uiFileSize) 
{
    UINT32   uiDcfIndex;
    UINT32   sDB;
    AHC_BOOL bIsRearCam;
    UINT32   uiCurDB;
    AHC_BOOL ret;
    AHC_BOOL bCurRearCam;
    
    uiCurDB = AHC_DCFDT_GetDB();
    AHC_MIMF_IndexReverseMapping(uiIndex, &uiDcfIndex, &sDB, &bIsRearCam);
    AHC_DCFDT_SelectDB(sDB);
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_GetRearCamPathFlag(&bCurRearCam);
        AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
    }
    
	ret = AHC_DCFDT_GetFileSizebyIndex(uiDcfIndex, uiFileSize); 
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_SetRearCamPathFlag(bCurRearCam);
    }
    AHC_DCFDT_SelectDB(uiCurDB);
    return ret;
}

AHC_BOOL AHC_MIMF_GetFileTypebyIndex(UINT32 uiIndex, UINT8 *ubFileType) 
{
    UINT32   uiDcfIndex;
    UINT32   sDB;
    AHC_BOOL bIsRearCam;
    UINT32   uiCurDB;
    AHC_BOOL ret;
    AHC_BOOL bCurRearCam;
    
    uiCurDB = AHC_DCFDT_GetDB();
	
    AHC_MIMF_IndexReverseMapping(uiIndex, &uiDcfIndex, &sDB, &bIsRearCam);
	
    AHC_DCFDT_SelectDB(sDB);
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_GetRearCamPathFlag(&bCurRearCam);
        AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
    }
    
	ret = AHC_DCFDT_GetFileTypebyIndex(uiDcfIndex, ubFileType); 
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_SetRearCamPathFlag(bCurRearCam);
    }
	
    AHC_DCFDT_SelectDB(uiCurDB);
    return ret;
}

AHC_BOOL AHC_MIMF_IsReadOnlybyIndex(UINT32 uiIndex, AHC_BOOL *bReadOnly) 
{
    UINT32   uiDcfIndex;
    UINT32   sDB;
    AHC_BOOL bIsRearCam;
    UINT32   uiCurDB;
    AHC_BOOL ret;
    AHC_BOOL bCurRearCam;
    
    uiCurDB = AHC_DCFDT_GetDB();
    AHC_MIMF_IndexReverseMapping(uiIndex, &uiDcfIndex, &sDB, &bIsRearCam);
    AHC_DCFDT_SelectDB(sDB);
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_GetRearCamPathFlag(&bCurRearCam);
        AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
    }
    
	ret = AHC_DCFDT_IsReadOnlybyIndex(uiDcfIndex, bReadOnly); 
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_SetRearCamPathFlag(bCurRearCam);
    }
    AHC_DCFDT_SelectDB(uiCurDB);
    return ret;
}

AHC_BOOL AHC_MIMF_GetFileTimebyIndex(UINT32 uiIndex, AHC_RTC_TIME *pRtcTime)
{
    UINT32   uiDcfIndex;
    UINT32   sDB;
    AHC_BOOL bIsRearCam;
    UINT32   uiCurDB;
    AHC_BOOL ret;
    AHC_BOOL bCurRearCam;
    
    uiCurDB = AHC_DCFDT_GetDB();
    AHC_MIMF_IndexReverseMapping(uiIndex, &uiDcfIndex, &sDB, &bIsRearCam);
    AHC_DCFDT_SelectDB(sDB);
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_GetRearCamPathFlag(&bCurRearCam);
        AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
    }
    ret = AHC_DCFDT_GetFileTimebyIndex(uiDcfIndex, pRtcTime);

    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_SetRearCamPathFlag(bCurRearCam);
    }
    AHC_DCFDT_SelectDB(uiCurDB);

    return ret;
}

AHC_BOOL AHC_MIMF_IsCharLockbyIndex(UINT32 uiIndex, AHC_BOOL *bCharLock)
{
    printc("Does not support!!\r\n");
    return AHC_FALSE;
}

AHC_BOOL AHC_MIMF_GetFileNamebyIndex(UINT32 uiIndex, char* FileName) 
{
    UINT32   uiDcfIndex;
    UINT32   sDB;
    AHC_BOOL bIsRearCam;
    UINT32   uiCurDB;
    AHC_BOOL ret;
    AHC_BOOL bCurRearCam;
    
    uiCurDB = AHC_DCFDT_GetDB();
    AHC_MIMF_IndexReverseMapping(uiIndex, &uiDcfIndex, &sDB, &bIsRearCam);
    AHC_DCFDT_SelectDB(sDB);
    
    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_GetRearCamPathFlag(&bCurRearCam);
        AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
    }
    ret = AHC_DCFDT_GetFileNamebyIndex(uiDcfIndex, FileName);

    if(bIsRearCam == AHC_TRUE){
        AHC_DCFDT_SetRearCamPathFlag(bCurRearCam);
    }
    AHC_DCFDT_SelectDB(uiCurDB);

    return ret;
}
void AHC_MIMF_RegisterCusCallback(void)
{
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_GETTOTALFILECOUNT, (void*)AHC_MIMF_GetTotalFileCount);
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_SETCURRENTINDEX, (void*)AHC_MIMF_SetCurrentIndex);
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_GETCURRENTINDEX, (void*)AHC_MIMF_GetCurrentIndex);
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_GETFILEPATHNAMEBYINDEX, (void*)AHC_MIMF_GetFilePathNamebyIndex);
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_GETFILESIZEBYINDEX, (void*)AHC_MIMF_GetFileSizebyIndex);
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_GETFILETYPEBYINDEX, (void*)AHC_MIMF_GetFileTypebyIndex);
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_ISREADONLYBYINDEX, (void*)AHC_MIMF_IsReadOnlybyIndex);
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_GETFILETIMEBYINDEX, (void*)AHC_MIMF_GetFileTimebyIndex);
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_ISCHARLOCKBYINDEX, (void*)AHC_MIMF_IsCharLockbyIndex);
    AHC_UF_MappingFuncRegisterCallBack(AHC_UFCB_GETFILENAMEBYINDEX, (void*)AHC_MIMF_GetFileNamebyIndex);
}
//end of AHC_MIMF