#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TARGET	:= universal-edit
export TOPDIR	:= $(CURDIR)

# These set the information text in the nds file
GAME_TITLE		:= Universal-Edit
GAME_SUBTITLE1	:= Universal-Team

include $(DEVKITARM)/ds_rules

.PHONY: checkarm9 graphics clean

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: checkarm9 $(TARGET).nds

#---------------------------------------------------------------------------------
checkarm9:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
$(TARGET).nds	: graphics $(NITRO_FILES) arm9/$(TARGET).elf
	ndstool	-c $(TARGET).nds -9 arm9/$(TARGET).elf \
	-b1 icon.bmp "$(GAME_TITLE);$(GAME_SUBTITLE1)" \
	-z 80040000 -u 00030004 -a 00000138

#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	rm -f $(TARGET).nds $(TARGET).arm9
