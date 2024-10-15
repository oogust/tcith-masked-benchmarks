#	Makefile
#	Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

# By default, we use the packaged picolibc as a standard library
USE_PICOLIBC ?= 1

#	target
PROJ	=	sloth
BUILD	=	_build
PROF	=	_prof

#	rtl
RTL		=	$(wildcard rtl/*.v)
VFLAGS	=	-Irtl --threads 1
VVP		=	$(BUILD)/$(PROJ).vvp
PC_LOG	=	core_pc.log

#	cross compiler
FW		?=	firmware
CCONF_H	?=	config.h
XCHAIN	?= 	riscv64-unknown-elf-
CFLAGS	= 	-O3 -Wall -g -mabi=ilp32 -march=rv32imc
CFLAGS	+=	-include $(CCONF_H) -DNDEBUG
# If we use picolibc, adapt the flags.
# NOTE: the local ld script mostly provides flash and RAM layout
# (in our case flash starts at 0 and is in fact part of RAM)
ifeq ($(USE_PICOLIBC),1)
    CFLAGS      +=      -specs=picolibc.specs
    LDFLAGS	= 	-Wl,-Bstatic,-T,flow/picolibc.ld,--no-relax
else
    LDFLAGS	= 	-Wl,-Bstatic,-T,flow/riscv.ld,--no-relax
endif
KATNUM	?=	10
CFLAGS	+=	-DKATNUM=$(KATNUM)

#	firmware sources
CSRC	+=	$(wildcard drv/*.c slh/*.c)
SSRC	+=	$(wildcard drv/*.S)
OBJS	+=	$(CSRC:.c=.o) $(SSRC:.S=.o)
CFLAGS	+=	-Idrv -Islh

ifeq ($(SHA_HW),1)
    CFLAGS += -DSHA_HW
endif

#	default target
.PHONY : veri
all:	libmpcith prim #veriprerequisites
prof:	$(PROF)/func.txt

PRIM_PATH = primitives

prim: 
	cd $(PRIM_PATH) && CC=$(XCHAIN)gcc make

prim_clean: 
	cd $(PRIM_PATH) && CC=$(XCHAIN)gcc $(MAKE) clean

# MQOM remated folder
MQOM_PATH = mqom
MQOM ?= mqom_cat1_gf251_fast

LIBS = $(MQOM_PATH)/sha3/libhash.a $(MQOM_PATH)/$(MQOM)/libsign.a $(PRIM_PATH)/libprim.a

CFLAGS += -I../../drv -I../../slh -I../../ -include ../../config.h -I./primitives
CFLAGS += -I$(MQOM_PATH)/$(MQOM) -I$(MQOM_PATH)/ -I$(MQOM_PATH)/sha3 -I$(MQOM_PATH)/sha3/opt64

# libmpcith
libmpcith: $(CCONF_H)
	cd $(MQOM_PATH)/$(MQOM) && CC=$(XCHAIN)gcc EXTRA_ALL_FLAGS="$(CFLAGS)" CFLAGS="$(CFLAGS)" make libsign

libmpcith_clean:
	cd $(MQOM_PATH)/$(MQOM) && CC=$(XCHAIN)gcc $(MAKE) clean

#	verilator

veri:	$(BUILD)/Vsim_tb $(FW).hex
	./$(BUILD)/Vsim_tb

$(BUILD)/Vsim_tb: $(BUILD)/Vsim_tb.mk
	cd $(BUILD) && $(MAKE) -f Vsim_tb.mk CC=gcc LDFLAGS=""

$(BUILD)/Vsim_tb.mk: $(RTL) flow/sim_tb.cpp
	verilator $(VFLAGS) -Mdir $(BUILD) -cc --exe \
		--top-module sim_tb -DSIM_TB $(RTL) flow/sim_tb.cpp

#	icarus verilog

sim:	$(VVP)
	vvp -N $(VVP)

$(VVP):	$(BUILD) $(RTL) $(FW).hex
	iverilog $(VFLAGS) -DSIM_TB -o $(VVP) $(RTL)

#	get firmware configuration from config.vh

$(CCONF_H):	rtl/config.vh
	cat $^ | tr '`' '#' | \
		grep -vi -e "#timescale" -e "#default_nettype" > $(CCONF_H)
	

#	FPGA bitstream generation

cw305.bit: $(BUILD) $(RTL) $(FW).hex
	cp $(FW).hex $(BUILD)
	cd $(BUILD) && $(VIVADO)vivado -mode batch \
		-log cw305_synth.log -source ../flow/xc7a100t-synth.tcl

vcu118.bit: $(BUILD) $(RTL) $(FW).hex
	cp $(FW).hex $(BUILD)
	cd $(BUILD) && $(VIVADO)vivado -mode batch \
		-log vcu118_synth.log -source ../flow/xcvu9p-synth.tcl

#	program FPGA device

prog_cw305:	cw305.bit
	python3 flow/prog_cw305.py

prog_vcu118:   vcu118.bit $(FW).hex
	$(VIVADO)vivado -nolog -mode batch -source flow/prog_vcu118.tcl

#	default targets

bits:	cw305.bit
prog:	prog_cw305

term:
	tio  -m INLCRNL /dev/ttyUSB1

#	asic area & timing estimate

synth:
	cd flow/yosys-syn && $(MAKE) synth

#	build firmware

$(FW).elf:	$(OBJS)
	$(XCHAIN)gcc $(LDFLAGS) $(CFLAGS)  $(OBJS)	-o $@ $(LIBS)
	$(XCHAIN)size -t $(OBJS)
	$(XCHAIN)size -t $(FW).elf

%.o: %.c $(CCONF_H)
	$(XCHAIN)gcc $(CFLAGS) -c -o $@ $<

%.o: %.S $(CCONF_H)
	$(XCHAIN)gcc $(CFLAGS) -c -o $@ $<

$(FW).dis:	$(FW).elf
	$(XCHAIN)objdump -g -d -l -S --source-comment='#' $^ > $@

$(FW).bin:	$(CCONF_H) $(FW).elf $(CSRC)
	$(XCHAIN)objcopy -O binary $(FW).elf $@

$(FW).hex:	$(FW).bin
	hexdump -v -e '1/4 "%08x\n"' $^ > $@

#	profiling with the program counter log
#	requires CORE_PC_LOG macro to be manually set in verilator!

$(PC_LOG):	veri

$(FW).pmap:	$(PC_LOG) $(FW).elf $(BUILD)
	$(XCHAIN)addr2line  -a -C -f -i -e $(FW).elf < $(PC_LOG) > $@

$(PROF)/func.txt:	$(FW).pmap
	rm -rf $(PROF)
	mkdir $(PROF)
	cd $(PROF) && ../flow/eprof.py

#	cleanup

$(BUILD):
	mkdir -p $(BUILD)

clean: libmpcith_clean prim_clean
	$(RM)	-f	$(FW).* $(CCONF_H) $(OBJS) $(VVP)
	$(RM)	-rf $(PROF) $(BUILD)
	$(RM)	-f	*.jou *.log *.bit
	$(MAKE) -f Makefile.docker clean
	cd slh && $(MAKE) clean
	cd flow/yosys-syn && $(MAKE) clean
