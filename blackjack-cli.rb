#!/usr/bin/ruby

require "blackjack"

include BlackJack

game = Game.new()

puts game.add_player("Steven", 1000.00);
puts game.shuffle_deck()

puts game.place_bet("Steven", 500.00);

puts game.deal()

while game.playing("Steven") do
	print "What would you like to do? "
	case ret = game.play_hand("Steven", str_to_action(gets.chomp()))
		when 0
		when BJE_ACTION || BJE_BET || BJE_FIRST || BJE_SPLIT
			puts error_to_str(ret)
		else
			puts "Fatal: #{error_to_str(ret)}"
	end
end

while game.dealer_playing() do
	game.play_dealer()
end

puts game.resolve()
