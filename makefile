CFLAGS= -Wall -ansi -O3 -g -D_BSD_SOURCE

all: msgserv rmb

msgserv: msgserv.c 
	gcc $(CFLAGS) msgserv.c -o msgserv

rmb: rmb.c
	gcc $(CFLAGS) rmb.c -o rmb 

clean:
	rm -f *.o *.~ msgserv *.gch
	rm -f *.o *.~ rmb *.gch
