##############################################################################
# Simple Makefile the the mad low-level decoder demonstration.
# (c) 2001--2004 Bertrand Petit
#-----------------------------------------------------------------------------
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# 3. Neither the name of the author nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##############################################################################
# $Name: v1_1p1 $
# $Date: 2004/02/22 03:31:15 $
# $Revision: 1.10 $
##############################################################################

##############################################################################
# Configuration section
##############################################################################

# Where are located the mad header and library?
MADINCDIR=-I/usr/local/include
MADLIBDIR=-L/usr/local/lib

# Options passed to the C compiler. ------------------------------------------
CFLAGS=$(MADINCDIR) -g
LDFLAGS=$(MADLIBDIR) -g

# These options are used by the author during development. Comment
# them if you're not using gcc.
CFLAGS+=-pedantic -Wall -Wpointer-arith -Wbad-function-cast	\
-Wcast-qual -Wcast-align -Wconversion -Wstrict-prototypes	\
-Wmissing-prototypes -Wmissing-declarations -Wnested-externs

# Uncomment the following assignment if your C preprocessor defines
# one of the 'unix', '__unix__, or '__unix' symbol and the system
# libraries does not provide the getopt() standard function.
#CPPFLAGS+=-DWITHOUT_GETOPT

# Options passed to the linker. ----------------------------------------------
# Note that the math library is only used by the wrapping code (for
# decibel handling), the mad library does not need it.
LOADLIBES=-lm -lmad
LDLIBS=$(LOADLIBES) # for BSD make

# Where all of this should be installed? -------------------------------------
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
MAN1DIR=$(PREFIX)/man/man1

# Installation-related definitions -------------------------------------------

# Command used to create a directory hierarchy.
MKDIRHIER=mkdir -p
#MKDIRHIER=mkdirhier

# Command used to install a file.
INSTALL=install -cp

##############################################################################
# Program building
##############################################################################

.PHONY: all
all: madlld madlld.1.gz

madlld: madlld.o bstdfile.o
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) -o $@

madlld.o: madlld.c bstdfile.h
bstdfile.o: bstdfile.c bstdfile.h

##############################################################################
# tests
##############################################################################

# Decode a test stream to a file
test.cdr: madlld test.mp3 
	./madlld <test.mp3 >test.cdr

# This may help some peoples.
test.wav: test.cdr
	sox test.cdr test.wav

# This uses the play command from the sox package
.PHONY: play test
play test: test.cdr
	play test.cdr

# Reminder...
test.mp3:
	@echo You should provide your own test stream in the \"test.mp3\" file.
	@exit 1

##############################################################################
# Manual page
##############################################################################

%-a4.ps: %-letter.ps
	psresize -Pletter -pa4 <$^ >$@

madlld-man-letter.ps: madlld.1
	groff -Tps -man madlld.1 >madlld-man-letter.ps

madlld.man: madlld.1
	groff -Tascii -man madlld.1 | sed 's/.//g' >madlld.man

madlld.1.gz: madlld.1
	gzip -9 <madlld.1 >madlld.1.gz

##############################################################################
# "Documentation" formating
##############################################################################

PSTITLE='Mad low-level demonstration decoder, source code version 1.1'
A2PSFLAGS=--header=$(PSTITLE) -C -T 4 --left-title='$$D{%Y-%m-%d %H:%M:%S}'
SOURCES=madlld.c bstdfile.h bstdfile.c Makefile

.PHONY:ps
ps: madlld-src-a4.ps madlld-src-letter.ps \
madlld-man-a4.ps madlld-man-letter.ps

madlld-src-a4.ps: $(SOURCES)
	a2ps $(A2PSFLAGS) -M a4 -o $@ $^

madlld-src-letter.ps: $(SOURCES)
	a2ps $(A2PSFLAGS) -M letter -o $@ $^

##############################################################################
# Installation
##############################################################################

.PHONY: install
install: $(BINDIR)/madlld $(MAN1DIR)/madlld.1.gz

# Program
$(BINDIR)/madlld: madlld $(BINDIR)
	$(INSTALL) madlld $(BINDIR)/
$(BINDIR):
	$(MKDIRHIER) $(BINDIR)

# Manual page
$(MAN1DIR)/madlld.1.gz: madlld.1.gz $(MAN1DIR)
	$(INSTALL) madlld.1.gz $(MAN1DIR)
$(MAN1DIR):
	$(MKDIRHIER) $(MAN1DIR)

##############################################################################
# Cleanup
##############################################################################

.PHONY: clean
clean:
	rm -f *.o *~ madlld test.cdr *.ps *.man *.[0-9].gz

##############################################################################
# End fof file Makefile
##############################################################################