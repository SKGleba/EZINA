TARGET=EZINA
TITLE_ID=SKGINARW0
OBJS   = main.o font.o graphics.o

LIBS = -lSceCtrl_stub -ltaihen_stub -lSceDisplay_stub -lScePower_stub -lSceVshBridge_stub -lSceMtpIfDriver_stub -lSceAppMgr_stub

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -O3
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

%.vpk: eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "EZINA" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
    -a plugin/inarw.skprx=sce_sys/dic0.png \$@

eboot.bin: $(TARGET).velf
	vita-make-fself -c $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.png
	$(PREFIX)-ld -r -b binary -o $@ $^

clean:
	@rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf $(OBJS) \
		eboot.bin param.sfo
