#!/usr/bin/ruby

require "blackjack"

include BlackJack

game = Game.new()

while !print("Enter player name, leave blank when done: ") && ((player = gets.chomp()) != "") do
	print "Enter balance for #{player}: "
	game.add_player(player, gets.chomp().to_f());
end

if ret = game.shuffle_deck() != 0
	puts "Error: Failed to shuffle deck: #{error_to_str(ret)}"
	exit 1
end

game.each_player do |name|
	printf "#{name}, enter your wager: "
	case ret = game.place_bet(name, gets.chomp().to_f())
		when 0
		when BJE_BET || BJE_NEG_BET
			puts error_to_str(ret)
		else
			puts "Fatal: #{error_to_str(ret)}"
			exit 1
	end
end

game.deal()

game.each_player do |name|
	game.playing_loop(name) do
		game.print_player(name)
		print "#{name}, what would you like to do? "
		case ret = game.play_hand(name, str_to_action(gets.chomp()))
			when 0
			when BJE_ACTION || BJE_BET || BJE_FIRST || BJE_SPLIT
				puts error_to_str(ret)
			else
				puts "Fatal: #{error_to_str(ret)}"
				exit 1
		end
	end
end

game.dealer_playing_loop do
	game.print_dealer()
	game.play_dealer()
end

game.resolve()

game.print()
