CC = gcc
LD = gcc

CFLAGS = -O2
SDL_CFLAGS = `sdl-config --cflags`
LFLAGS = 

SDL_LIBS = `sdl-config --libs`

FILES = Make4psp Makefile Makefile.lib mdxmini.txt
FILES_ORG = COPYING AUTHORS

ZIPSRC = mdxmini`date +"%y%m%d"`.zip
TOUCH = touch -t `date +"%m%d0000"`


OBJS = sdlplay.o libmdxmini.a
SRCDIR = src

TARGET = mdxplay

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(LD) $(LFLAGS) -o $@ $(OBJS) $(SDL_LIBS)

libmdxmini.a:
	make -f Makefile.lib

%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(SDL_CFLAGS) -o $@ $<

clean :
	make -f Makefile.lib clean
	rm -f $(TARGET) $(OBJS)

dist :
	find . -name ".DS_Store" -exec rm -f {} \;
	find $(FILES) $(SRCDIR) -exec $(TOUCH) {} \;
	
	rm -f $(ZIPSRC)
	zip -r $(ZIPSRC) $(SRCDIR) $(FILES) $(FILES_ORG)

	
