################################################################################
# This is a makefile for EVB platform to build bootloader                      #
################################################################################

# Include the makefile for standard definitions
include ../std_define.mk

# Variables for bootloader
CHIP_MODEL = MCR_V2
FW_NAME = MBOOT_FW
SCATTER = bootloader_LDS_mcr_v2.txt
POST_LINK = bootloader_PostLink_mcr_v2.bat
AXF_NAME = bootloader.axf

# Specified flags for bootloader
ASFLAGS := $(STD_ASFLAGS)
CCFLAGS := $(STD_CCFLAGS) -D$(FW_NAME) -DCHIP=$(CHIP_MODEL)
CCFLAGS += -DSA_PL -DBUILD_CE -DBUILD_FW -DVIDEO_DECODER=HW_MP4V
LDFLAGS := $(STD_LDFLAGS) -scatter $(SCATTER)

# The basename of target sources for bootloader
BSP = ait_bsp bsp bsp_a cstartup_ram lib_retina mmu
MODULES = lcd_common mmpf_display mmpf_display_irq mmpf_ibc_irq mmpf_vif \
          mmpf_dma mmpf_dma_irq mmpf_timer mmpf_timer_irq mmpf_i2cm \
          mmpf_pio mmpf_pio_irq mmpf_uart mmpf_uart_irq \
          mmpf_nand mmpf_nand_irq mmpf_sd mmpf_sd_irq mmpf_sf \
          mmpf_dram mmpf_mci mmpf_pll
DVR_API = mmps_display mmps_system mmpd_display mmpd_dma mmpd_system mmph_hif
DVR_SYS = mmpf_storage_api mem_misc r_misc sd_misc sd1_misc sf_misc sm_misc \
          mmpf_high_task mmpf_high_task_api mmpf_system os_wrap sys_task \
          all_fw mmpf_hif
PRJ_SRC = bootloader
PRJ_CFG = ait_config ait_utility config_fw
VENDOR = lcd_sony240x320
#VENDOR = lcd_GiantplusGPM773_240x320
#VENDOR = lcd_OTA5182A

TARGET := $(BSP) $(MODULES) $(DVR_API) $(DVR_SYS) $(PRJ_SRC) $(PRJ_CFG) $(VENDOR)

# The libraries for bootloader
LIBS = uCOSII.a ufs.a

# Object combine & target binaries
OBJS := $(addsuffix .o, $(TARGET))
LD_OBJS := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(TARGET)))
LD_LIBS := $(foreach lib, $(LIBS), $(filter %/$(lib), $(LIBRARIES)))
LD_AXF := $(OBJ_DIR)/$(AXF_NAME)

all: $(LD_AXF)
	@if exist $(LD_AXF) echo Build completed

$(LD_AXF): $(OBJS) $(LIBS)
	$(LD) $(LD_OBJS) $(LD_LIBS) $(LDFLAGS) -o $(LD_AXF)
	$(POST_LINK)

%.o: %.c
	if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(TCC) $(CCFLAGS) $(INCLUDES) $(DEP_FLAG) $< >> $(OBJ_DIR)/$*.d
	$(TCC) $(CCFLAGS) $(INCLUDES) $< -o $(OBJ_DIR)/$@

%.o: %.c32
	if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CCFLAGS) $(INCLUDES) $(DEP_FLAG) $< >> $(OBJ_DIR)/$*.d
	$(CC) $(CCFLAGS) $(INCLUDES) $< -o $(OBJ_DIR)/$@

%.o: %.s
	if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(AS) $(ASFLAGS) $(INCLUDES) $< -o $(OBJ_DIR)/$@

rebuild: clean all

.PHONY : clean
clean:
	if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)

.PHONY : debug
debug:
	cmd /c start axd -debug $(OBJ_DIR)\$(AXF_NAME)

-include $(LD_OBJS:%.o=%.d)

# Include the list of dependencies to make sure the target
# is accurate and up-to-date.