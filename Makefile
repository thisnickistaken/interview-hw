prefix=/usr
bin=$(prefix)/bin
lib=$(prefix)/lib
inc=$(prefix)/include
CFLAGS=-O1

all:
	$(CC) -c libblackjack.c -o libblackjack.o -fpic -g $(CFLAGS)
	$(CC) -c blackjack-cli.c -o blackjack-cli -g $(CFLAGS)
	$(CC) libblackjack.o -shared -Wl,-soname,libblackjack.so.0 -o libblackjack.so.0.0.0 -g
	$(CC) -L. -lblackjack blackjack-cli.o -o blackjack-cli -g

install:
	install -m 755 -o root -g root libblackjack.so libblackjack.so.0 $(lib)
	install -m 644 -o root -g root blackjack.h $(inc)

uninstall:
	rm -f $(lib)/libblackjack.so $(lib)/libblackjack.so.0
	rm -f $(inc)/blackjack.h

clean:
	rm *.o *~ *.so*

