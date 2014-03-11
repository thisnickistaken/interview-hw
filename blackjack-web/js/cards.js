var SUIT_HEARTS = 3
var SUIT_DIAMONDS = 2
var SUIT_CLUBS = 1
var SUIT_SPADES = 4

var FACE_JACK = 11
var FACE_QUEEN = 12
var FACE_KING = 13

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

