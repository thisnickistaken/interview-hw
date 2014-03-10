prefix=/usr
bin=$(prefix)/bin

ldconfig=/sbin/ldconfig

CFLAGS=-O1

all:
	$(CC) -c blackjack-cli.c -o blackjack-cli.o -g $(CFLAGS)
	$(CC) -c blackjack-gamestate-test.c -o blackjack-gamestate-test.o -g $(CFLAGS)
	$(CC) blackjack-cli.o -lblackjack -o blackjack-cli -g
	$(CC) blackjack-gamestate-test.o -lblackjack -o blackjack-gamestate-test -g

install:
	install -m 755 -o root -g root blackjack-cli $(bin)
	install -m 755 -o root -g root blackjack-gamestate-test $(bin)

uninstall:
	rm -f $(bin)/blackjack-cli $(bin)/blackjack-gamestate-test

clean:
	rm -f blackjack-cli blackjack-gamestate-test *.o *~

