# Default firmware. Use TARGET=AudioTest to build the hardware tests instead.
TARGET ?= LoopPedal

ifneq ($(TARGET),$(filter $(TARGET),LoopPedal AudioTest))
$(error TARGET must be LoopPedal or AudioTest)
endif

# Sources
CPP_SOURCES = $(TARGET).cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy
DAISYSP_DIR = ../../DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

