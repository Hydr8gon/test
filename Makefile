PROJ_NAME := NXEngine64
BUILD_DIR := build
SRC_DIRS  := src src/ai src/ai/almond src/ai/boss src/ai/egg src/ai/final_battle src/ai/first_cave \
             src/ai/hell src/ai/last_cave src/ai/maze src/ai/npc src/ai/oside src/ai/plantation \
             src/ai/sand src/ai/sym src/ai/village src/ai/weapons src/ai/weed src/autogen src/common \
             src/endgame src/i18n src/intro src/pause src/siflib src/TextBox n64 n64/graphics n64/sound

CPPFILES := $(filter-out src/input.cpp src/main.cpp,$(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp)))
HFILES   := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.h))
OFILES   := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CPPFILES))

include $(N64_INST)/include/n64.mk

N64_CXXFLAGS += -Os -w -Ideps -DNO_SDL

all: $(PROJ_NAME).z64

$(PROJ_NAME).z64: N64_ROM_TITLE = "$(PROJ_NAME)"
$(PROJ_NAME).z64: N64_ROM_SAVETYPE = sram256k

$(BUILD_DIR)/$(PROJ_NAME).elf: $(OFILES)

$(OFILES): $(HFILES)

clean:
	rm -rf $(BUILD_DIR) $(PROJ_NAME).z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
