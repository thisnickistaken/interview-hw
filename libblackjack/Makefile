prefix=/usr
lib=$(prefix)/lib
inc=$(prefix)/include

ldconfig=/sbin/ldconfig

CFLAGS=-O1

all:
	$(CC) -c libblackjack.c -o libblackjack.o -fpic -g $(CFLAGS)
	$(CC) libblackjack.o -shared -Wl,-soname,libblackjack.so.0 -o libblackjack.so.0.0.0 -g

install:
	install -m 755 -o root -g root libblackjack.so.0.0.0 $(lib)
	$(ldconfig)
	ln -sf libblackjack.so.0 $(lib)/libblackjack.so
	install -m 644 -o root -g root blackjack.h $(inc)

uninstall:
	rm -f $(lib)/libblackjack.so $(lib)/libblackjack.so.0.0.0
	$(ldconfig)
	rm -f $(inc)/blackjack.h

clean:
	rm -f *.o *~ *.so*

