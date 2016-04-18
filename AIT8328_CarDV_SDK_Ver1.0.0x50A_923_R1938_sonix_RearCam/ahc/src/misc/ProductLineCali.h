typedef struct 
{
    unsigned short   usVerMajor;
    unsigned short   usVerMinor;
    unsigned short   usLumaBaseX;
    unsigned short   usLumaBaseY;
    unsigned short   usLumaOffsetX;
    unsigned short   usLumaOffsetY;
    unsigned short   usSHADE_CenterX;
    unsigned short   usSHADE_CenterY;
    unsigned short   usLumaRateX;
    unsigned short   usLumaRateY;
	unsigned short	 usLensShading_R[256];
	unsigned short	 usLensShading_G[256];
	unsigned short	 usLensShading_B[256];
    unsigned short   usCS_BASE_X;
    unsigned short   usCS_BASE_Y;
    unsigned short   usCS_OFFSET_X;
    unsigned short   usCS_OFFSET_Y;
    unsigned short   usCS_CENTER_X;
    unsigned short   usCS_CENTER_Y;
	unsigned short   usCSScale;
	unsigned short	 usXSamplePos[41];
	unsigned short	 usYSamplePos[31];
	unsigned short	 usCS_Table_R[1344];  // 42*32 = 1344
	unsigned short	 usCS_Table_G[1344];
	unsigned short	 usCS_Table_B[1344];
} LSCS_CALI_RESULT;

typedef struct 
{
	unsigned long         CheckSum;
	unsigned short         Version;
	unsigned short         TurnkeyFwMajorVersion;
	unsigned short         TurnkeyFwMediumVersion;
	unsigned short         TurnkeyFwMinorVersion;
	LSCS_CALI_RESULT   LsCsResult;  //LS and CS calibration value;
    unsigned short         INFPos       ; //INFPOS calibration value;
    unsigned short         LensMiniVolt ; //INFPOS calibration value;
    short          InfErrZmPos[11] ; //INFPOS calibration value;
    short          InfErrValue[11] ; //INFPOS calibration value;
    unsigned short    	   AeCali[6]	   ; //AE calibration value
    long          AWBCP1FwdMatrix[9] ; //AWB calibration value;
    long          AWBCP1InvMatrix[9] ; //AWB calibration value;
    short         StillAfAddiShift;    // additional shift after stillaf
} CALI_RESULT;


void PLCali_AF_Inf_Pos(void);
void PLCali_LSCS(void);
void PLCali_LSCS_Raw(void);
void PLCali_CheckCaliBin(char* szParam);
void PLCali_ReadCaliBinToFlash(int IsWriteToFlash);
void PLCali_ResetCaliBinToFlash(void);
void PLCali_AF_GetStillafPos(int stillcnt);
void PLCali_LSCS_SetFromSD(void);	
void CaptureRaw(unsigned char ubType, char  *m_bLsCsRawFileName);	//capture raw after off shading
void PLCali_AWB_GetRgbAve(int mode);
void PLCheck_AE(void);
void PLCali_AWB_CalcData(void);
void PLCali_AE(void);
void CaptureRaw_LsCsOn(unsigned char ubType, char  *m_bLsCsRawFileBaseName,int IsTo16Bits);