var a_game = new XMLHttpRequest();
var game;
var player_name = "";
var game_name = "";
var t_cycle;
var hindex, pindex, dindex;

function play_hand(action)
{
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "game=" + game_name + "&player=" + player_name + "&action=" + action);
}

function load_game()
{
	game_name = document.getElementById("control").name.value;
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "action=load&game=" + game_name);
}

function new_game()
{
	game_name = document.getElementById("control").name.value;
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "action=new&game=" + game_name);
}

function add_player()
{
	var manage_players = document.getElementById("manage_players");
	
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "action=add&game=" + game_name + "&player=" + manage_players.name.value + "&balance=" + manage_players.amount.value);
}

function remove_player()
{
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "action=remove&game=" + game_name + "&player=" + player_name);
}

function deal_game()
{
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "action=deal&game=" + game_name);
}

function shuffle_deck()
{
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "action=shuffle&game=" + game_name);
}

function place_bet()
{
	send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, change_display, "action=bet&game=" + game_name + "&player=" + player_name + "&bet=" + document.getElementById("manage_players").amount.value);
}

function change_display()
{
	var response;
	var x,y;
	
	if(a_game.readyState == 4)
	{
		response = eval("(" + a_game.responseText + ")");
		
		if(typeof response.game !== "undefined")
		{
			game = response.game;
			
			stop_cycle();
			if(game.seats.length == 0)
			{
				document.getElementById("player").innerHTML = "To Add Players:";
				document.getElementById("balance").innerHTML = "Fill in a player name and balance"
				document.getElementById("bet").innerHTML = "then click on the";
				document.getElementById("state").innerHTML = "'Add' button.";
				clear_cards();
			}
			for(x = 0; x < game.seats.length; x++)
			{
				for(y = 0; y < game.seats[x].hands.length; y++)
				{
					if(game.seats[x].hands[y].state == "In Play")
					{
						display_player(game.seats[x]);
						display_hand(game.seats[x].hands[y]);
						break;
					}
				}
				if(y != game.seats[x].hands.length)
					break;
			}
			if(game.dealer.state != "In Play" && x == game.seats.length)
			{
				if(game.seats.length > 0)
				{
					display_player(game.seats[0]);
					if(game.seats[0].hands.length > 0)
						display_hand(game.seats[0].hands[0]);
				}
				start_player_cycle();
			}
				
			player_list_refresh(game.seats);
		}
		if(typeof response.error !== "undefined")
			alert(response.error.description);
	}
}

function display_dealer()
{
	var x;
	
	document.getElementById("player").innerHTML = "Dealer";
	document.getElementById("balance").innerHTML = "Dealer";
	clear_cards();
	for(x = 0; x < game.dealer.cards.length; x++)
		add_card(game.dealer.cards[x].suit, game.dealer.cards[x].value);
	document.getElementById("bet").innerHTML = "Dealer";
	document.getElementById("state").innerHTML = game.dealer.state;
}

function display_hand(hand)
{
	var x, bet;
	
	clear_cards();
	for(x = 0; x < hand.cards.length; x++)
		add_card(hand.cards[x].suit, hand.cards[x].value);
	bet = "Bet:";
	if(typeof hand.bet !== "undefined")
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

function find_player(name)
{
	var x;
	
	for(x = 0; x < game.seats.length; x++)
		if(game.seats[x].name == name)
			return game.seats[x];
	
	return null;
}

function select_player(name)
{
	var player;
	var manage_players = document.getElementById("manage_players");
	
	if((player = find_player(name)) == null)
	{
		alert("Player is not seated at table.");
		return;
	}
	
	stop_cycle();
	manage_players.name.value = player.name;
	manage_players.amount.value = player.balance;
	display_player(player);
	player_list_refresh(game.seats);
	for(x = 0; x < player.hands.length; x++)
		if(player.hands[x].state == "In Play")
			break;
	if(player.hands.length == x)
		start_hand_cycle();
	else
		display_hand(player.hands[x]);
}

function player_list_refresh(players)
{
	var x;
	var tmp = "<table border=1 class=\"center\">";
	
	for(x = 0; x < players.length; x++)
		tmp += "<tr class=\"select" + (players[x].name == player_name ? "ed" : "able") + "\" onclick=\"select_player('" + players[x].name + "')\"><td>" + players[x].name + "</td><td>$" + players[x].balance + "</td></tr>";
	tmp += "</table>";
	
	document.getElementById("player_list").innerHTML = tmp;
}

function stop_cycle()
{
	window.clearInterval(t_cycle);
}

function start_hand_cycle()
{
	stop_cycle();
	t_cycle = window.setInterval("cycle_hands(x++)", 1000);
}

function cycle_hands(index)
{	
	var player = find_player(player_name);
	
	index %= player.hands.length;
	display_hand(player.hands[index]);
}

function start_player_cycle()
{
	stop_cycle();
	hindex = 0;
	pindex = 0;
	dindex = 0;
	t_cycle = window.setInterval("cycle_players()", 1000);
}

function cycle_players()
{
	if(dindex == 0)
	{
		if(game.seats.length == 0)
		{
			stop_cycle();
			return;
		}
		if((hindex = ++hindex % game.seats[pindex].hands.length) == 0)
			if((pindex = ++pindex % game.seats.length) == 0)
				dindex++;
	}
	else
		dindex = 0;
	if(dindex == 0)
	{
		display_player(game.seats[pindex]);
		display_hand(game.seats[pindex].hands[hindex]);
	}
	else
		display_dealer();
}

