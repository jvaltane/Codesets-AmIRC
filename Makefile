# Not 100% standard Makefile for amirc plugins...
# These are about the only lines that need change
APP = Codesets
O_FILES = libinit$(EXT).o $(APP)$(EXT).o

CC = m68k-amigaos-gcc
STRIP = m68k-amigaos-strip
FLAGS = -I. -Iincludes -g -nostdlib -Wall -O2 -fomit-frame-pointer -msmall-code -Wno-attributes
CFLAGS = $(FLAGS)
LFLAGS = $(FLAGS) -s

# Everything below here should work ok - unles you add more source files...
$(APP).AmIPlug$(EXT): $(O_FILES)
	echo Making $@...
	$(CC) $(LFLAGS) $(PROCESSOR) -o $@ $(O_FILES) -lamiga
	$(STRIP) $@

libinit$(EXT).o: libinit.c
	echo Making $@...
	$(CC) $(CFLAGS) $(PROCESSOR) -c -o $@ $^

$(APP)$(EXT).o: $(APP).c
	echo Making $@...
	$(CC) $(CFLAGS) $(PROCESSOR) -c -o $@ $(APP).c

all:
	$(MAKE) EXT=''     PROCESSOR=''        --no-print-directory
	$(MAKE) EXT='_020' PROCESSOR='-m68020' --no-print-directory
	$(MAKE) EXT='_030' PROCESSOR='-m68030' --no-print-directory
	$(MAKE) EXT='_040' PROCESSOR='-m68040' --no-print-directory
	$(MAKE) EXT='_060' PROCESSOR='-m68060' --no-print-directory
	echo Now \"make release\" for the archives...

release: all
	rm -f $(APP).lha
	echo Copying to temporary directory...
	mkdir -p $(APP)-AmIRC/Plugins $(APP)-AmIRC/Src
	cp Install Install.info $(APP).readme LICENSE $(APP)-AmIRC/
	cp $(APP).AmIPlug $(APP).AmIPlug_020 $(APP).AmIPlug_030 $(APP).AmIPlug_040 $(APP).AmIPlug_060 $(APP)-AmIRC/Plugins/
	cp -r *.c *.h includes IMPORTANT $(APP).readme LICENSE $(APP)-AmIRC/Src/
	echo Creating archive...
	lha a -0o5 $(APP)-AmIRC.lha $(APP)-AmIRC
	rm -rf $(APP)-AmIRC
	echo Done

clean:
	echo Cleaning...
	rm -f *.o $(APP).AmIPlug $(APP).AmIPlug_020 $(APP).AmIPlug_030 $(APP).AmIPlug_040 $(APP).AmIPlug_060 $(APP)-AmIRC.lha

.SILENT:
