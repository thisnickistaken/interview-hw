#include <blackjack.h>

#include "ruby.h"

VALUE mBlackJack = Qnil;
VALUE cGame = Qnil;
VALUE cPlayer = Qnil;
VALUE cHand = Qnil;
VALUE cCard = Qnil;

void Init_blackjack();

VALUE bj_str_to_suit(VALUE self, VALUE name);
VALUE bj_str_to_face(VALUE self, VALUE name);
VALUE bj_str_to_value(VALUE self, VALUE name);
VALUE bj_str_to_state(VALUE self, VALUE name);
VALUE bj_str_to_action(VALUE self, VALUE name);
VALUE bj_suit_to_str(VALUE self, VALUE type);
VALUE bj_face_to_str(VALUE self, VALUE type);
VALUE bj_state_to_str(VALUE self, VALUE type);
VALUE bj_action_to_str(VALUE self, VALUE type);
VALUE bj_error_to_str(VALUE self, VALUE type);

VALUE game_initialize(VALUE self);
VALUE game_shuffle_deck(VALUE self);
VALUE game_deal(VALUE self);
VALUE game_play_hand(VALUE self, VALUE name, VALUE action);
VALUE game_play_dealer(VALUE self);
VALUE game_add_player(VALUE self, VALUE name, VALUE balance);
VALUE game_remove_player(VALUE self, VALUE name);
VALUE game_place_bet(VALUE self, VALUE name, VALUE bet);
VALUE game_playing(VALUE self, VALUE name);
VALUE game_playing_loop(VALUE self, VALUE name);
VALUE game_dealer_playing(VALUE self);
VALUE game_dealer_playing_loop(VALUE self);
VALUE game_resolve(VALUE self);
VALUE game_players_seated(VALUE self);

VALUE game_each_player(VALUE self);
VALUE game_get_player_balance(VALUE self, VALUE name);

VALUE game_each_hand(VALUE self, VALUE name);
VALUE game_get_hand(VALUE self, VALUE name, VALUE number);

VALUE game_each_card(VALUE self, VALUE name, VALUE hand_number);
VALUE game_get_card(VALUE self, VALUE name, VALUE hand_number, VALUE card_number);

VALUE game_print_player(VALUE self, VALUE name);
VALUE game_print_dealer(VALUE self);
VALUE game_print(VALUE self);

VALUE player_initialize(VALUE self, VALUE name, VALUE balance);
VALUE player_get_name(VALUE self);
VALUE player_get_balance(VALUE self);
VALUE player_print(VALUE self);

VALUE hand_initialize(VALUE self, VALUE state, VALUE bet);
VALUE hand_get_state(VALUE self);
VALUE hand_get_bet(VALUE self);
VALUE hand_print(VALUE self);

VALUE card_initialize(VALUE self, VALUE suit, VALUE value);
VALUE card_get_suit(VALUE self);
VALUE card_get_value(VALUE self);
VALUE card_print(VALUE self);

void Init_blackjack()
{
	mBlackJack = rb_define_module("BlackJack");

	rb_define_const(mBlackJack, "SUIT_HEARTS", INT2NUM(SUIT_HEARTS));
	rb_define_const(mBlackJack, "SUIT_DIAMONDS", INT2NUM(SUIT_DIAMONDS));
	rb_define_const(mBlackJack, "SUIT_CLUBS", INT2NUM(SUIT_CLUBS));
	rb_define_const(mBlackJack, "SUIT_SPADES", INT2NUM(SUIT_SPADES));
	
	rb_define_const(mBlackJack, "FACE_JACK", INT2NUM(FACE_JACK));
	rb_define_const(mBlackJack, "FACE_QUEEN", INT2NUM(FACE_QUEEN));
	rb_define_const(mBlackJack, "FACE_KING", INT2NUM(FACE_KING));
	
	rb_define_const(mBlackJack, "ACT_HIT", INT2NUM(ACT_HIT));
	rb_define_const(mBlackJack, "ACT_STAND", INT2NUM(ACT_STAND));
	rb_define_const(mBlackJack, "ACT_DOUBLE_DOWN", INT2NUM(ACT_DOUBLE_DOWN));
	rb_define_const(mBlackJack, "ACT_SPLIT", INT2NUM(ACT_SPLIT));
	rb_define_const(mBlackJack, "ACT_SURRENDER", INT2NUM(ACT_SURRENDER));
	
	rb_define_const(mBlackJack, "HAND_IN_PLAY", INT2NUM(HAND_IN_PLAY));
	rb_define_const(mBlackJack, "HAND_STAND", INT2NUM(HAND_STAND));
	rb_define_const(mBlackJack, "HAND_WIN", INT2NUM(HAND_WIN));
	rb_define_const(mBlackJack, "HAND_LOSS", INT2NUM(HAND_LOSS));
	rb_define_const(mBlackJack, "HAND_BLACKJACK", INT2NUM(HAND_BLACKJACK));
	rb_define_const(mBlackJack, "HAND_SURRENDER", INT2NUM(HAND_SURRENDER));
	rb_define_const(mBlackJack, "HAND_BUST", INT2NUM(HAND_BUST));
	rb_define_const(mBlackJack, "HAND_PUSH", INT2NUM(HAND_PUSH));
	rb_define_const(mBlackJack, "HAND_WATCHING", INT2NUM(HAND_WATCHING));
	
	rb_define_const(mBlackJack, "BJE_ARGS", INT2NUM(BJE_ARGS));
	rb_define_const(mBlackJack, "BJE_FREE", INT2NUM(BJE_FREE));
	rb_define_const(mBlackJack, "BJE_DEAL", INT2NUM(BJE_DEAL));
	rb_define_const(mBlackJack, "BJE_ACTION", INT2NUM(BJE_ACTION));
	rb_define_const(mBlackJack, "BJE_NOP", INT2NUM(BJE_NOP));
	rb_define_const(mBlackJack, "BJE_COUNT", INT2NUM(BJE_COUNT));
	rb_define_const(mBlackJack, "BJE_BET", INT2NUM(BJE_BET));
	rb_define_const(mBlackJack, "BJE_SPLIT", INT2NUM(BJE_SPLIT));
	rb_define_const(mBlackJack, "BJE_ALLOC", INT2NUM(BJE_ALLOC));
	rb_define_const(mBlackJack, "BJE_FIRST", INT2NUM(BJE_FIRST));
	rb_define_const(mBlackJack, "BJE_DUP", INT2NUM(BJE_DUP));
	rb_define_const(mBlackJack, "BJE_NOT_FOUND", INT2NUM(BJE_NOT_FOUND));
	rb_define_const(mBlackJack, "BJE_NEG_BET", INT2NUM(BJE_NEG_BET));
	
	rb_define_method(mBlackJack, "str_to_suit", bj_str_to_suit, 1);
	rb_define_method(mBlackJack, "str_to_face", bj_str_to_face, 1);
	rb_define_method(mBlackJack, "str_to_value", bj_str_to_value, 1);
	rb_define_method(mBlackJack, "str_to_state", bj_str_to_state, 1);
	rb_define_method(mBlackJack, "str_to_action", bj_str_to_action, 1);
	rb_define_method(mBlackJack, "suit_to_str", bj_suit_to_str, 1);
	rb_define_method(mBlackJack, "face_to_str", bj_face_to_str, 1);
	rb_define_method(mBlackJack, "state_to_str", bj_state_to_str, 1);
	rb_define_method(mBlackJack, "action_to_str", bj_action_to_str, 1);
	rb_define_method(mBlackJack, "error_to_str", bj_error_to_str, 1);
	
	cGame = rb_define_class_under(mBlackJack, "Game", rb_cObject);
	
	rb_define_private_method(cGame, "initialize", game_initialize, 0);
	rb_define_method(cGame, "shuffle_deck", game_shuffle_deck, 0);
	rb_define_method(cGame, "deal", game_deal, 0);
	rb_define_method(cGame, "play_hand", game_play_hand, 2);
	rb_define_method(cGame, "play_dealer", game_play_dealer, 0);
	rb_define_method(cGame, "add_player", game_add_player, 2);
	rb_define_method(cGame, "remove_player", game_remove_player, 1);
	rb_define_method(cGame, "place_bet", game_place_bet, 2);
	rb_define_method(cGame, "playing", game_playing, 1);
	rb_define_method(cGame, "playing_loop", game_playing_loop, 1);
	rb_define_method(cGame, "dealer_playing", game_dealer_playing, 0);
	rb_define_method(cGame, "dealer_playing_loop", game_dealer_playing_loop, 0);
	rb_define_method(cGame, "resolve", game_resolve, 0);
	rb_define_method(cGame, "players_seated", game_players_seated, 0);
	
	rb_define_method(cGame, "each_player", game_each_player, 0);
	rb_define_method(cGame, "get_player_balance", game_each_player, 1);
	
	rb_define_method(cGame, "each_hand", game_each_hand, 1);
	rb_define_method(cGame, "get_hand", game_get_hand, 2);
	
	rb_define_method(cGame, "each_card", game_each_card, 2);
	rb_define_method(cGame, "get_card", game_get_card, 3);
	
	rb_define_method(cGame, "print_player", game_print_player, 1);
	rb_define_method(cGame, "print_dealer", game_print_dealer, 0);
	rb_define_method(cGame, "print", game_print, 0);
	
	cPlayer = rb_define_class_under(mBlackJack, "Player", rb_cObject);
	
	rb_define_private_method(cPlayer, "initialize", player_initialize, 2);
	rb_define_method(cPlayer, "get_name", player_get_name, 0);
	rb_define_method(cPlayer, "get_balance", player_get_balance, 0);
	rb_define_method(cPlayer, "print", player_print, 0);
	
	cHand = rb_define_class_under(mBlackJack, "Hand", rb_cObject);
	
	rb_define_private_method(cHand, "initialize", hand_initialize, 2);
	rb_define_method(cHand, "get_state", hand_get_state, 0);
	rb_define_method(cHand, "get_bet", hand_get_bet, 0);
	rb_define_method(cHand, "print", hand_print, 0);
	
	cCard = rb_define_class_under(mBlackJack, "Card", rb_cObject);
	
	rb_define_private_method(cCard, "initialize", card_initialize, 2);
	rb_define_method(cCard, "get_suit", card_get_suit, 0);
	rb_define_method(cCard, "get_value", card_get_value, 0);
	rb_define_method(cCard, "get_print", card_print, 0);
}

VALUE bj_str_to_suit(VALUE self, VALUE name)
{
	Check_Type(name, T_STRING);
	
	return INT2NUM(str_to_suit(StringValueCStr(name)));
}

VALUE bj_str_to_face(VALUE self, VALUE name)
{
	Check_Type(name, T_STRING);
	
	return INT2NUM(str_to_face(StringValueCStr(name)));
}

VALUE bj_str_to_value(VALUE self, VALUE name)
{
	Check_Type(name, T_STRING);
	
	return INT2NUM(str_to_value(StringValueCStr(name)));
}

VALUE bj_str_to_state(VALUE self, VALUE name)
{
	Check_Type(name, T_STRING);
	
	return INT2NUM(str_to_state(StringValueCStr(name)));
}

VALUE bj_str_to_action(VALUE self, VALUE name)
{
	Check_Type(name, T_STRING);
	
	return INT2NUM(str_to_action(StringValueCStr(name)));
}

VALUE bj_suit_to_str(VALUE self, VALUE type)
{
	Check_Type(type, T_FIXNUM);
	
	return rb_str_new2(suit_to_str(NUM2INT(type)));
}

VALUE bj_face_to_str(VALUE self, VALUE type)
{
	Check_Type(type, T_FIXNUM);
	
	return rb_str_new2(face_to_str(NUM2INT(type)));
}

VALUE bj_state_to_str(VALUE self, VALUE type)
{
	Check_Type(type, T_FIXNUM);
	
	return rb_str_new2(state_to_str(NUM2INT(type)));
}

VALUE bj_action_to_str(VALUE self, VALUE type)
{
	Check_Type(type, T_FIXNUM);
	
	return rb_str_new2(action_to_str(NUM2INT(type)));
}

VALUE bj_error_to_str(VALUE self, VALUE type)
{
	Check_Type(type, T_FIXNUM);
	
	return rb_str_new2(error_to_str(NUM2INT(type)));
}

VALUE game_initialize(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	if(!(ctx = create_blackjack_context()))
		return Qnil;
	
	rb_iv_set(self, "@ctx", Data_Wrap_Struct(rb_cObject, NULL, free_blackjack_context, ctx));
	
	return self;
}

VALUE game_shuffle_deck(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	return INT2NUM(shuffle_deck(ctx));
}

VALUE game_deal(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	return INT2NUM(deal_game(ctx));
}

VALUE game_play_hand(VALUE self, VALUE name, VALUE action)
{
	struct player *p = NULL;
	struct blackjack_context *ctx = NULL;
	
	Check_Type(name, T_STRING);
	Check_Type(action, T_FIXNUM);
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return INT2NUM(BJE_NOT_FOUND);
	
	return INT2NUM(play_hand(ctx, p, NUM2INT(action)));
}

VALUE game_play_dealer(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	return INT2NUM(play_dealer(ctx));
}

VALUE game_add_player(VALUE self, VALUE name, VALUE balance)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	
	Check_Type(name, T_STRING);
	Check_Type(balance, T_FLOAT);
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = create_player(StringValueCStr(name), NUM2DBL(balance))))
		return INT2NUM(BJE_ALLOC);
	
	return INT2NUM(add_player(&ctx->seats, p));
}

VALUE game_remove_player(VALUE self, VALUE name)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	int ret;
	
	Check_Type(name, T_STRING);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return INT2NUM(BJE_NOT_FOUND);
	
	if(!(ret = remove_player(&ctx->seats, p)))
		free_player(p);
	
	return INT2NUM(ret);	
}

VALUE game_place_bet(VALUE self, VALUE name, VALUE bet)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	
	Check_Type(name, T_STRING);
	Check_Type(bet, T_FLOAT);
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return INT2NUM(BJE_NOT_FOUND);
	
	return INT2NUM(place_bet(p, NUM2DBL(bet)));
}

VALUE game_playing(VALUE self, VALUE name)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	
	Check_Type(name, T_STRING);
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return INT2NUM(BJE_NOT_FOUND);
	
	if(playing(p))
		return Qtrue;
	else
		return Qfalse;
}

VALUE game_playing_loop(VALUE self, VALUE name)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return INT2NUM(BJE_NOT_FOUND);
	
	while(playing(p))
		rb_yield(Qnil);
	
	return INT2NUM(0);
}

VALUE game_dealer_playing(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(dealer_playing(ctx))
		return Qtrue;
	else
		return Qfalse;
}

VALUE game_dealer_playing_loop(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	while(dealer_playing(ctx))
		rb_yield(Qnil);
	
	return INT2NUM(0);
}

VALUE game_resolve(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	return INT2NUM(resolve_game(ctx));
}

VALUE game_players_seated(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(ctx->seats)
		return Qtrue;
	return Qfalse;
}

VALUE game_each_player(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	char buf[128];
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	for(p = ctx->seats; p; p = p->next)
	{
		if(snprintf(buf, 128, "Player.new(\"%s\", %f)", p->name, p->balance) >= 128)
			return INT2NUM(BJE_ALLOC);
		rb_yield(rb_eval_string(buf));
	}
	
	return INT2NUM(0);	
}

VALUE game_get_player(VALUE self, VALUE name)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	char buf[128];
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return Qnil;
	
	if(snprintf(buf, 128, "Player.new(\"%s\", %f)", p->name, p->balance) >= 128)
		return Qnil;
	
	return rb_eval_string(buf);
}

VALUE game_each_hand(VALUE self, VALUE name)
{	
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	struct hand *h = NULL;
	char buf[32];
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return INT2NUM(BJE_NOT_FOUND);
	
	for(h = &p->hand; h; h = h->split)
	{
		if(snprintf(buf, 32, "Hand.new(%d, %f)", h->state, h->bet) >= 32)
			return INT2NUM(BJE_ALLOC);
		rb_yield(rb_eval_string(buf));
	}
	
	return INT2NUM(0);
}

VALUE game_get_hand(VALUE self, VALUE name, VALUE number)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	struct hand *h = NULL;
	char buf[32];
	int x = 0;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return Qnil;
	
	for(h = &p->hand; h && x++ < NUM2INT(number); h = h->split);
	
	if(h)
	{
		if(snprintf(buf, 32, "Hand.new(%d, %f)", h->state, h->bet) >= 32)
			return Qnil;
		return rb_eval_string(buf);
	}
	
	return Qnil;	
}

VALUE game_each_card(VALUE self, VALUE name, VALUE hand_number)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	struct hand *h = NULL;
	struct card *c = NULL;
	char buf[32];
	int x = 0;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return INT2NUM(BJE_NOT_FOUND);
	
	for(h = &p->hand; h && x++ < NUM2INT(hand_number); h = h->split);
	
	if(!h)
		return INT2NUM(BJE_NOT_FOUND);
	
	for(c = h->cards; c; c = c->next)
	{
		if(snprintf(buf, 32, "Card.new(%d, %d)", c->suit, c->value) >= 32)
			return INT2NUM(BJE_ALLOC);
		rb_yield(rb_eval_string(buf));
	}
	
	return INT2NUM(0);
}

VALUE game_get_card(VALUE self, VALUE name, VALUE hand_number, VALUE card_number)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	struct hand *h = NULL;
	struct card *c = NULL;
	char buf[32];
	int x = 0, y = 0;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return Qnil;
	
	for(h = &p->hand; h && x++ < NUM2INT(hand_number); h = h->split);
	
	if(!h)
		return Qnil;
	
	for(c = h->cards; c && y++ < NUM2INT(card_number); c = c->next);
	
	if(c)
	{
		if(snprintf(buf, 32, "Card.new(%d, %d)", c->suit, c->value) >= 32)
			return INT2NUM(BJE_ALLOC);
		return rb_eval_string(buf);
	}
	
	return Qnil;
}

VALUE game_print_player(VALUE self, VALUE name)
{
	struct blackjack_context *ctx = NULL;
	struct player *p = NULL;
	
	Check_Type(name, T_STRING);
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	if(!(p = find_player(ctx->seats, StringValueCStr(name))))
		return INT2NUM(BJE_NOT_FOUND);
	
	print_player(p);
}

VALUE game_print_dealer(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	print_dealer(ctx);
}

VALUE game_print(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	print_game(ctx);
}

VALUE player_initialize(VALUE self, VALUE name, VALUE balance)
{
	rb_iv_set(self, "@name", name);
	rb_iv_set(self, "@balance", balance);
	
	return self;
}

VALUE player_get_name(VALUE self)
{
	return rb_iv_get(self, "@name");
}

VALUE player_get_balance(VALUE self)
{
	return rb_iv_get(self, "@balance");
}

VALUE player_print(VALUE self)
{
	struct player p;
	VALUE name = Qnil;
	
	memset(&p, 0, sizeof(struct player));
	name = rb_iv_get(self, "@name");
	p.name = StringValueCStr(name);
	p.balance = NUM2DBL(rb_iv_get(self, "@balance"));
	print_player(&p);
	
	return Qnil;
}

VALUE hand_initialize(VALUE self, VALUE state, VALUE bet)
{
	rb_iv_set(self, "@state", state);
	rb_iv_set(self, "@bet", bet);
	
	return self;
}

VALUE hand_get_state(VALUE self)
{
	return rb_iv_get(self, "@state");
}

VALUE hand_get_bet(VALUE self)
{
	return rb_iv_get(self, "@bet");
}

VALUE hand_print(VALUE self)
{
	struct hand h;
	
	memset(&h, 0, sizeof(struct hand));
	h.state = NUM2INT(rb_iv_get(self, "@state"));
	h.bet = NUM2DBL(rb_iv_get(self, "@bet"));
	print_hand(&h);
	
	return Qnil;
}

VALUE card_initialize(VALUE self, VALUE suit, VALUE value)
{
	rb_iv_set(self, "@suit", suit);
	rb_iv_set(self, "@value", value);
	
	return self;
}

VALUE card_get_suit(VALUE self)
{
	return rb_iv_get(self, "@suit");
}

VALUE card_get_value(VALUE self)
{
	return rb_iv_get(self, "@value");
}

VALUE card_print(VALUE self)
{
	struct card c;
	
	memset(&c, 0, sizeof(struct card));
	c.suit = NUM2INT(rb_iv_get(self, "@suit"));
	c.value = NUM2INT(rb_iv_get(self, "@value"));
	print_card(&c);
	
	return Qnil;
}

