#include <blackjack.h>

#include "ruby.h"

VALUE mBlackJack = Qnil;
VALUE cGame = Qnil;

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
VALUE game_play_hand(VALUE self, VALUE player, VALUE action);
VALUE game_play_dealer(VALUE self);
VALUE game_add_player(VALUE self, VALUE name, VALUE balance);
VALUE game_remove_player(VALUE self, VALUE name);
VALUE game_place_bet(VALUE self, VALUE name, VALUE bet);
VALUE game_playing(VALUE self, VALUE name);
VALUE game_dealer_playing(VALUE self);
VALUE game_resolve(VALUE self);

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
	rb_define_method(cGame, "dealer_playing", game_dealer_playing, 0);
	rb_define_method(cGame, "resolve", game_resolve, 0);
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

VALUE game_play_hand(VALUE self, VALUE player, VALUE action)
{
	struct player *p = NULL;
	struct blackjack_context *ctx = NULL;
	
	Check_Type(player, T_DATA);
	Check_Type(action, T_FIXNUM);
	
	Data_Get_Struct(player, struct player, p);
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
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
	
	return INT2NUM(playing(p));
}

VALUE game_dealer_playing(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	return INT2NUM(dealer_playing(ctx));
}

VALUE game_resolve(VALUE self)
{
	struct blackjack_context *ctx = NULL;
	
	Data_Get_Struct(rb_iv_get(self, "@ctx"), struct blackjack_context, ctx);
	
	return INT2NUM(resolve_game(ctx));
}

