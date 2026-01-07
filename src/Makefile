#
# GNU makefile for flabc
#==============================


# system specific settings
#--------------------------------

# uncomment for universal binary on OSX
#ARCH = -arch i386 -arch ppc

# for support of older macos versions (comment out if unwanted)
ifeq ($(shell uname -s),Darwin)
	OSXMINVERSION = -mmacosx-version-min=10.13
endif 

# settings for GNU C compiler
CC = g++
#CFLAGS = -Wall -g -fpermissive `fltk-config --cxxflags` $(ARCH)
CFLAGS = -Wall -O2 -fpermissive `fltk-config --cxxflags` $(ARCH) $(OSXMINVERSION)
LDFLAGS = `fltk-config --ldflags` -lstdc++ $(ARCH) $(OSXMINVERSION)

# installation directories
# (adjust to your preferences)
#-----------------------------------------

# install root
PREFIX = /usr/local
# directory for the binary
BINDIR = $(PREFIX)/bin
# directories for templates and examples
TEMPLATEDIR =  $(PREFIX)/share/flabc/templates
EXAMPLEDIR =  $(PREFIX)/share/flabc/examples
# directory for the font library
DOCDIR =  $(PREFIX)/share/doc/flabc


# from here on, no alterations
# should be necessary
#----------------------------------

PROGRAM = flabc
OBJECTS = messagebox_msk.o messagebox.o \
	dialogs_msk.o dialogs.o \
	mainwin_msk.o mainwin.o \
	editor.o config.o utils.o os.o \
	fltkextensions.o flabc.o

OSXAPPDIR = ../osx/app-template
WINAPPDIR = ../win/winapp

# do not remove intermediate cpp files
.PRECIOUS: %.cpp

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) -o $@ $+ $(LDFLAGS)

# for standalone app on osx
addres: $(PROGRAM)
	fltk-config --post $(PROGRAM)

static: $(OBJECTS)
	$(CC) -o $(PROGRAM) $+ --static $(LDFLAGS)
	strip $(PROGRAM)

universal:
	ARCH="-arch i386 -arch ppc"; export ARCH; make

rpm: $(PROGRAM)
	# check version
	test `grep ' VERSION ' flabc.h | cut -f 2 -d '"'` = `grep '%version ' flabc.list | cut -f 2 -d ' '` || \
		{ echo "different versions in flabc.h and flabc.list"; exit 1; }
	epm -v -n -f rpm $(PROGRAM)

deb: $(PROGRAM)
	# check version
	test `grep ' VERSION ' flabc.h | cut -f 2 -d '"'` = `grep '%version ' flabc.list | cut -f 2 -d ' '` || \
		{ echo "different versions in flabc.h and flabc.list"; exit 1; }
	@test  `id -u` = 0 || { echo "Must be root for 'make deb'"; exit 1; }
	epm -v -n -f deb $(PROGRAM)

osxapp: $(PROGRAM)
	cp flabc $(OSXAPPDIR)/Contents/MacOS/
	strip $(OSXAPPDIR)/Contents/MacOS/flabc

winapp: $(OBJECTS)
	cd logo; windres -o ../winicon.o winicon.rc; cd ..
	gcc -o $(PROGRAM) $+ winicon.o $(LDFLAGS) -static
	cp flabc.exe $(WINAPPDIR)/bin
	strip $(WINAPPDIR)/bin/flabc.exe

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c $*.cpp

%.cpp %.h: %.fl
	fluid -c -o $*.cpp $<

install:
	install -m 755 -s $(PROGRAM) $(BINDIR)/$(PROGRAM)
	install -d $(DOCDIR)
	install -m 644 ../README.md ../CHANGES ../LICENSE ../doc/* $(DOCDIR)
	install -d $(TEMPLATEDIR)
	install -m 644 ../templates/* $(TEMPLATEDIR)
	install -d $(EXAMPLEDIR)
	install -m 644 ../examples/* $(EXAMPLEDIR)

uninstall:
	rm -f $(BINDIR)/$(PROGRAM)
	rm -f $(DOCDIR)/README.md $(DOCDIR)/CHANGES $(DOCDIR)/LICENSE
	@for f in ../doc/*; do \
		echo "rm -f $(DOCDIR)/`basename $$f`"; \
		rm -f $(DOCDIR)/`basename $$f`; \
	done
	rmdir $(DOCDIR) || :
	@for f in ../templates/*; do \
		echo "rm -f $(TEMPLATEDIR)/`basename $$f`"; \
		rm -f $(TEMPLATEDIR)/`basename $$f`; \
	done
	rmdir $(TEMPLATEDIR) || :
	@for f in ../examples/*; do \
		echo "rm -f $(EXAMPLEDIR)/`basename $$f`"; \
		rm -f $(EXAMPLEDIR)/`basename $$f`; \
	done
	rmdir $(EXAMPLEDIR) || :

tags: TAGS
TAGS: *.cpp *.h
	etags *.cpp *.h

clean:
	rm -f $(PROGRAM) $(PROGRAM).exe $(OBJECTS) winicon.o
	rm -f $(OSXAPPDIR)/Contents/MacOS/flabc
	rm -f $(WINAPPDIR)/bin/flabc.exe

