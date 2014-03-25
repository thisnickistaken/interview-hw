#!/usr/bin/ruby

require "blackjack"
require "sinatra"

include BlackJack

game = nil

class Game
	def to_html
		ret = ""
		unless self.players_seated
			ret += "<h4>To Add Players:</h4>"
			ret += "<h5>Fill in a player name and balance</h5>"
			ret += "<h6>then click on the</h6>"
			ret += "<h6>'Add' button.</h6>"
		end
		unless (player = self.controlling_player) == nil
			x = 0
			self.each_hand(player.get_name) do |hand|
				if hand.get_state == HAND_IN_PLAY
					ret += "<h4>#{player.get_name}</h4>"
					ret += "<h5>Balance: $#{player.get_balance}</h5>"
					ret += "<h6>Bet: $#{hand.get_bet}</h6>"
					ret += "<h6>#{state_to_str(hand.get_state)}</h6>"
					cards = ""
					count = 0
					self.each_card(player.get_name, x) do |card|
						cards += card.to_html
						count += 1
					end
					ret += "<div id=\"cards\" style=\"width: #{count * 79}px\">#{cards}</div>"
				end
				x += 1
			end
		else
			unless self.get_dealer_state == HAND_IN_PLAY
				ret += "<h1>GAME END</h1>"
			else
				ret += "<h4>Place bets</h4>"
				ret += "<h5>then deal game</h5>"
				ret += "<h6>when you are</h6>"
				ret += "<h6>done adding players</h6>"
			end
		end
		
		return ret
	end
end

class Card
	def to_html
		return "<div class=\"cards\" style=\"background-position: #{(self.get_value - 1) * -79}px #{(self.get_suit - 1) * -123}px;\"></div>"
	end
end

if File.exists?("parts/header.html")
	$header = File.open("parts/header.html", "r").read
else
	puts "Header does not exist!"
end
if File.exists?("parts/header-mid.html")
	$header_mid = File.open("parts/header-mid.html", "r").read
else
	puts "Header-mid does not exist!"
end
if File.exists?("parts/footer.html")
	$footer = File.open("parts/footer.html", "r").read
else
	puts "Footer does not exist!"
end
if File.exists?("parts/main.css")
	$css = File.open("parts/main.css", "r").read
else
	puts "CSS does not exist!"
end
if File.exists?("parts/cards.png")
	$card_image = File.open("parts/cards.png", "r").read
else
	puts "CSS does not exist!"
end

get "*/main.css" do
	headers "Content-type" => "text/css"
	$css
end

get "*/cards.png" do
	headers "Content-type" => "image/png"
	$card_image
end

get "/" do
	display game
end

get "/new" do
	game = Game.new
	display game
end

get "/add/*:*" do |name, balance|
	if name == ""
		"<h1>No name specified for player</h1>"
	end
	if balance == ""
		"<h1>No balance specified for player</h1>"
	end
	unless (ret = game.add_player(name, balance.to_f)) == 0
		"<h1>Error adding player: #{error_to_str(ret)}</h1>"
	end
	display game
end

get "/remove/*" do |name|
	if name == ""
		return "<h1>No name specified for player</h1>"
	end
	unless (ret = game.remove_player(name)) == 0
		return "<h1>Error removing player: #{error_to_str(ret)}</h1>"
	end
	display game
end

get "/bet/*:*" do |name, bet|
	if name == ""
		return "<h1>No name specified for player</h1>"
	end
	if bet == ""
		return "<h1>No bet specified</h1>"
	end
	unless (ret = game.place_bet(name, bet.to_f)) == 0
		return "<h1>Error placing bet: #{error_to_str(ret)}</h1>"
	end
	display game
end

get "/deal" do
	unless (ret = game.deal) == 0
		return "<h1>Error dealing game: #{error_to_str(ret)}</h1>"
	end
	display game
end

get "/shuffle" do
	unless (ret = game.shuffle_deck) == 0
		return "<h1>Error shuffling deck: #{error_to_str(ret)}</h1>"
	end
	display game
end

get "/play/*:*" do |name, action|
	if name == ""
		return "<h1>No name specified for player</h1>"
	end
	if action == ""
		return "<h1>No action specified for player</h1>"
	end
	unless (ret = game.play_hand(name, str_to_action(action))) == 0
		return "<h1>Action failed: #{error_to_str(ret)}</h1>"
	end
	display game
end

not_found do
	"<h1>Invalid Request</h1>"
end

def check_resolve(game)
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
end

def resolve(game)
	game.dealer_playing_loop do
		unless (ret = game.play_dealer) == 0
			return ret
		end
	end
	unless (ret = game.resolve) == 0
		return ret
	end
	
	return 0
end

def display(game)
	ret = $header
	ret += $header_mid
	unless game == nil
		if check_resolve(game)
			resolve game
		end
		ret += game.to_html
	else
		ret += "<h4>Click</h4>"
		ret += "<h5>'New Game'</h5>"
		ret += "<h6>to start</h6>"
		ret += "<h6>a new game</h6>"
		
	end
	ret += $footer
	
	return ret
end
