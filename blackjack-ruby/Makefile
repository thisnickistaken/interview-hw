prefix=/usr
bin=$(prefix)/bin

install:
	install -m 755 -o root -g root blackjack-cli.rb $(bin)
	install -m 755 -o root -g root blackjack-gamestate-test.rb $(bin)

uninstall:
	rm -f $(bin)/blackjack-cli.rb $(bin)/blackjack-gamestate-test.rb
