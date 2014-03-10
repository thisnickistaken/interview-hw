#!/usr/bin/ruby

require "blackjack"

include BlackJack

game = Game.new()

while !print("Enter player name, leave blank when done: ") && ((name = gets.chomp()) != "") do
	print "Enter balance for #{name}: "
	game.add_player(name, gets.chomp().to_f());
end

while game.players_seated
	unless (ret = game.shuffle_deck()) == 0
		puts "Error: Failed to shuffle deck: #{error_to_str(ret)}"
		exit 1
	end
	
	game.each_player do |player|
		begin
			printf "#{player.get_name}, enter your wager: "
			case ret = game.place_bet(player.get_name, gets.chomp().to_f())
				when 0
				when BJE_BET, BJE_NEG_BET
					puts error_to_str(ret)
				else
					puts "Fatal: #{error_to_str(ret)}"
					exit 1
			end
		end while ret != 0
	end
	
	unless (ret = game.deal()) == 0
		puts "Error: #{error_to_str(ret)}"
		exit 1
	end
	
	game.each_player do |player|
		game.playing_loop(player.get_name) do
			game.print_player(player.get_name)
			print "#{player.get_name}, what would you like to do? "
			case ret = game.play_hand(player.get_name, str_to_action(gets.chomp()))
				when 0
				when BJE_ACTION, BJE_BET, BJE_FIRST, BJE_SPLIT
					puts error_to_str(ret)
				else
					puts "Fatal: #{error_to_str(ret)}"
					exit 1
			end
		end
	end
	
	game.dealer_playing_loop do
		game.print_dealer()
		unless (ret = game.play_dealer()) == 0
			puts "Error: #{error_to_str(ret)}"
			exit 1
		end
	end
	
	unless (ret = game.resolve()) == 0
		puts "Error: #{error_to_str(ret)}"
		exit 1
	end
	
	game.print()
	
	names = Array.new();
	game.each_player do |player|
		if player.get_balance == 0
			puts "#{player.get_name} has run out of money!"
			names.push(player.get_name)
		end
	end
	names.each do |name|
		puts "Retiring #{name}."
		unless (ret = game.remove_player(name)) == 0
			puts "Error: #{error_to_str(ret)}"
			exit 1
		end
	end
end	
