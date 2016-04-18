64MB Version
// Add modify MCP history in there

32MB Version
// Difference with MCR_V2 target (64MB version)
1. Clone new target by MCR_V2
2. change "WIFI_PORT=0"
3. Add define "MCR_V2_32MB" in ARM C / Thumb C
4. Scatter file change to "core_LDS_mcr_v2_32MB.txt"
5. postlink batch file change to "core_PostLink_mcr_v2_32MB.bat"
6. Cancel to build "core\src\drv\net"