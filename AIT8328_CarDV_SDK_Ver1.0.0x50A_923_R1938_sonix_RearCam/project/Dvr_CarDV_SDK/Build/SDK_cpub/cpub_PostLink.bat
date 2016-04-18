set PRJ_NAME=CPUB
set AXF_DIR=.\%PRJ_NAME%_Data\MCR_V2
set AXF_NAME=%PRJ_NAME%.axf
set RAM_DIR=%AXF_DIR%
set PKG_DIR=c:\aitfw\82x
set PKG_NAME=MCR_CPUB
set SIZE_INFO=size_info.txt
set H_DIR=c:\aitfw\82x
set TOOL_DIR=..\..\..\..\core\tool
set SRC_DIR=..\..\..\..\core\src\dvr\sys\multicore

del %AXF_DIR%\CPUB_FW_ITCM
del %AXF_DIR%\CPUB_FW_DRAM
del %AXF_DIR%\CPUB_FW_RORW
del %AXF_DIR%\CPUB_FW_ZI

:: Save the firmware size information
fromELF.exe -z %AXF_DIR%\%AXF_NAME% -output %RAM_DIR%/%SIZE_INFO%

:: Create plain binary
fromELF.exe -c -bin %AXF_DIR%\%AXF_NAME% -output %RAM_DIR%

ren %AXF_DIR%\ITCM_EXE CPUB_FW_ITCM
ren %AXF_DIR%\RESET    CPUB_FW_RORW

:: Create the ZI binary according to the size information of ZI area
:: PaddingZI usage: [size info file] [output file]
%TOOL_DIR%\PaddingZI %RAM_DIR%/%SIZE_INFO% %AXF_DIR%\CPUB_FW_ZI
:: Append the ZI area to complete the FW binary
copy /b %AXF_DIR%\CPUB_FW_RORW + %AXF_DIR%\CPUB_FW_ZI %AXF_DIR%\CPUB_FW_DRAM

:: Convert the FW binaries into array and save as header file within source folder
:: BinToDwordArray usage: [input file] [output file] [array name]
%TOOL_DIR%\BinToDwordArray.exe %AXF_DIR%\CPUB_FW_ITCM %SRC_DIR%\CPUB_FW_ITCM.h CPUB_FW_ITCM
%TOOL_DIR%\BinToDwordArray.exe %AXF_DIR%\CPUB_FW_DRAM %SRC_DIR%\CPUB_FW_DRAM.h CPUB_FW_DRAM

rem pause
