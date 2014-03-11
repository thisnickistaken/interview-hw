var a_game = new XMLHttpRequest();
var game = null;
var player = "";
var game_name = "";

function play_hand(action)
{
	//send_ajax("/cgi-bin/blackjack-web.cgi", "post", a_game, <funct>, "game=" + game + "&player=" + player + "&action=" + action);
	send_ajax("tmp.json", "post", a_game, change_display, "game=" + game + "&player=" + player + "&action=" + action);
}

function change_display()
{
	if(a_game.readyState == 4)
	{
		eval(a_game.responseText);
	}
}
