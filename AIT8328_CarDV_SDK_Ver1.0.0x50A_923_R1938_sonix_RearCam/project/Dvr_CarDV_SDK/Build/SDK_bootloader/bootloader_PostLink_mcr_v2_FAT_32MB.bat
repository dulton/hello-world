set PRJ_NAME=bootloader
set AXF_DIR=.\%PRJ_NAME%_Data\MCR_V2_FAT_32MB
set AXF_NAME=%PRJ_NAME%.axf
set RAM_DIR=%AXF_DIR%
set PKG_DIR=..\..\tools\DownloadTool_mcrv2_32MB
set PKG_NAME=BOOTFAT

fromELF.exe -c -bin %AXF_DIR%\%AXF_NAME% -output %RAM_DIR%\ALL_DRAM
..\FirmwarePackager .\bootloader_LDS_mcr_v2_FAT.txt %RAM_DIR%\ %PKG_DIR%\%PKG_NAME% /q
