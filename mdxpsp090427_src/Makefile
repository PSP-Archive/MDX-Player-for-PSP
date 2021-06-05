
all: build copy

ZIPBIN = mdxpsp`date +"%y%m%d"`.zip
ZIPSRC = mdxpsp`date +"%y%m%d"`_src.zip

SRCPACK = src MDXPSP.PNG MDXPSP.PNG make4psp Makefile
SRCPACK += mdxpsp.txt pspcopy.sh pspum.sh

BINPACK = __SCE__* %__SCE__* EBOOT.PBP mdxpsp.txt

TOUCH = touch -t `date +"%m%d0100"`

build:
	make -f make4psp
	make -f make4psp SCEkxploit

clean:
	make -f make4psp clean

copy:
	sh pspcopy.sh
	sh pspum.sh
	
srcdist :
	find . -name ".DS_Store" -exec rm -f {} \;
	find $(SRCPACK) -exec $(TOUCH) {} \;

	rm -f $(ZIPSRC)
	zip -r $(ZIPSRC) $(SRCPACK)
	$(TOUCH) $(ZIPSRC)
 
	
bindist :
	find . -name ".DS_Store" -exec rm -f {} \;
	find  $(BINPACK) -exec $(TOUCH) {} \;

	rm -f $(ZIPBIN)
	zip -r $(ZIPBIN) $(BINPACK)
	$(TOUCH) $(ZIPBIN)

