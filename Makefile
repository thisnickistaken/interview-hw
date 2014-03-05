prefix=/usr
bin=$(prefix)/bin
lib=$(prefix)/lib
inc=$(prefix)/include

ldconfig=/sbin/ldconfig

CFLAGS=-O1

all:
	$(CC) -c libblackjack.c -o libblackjack.o -fpic -g $(CFLAGS)
	$(CC) -c blackjack-cli.c -o blackjack-cli.o -g $(CFLAGS)
	$(CC) libblackjack.o -shared -Wl,-soname,libblackjack.so.0 -o libblackjack.so.0.0.0 -g
	ln -sf libblackjack.so.0.0.0 libblackjack.so.0
	ln -sf libblackjack.so.0 libblackjack.so
	$(CC) -L. -lblackjack blackjack-cli.o -o blackjack-cli -g

install:
	install -m 755 -o root -g root libblackjack.so.0.0.0 $(lib)
	$(ldconfig)
	ln -s libjacksock.so.0 $(lib)/libblackjack.so
	install -m 755 -o root -g root blackjack-cli $(bin)
	install -m 644 -o root -g root blackjack.h $(inc)

uninstall:
	rm -f $(lib)/libblackjack.so $(lib)/libblackjack.so.0
	rm -f $(inc)/blackjack.h

clean:
	rm -f blackjack-cli *.o *~ *.so*

