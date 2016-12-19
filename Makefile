STAGE_DIR=/code2/mips_qt/OpenWrt-SDK-15.05.1-ramips-mt7620_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2
GCC_MIPS_PATH=/code2/mips_qt/OpenWrt-Toolchain-ramips-for-mipsel_24kec+dsp-gcc-4.8-linaro_uClibc-0.9.33.2/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2
FROZEN_DIR=/code2/mips_qt/c/demo/ws_cl/28062112/frozen/frozen
GCC=$(GCC_MIPS_PATH)/bin/mipsel-openwrt-linux-g++
DIRECTIVE=-Ddebug -DMIPSEL -DMIPS
CCOPT=-fPIC -g -Wfatal-errors
INCLUDE=-I. -I.. -I$(STAGE_DIR)/usr/include -Ilws_mips/include -Ilua_inc -Ifrozen/frozen 
CC_FLAGS=$(CCOPT) $(DIRECTIVE) $(INCLUDE) -std=c++11
LIB=-nodefaultlibs -lgcc_s -lgcc -lc  -lstdc++ -lm  -lmbedtls -ldl -lwebsockets 
LD_PATH=-L$(STAGE_DIR)/usr/lib -Llws_mips/lib frozen/frozen/frozen.o -Luci -luci -lubox -llua
LD_FLAGS=$(LD_PATH) $(LIB)  
OBJDIR=./obj_mips
#luci_test.cpp
SRC=client.cpp ws_service.cpp util.cpp luci.cpp system.cpp config.cpp controller.cpp luaex.cpp responses.cpp
OBJS=$(addprefix $(OBJDIR)/,$(SRC:.cpp=.o)) obj_mips/jsonutils.o
EXE=luci_mips_test_bin

all: $(EXE)

debug:
	@echo "STAGE_DIR  $(STAGE_DIR)"
	@echo "GCC_PATH : $(GCC_MIPS_PATH)"
	@echo "GCC : $(GCC)"
	@echo "CCFLAGS : $(CC_FLAGS)"
	@echo "LDFLAGS : $(LD_FLAGS)"
	@echo "OBJS : $(OBJS)"

$(EXE): $(OBJS)
	$(GCC) $(LD_FLAGS) -o $(EXE) $^

$(OBJDIR)/%.o: %.cpp
	$(GCC) $(CC_FLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
	$(GCC) $(CC_FLAGS) -c $< -o $@

scp:
	scp luci_mips_test_bin 172.21.223.12:/cust/luci_mips_test_bin

clean:
	rm obj_mips/*.o luci_mips_test_bin