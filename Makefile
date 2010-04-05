ENCODER_SRCS=amrenc.c sp_enc.c interf_enc.c

CFLAGS=-O3 -lm -Wall

all: amrenc
	strip -s -R.comment amrenc

amrenc: $(ENCODER_SRCS)
	$(CC) $(CFLAGS) $(ENCODER_SRCS) -o amrenc

clean:
	rm -rf *.o *~ amrenc

test: all
	time ./amrenc audiodump.wav audiodump.amr

install: all
	mkdir -p $(INSTDIR)/bin
	cp amrenc $(INSTDIR)/bin
	strip -s -R.comment $(INSTDIR)/bin/amrenc
