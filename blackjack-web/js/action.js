var a_game = new XMLHttpRequest();
var player_name = "";
var game_name = "";

function play_hand(action)
{
	//send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "game=" + game + "&player=" + player + "&action=" + action);
	send_ajax("tmp.json", "post", a_game, change_display, "game=" + game_name + "&player=" + player_name + "&action=" + action);
}

function load_game(name)
{
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "action=load&game=" + game_name);
}

function new_game(name)
{
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "action=new&game=" + game_name);
}

function change_display()
{
	var x,y;
	
	if(a_game.readyState == 4)
	{
		eval(a_game.responseText);
		
		if(game != undefined)
		{
			for(x = 0; x < game.seats.length; x++)
			{
				for(y = 0; y < game.seats[x].hands.length; y++)
				{
					if(game.seats[x].hands[y].state == "In Play")
					{
						display_player(game.seats[x]);
						display_hand(game.seats[x].hands[y]);
					}
				}
			}
		}
		if(error != undefined)
			alert(error.description);
	}
}

function display_hand(hand)
{
	var x, bet;
	
	clear_cards();
	for(x = 0; x < hand.cards.length; x++)
		add_card(hand.cards[x].suit, hand.cards[x].value);
	bet = "Bet:";
	if(hand.bet != undefined)
		bet += " $" + hand.bet;
	document.getElementById("bet").innerHTML = bet;
	document.getElementById("state").innerHTML = hand.state;
}

function display_player(player)
{
	player_name = player.name;
	document.getElementById("player").innerHTML = player.name;
	document.getElementById("balance").innerHTML = "Balance: $" + player.balance;
}

