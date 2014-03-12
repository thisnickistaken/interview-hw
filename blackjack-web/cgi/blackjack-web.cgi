#!/usr/bin/ruby

require "blackjack"
require "cgi"

include BlackJack

class Game
	def to_json
		players = ""
		self.each_player do |player|
			x = 0;
			hands = ""
			self.each_hand(player.get_name) do |hand|
				cards = ""
				self.each_card(player.get_name, x) do |card|
					cards += "#{(cards == "" ? "" : ",")}{suit:#{card.get_suit},value:#{card.get_value}}"
				end
				hands += "#{(hands == "" ? "" : ",")}{state:'#{state_to_str(hand.get_state)}',bet:#{hand.get_bet},cards:[#{cards}]}"
				x += 1;
			end
			players += "#{(players == "" ? "" : ",")}{name:'#{player.get_name}',balance:#{player.get_balance},hands:[#{hands}]}"
		end
		cards = ""
		self.each_dealer_card do |card|
			cards += "#{(cards == "" ? "" : ",")}{suit:#{card.get_suit},value:#{card.get_value}}"
		end
		return "{dealer:{state:'#{state_to_str(self.get_dealer_state)}',cards:[#{cards}]},seats:[#{players}]}"
	end
end

def error_json(description)
	return "{error:{description:'#{description}'}}"
end

game = Game.new
cgi = CGI.new

savedir = "/home/blackjack"

print "Content-type: application/json; charset=utf8\r\n"
print "Pragma: no-cache\r\n"
print "Cache-control: no-cache,must-revalidate\r\n"
print "\r\n"

if cgi["game"] == ""
	print error_json("Game name not supplied.")
	exit 0
end
if cgi["action"] != "new"
	game.restore(File.open(savedir + "/" + cgi["game"], "r").read);
end
save = true
case cgi["action"]
	when "new", "shuffle"
		if (ret = game.shuffle_deck) != 0
			print error_json("Failed to shuffle deck: #{error_to_str(ret)}")
			exit 0
		end
	when "load"
		save = false
	when "add"
		if cgi["player"] == ""
			print error_json("No player specified for addition to game.")
			exit 0
		end
		if cgi["balance"] == ""
			print error_json("No balance specified for #{cgi["player"]}.")
			exit 0
		end
		if (ret = game.add_player(cgi["player"], cgi["balance"].to_f)) != 0
			print error_json("Player addition error: #{error_to_str(ret)}")
			exit 0
		end
	when "remove"
		if cgi["player"] == ""
			print error_json("No player specified for removal from game.")
			exit 0
		end
		if (ret = game.remove_player(cgi["player"])) != 0
			print error_json("Player removal error: #{error_to_str(ret)}")
			exit 0
		end
	when "bet"
		if cgi["player"] == ""
			print error_json("No player specified for bet.")
			exit 0
		end
		if cgi["bet"] == ""
			print error_json("No bet specified.")
			exit 0
		end
		if (ret = game.place_bet(cgi["player"], cgi["bet"].to_f)) != 0
			print error_json("Failed to place bet: #{error_to_str(ret)}")
			exit 0
		end
	when "deal"
		if (ret = game.deal) != 0
			print error_json("Failed to deal game: #{error_to_str(ret)}")
			exit 0
		end
	else
		if cgi["player"] == ""
			print error_json("Player must be supplied for actions other than \"New\" and \"Load\".")
			exit 0
		end
		if (ret = game.play_hand(cgi["player"], str_to_action(cgi["action"]))) != 0
			print error_json("Action failed: #{error_to_str(ret)}.")
			exit 0
		end
end
resolve = true
if game.players_seated == false || game.get_dealer_state != HAND_IN_PLAY
	resolve = false
else
	game.each_player do |player|
		game.each_hand(player.get_name) do |hand|
			if hand.get_state == HAND_IN_PLAY
				resolve = false
				break
			end
		end
		if not resolve
			break
		end
	end
end
if resolve
	game.dealer_playing_loop do
		if (ret = game.play_dealer) != 0
			print error_json("Failed to play dealer: #{error_to_str(ret)}")
			exit 0
		end
	end
	if (ret = game.resolve) != 0
		print error_json("Failed to resolve game: #{error_to_str(ret)}")
		exit 0
	end
end
if save
	File.open(savedir + "/" + cgi["game"], "w").write(game.save)
end
print "{game:#{game.to_json}}"
