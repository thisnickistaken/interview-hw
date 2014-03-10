#!/usr/bin/ruby

require "blackjack"
require "base64"

include BlackJack

game = Game.new()

#file = File.new("tmp", "r")
#image = file.read

game.add_player("Steven", 1000.0)
game.add_player("Moo Cow", 2000.0)

game.shuffle_deck
game.deal
game.print
saved = game.save

puts "----------------------------------------------------"

game2 = Game.new()
game2.restore(saved)
game2.print

puts Base64.encode64(saved);
puts Base64.encode64(game2.save);
