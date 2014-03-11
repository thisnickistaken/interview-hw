var cards = 0;

function add_card(suit, value)
{
	var card_div = document.getElementById("cards");
	
	cards++;
	card_div.style.width = (79 * cards) + "px";
	card_div.innerHTML += "<div class=\"cards\" style=\"background-position: " + ((value - 1) * -79) + "px " + ((suit - 1) * -123) + "px;\"></div>";
}

function clear_cards()
{
	var card_div = document.getElementById("cards");
	
	cards = 0;
	card_div.innerHTML = "";
}

