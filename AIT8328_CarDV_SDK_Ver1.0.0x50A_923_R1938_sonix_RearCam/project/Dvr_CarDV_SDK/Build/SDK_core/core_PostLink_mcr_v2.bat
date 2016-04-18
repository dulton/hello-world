set PRJ_NAME=core
set AXF_DIR=.\%PRJ_NAME%_Data\MCR_V2
set AXF_NAME=%PRJ_NAME%.axf
set RAM_DIR=%AXF_DIR%
set PKG_DIR=..\..\tools\DownloadTool_mcrv2
set PKG_NAME=ALL

fromELF.exe -c -bin %AXF_DIR%\%AXF_NAME% -output %RAM_DIR%\RESET
..\FirmwarePackager .\core_LDS_mcr_v2.txt %RAM_DIR%\ %PKG_DIR%\%PKG_NAME% /q
