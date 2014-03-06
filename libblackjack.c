#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blackjack.h"

struct blackjack_context *create_blackjack_context()
{
	struct blackjack_context *ctx = NULL;
	int x;
	
	if(ctx = malloc(sizeof(struct blackjack_context)))
	{
		memset(ctx, 0, sizeof(struct blackjack_context));
		for(x ^= x; x < MAX_CARDS; x++)
		{
			ctx->deck[x].suit = (x / 13) + 1;
			ctx->deck[x].value = (x % 13) + 1;
		}
		initstate(time(NULL), ctx->rand_state, MAX_RAND);
	}
	
	return ctx;
}

struct player *create_player(char *name, float balance)
{
	struct player *p = NULL;
	
	if(!name)
		return NULL;
	
	if(p = malloc(sizeof(struct player)))
	{
		memset(p, 0, sizeof(struct player));
		if(!(p->name = strdup(name)))
		{
			free(p);
			return NULL;
		}
		p->balance = balance;
	}
	
	return p;
}

struct hand *create_hand(float bet)
{
	struct hand *h = NULL;
	
	if(bet == 0)
		return NULL;
	
	if(h = malloc(sizeof(struct hand)))
	{
		memset(h, 0, sizeof(struct hand));
		h->bet = bet;
	}
	
	return h;
}

int shuffle_deck(struct blackjack_context *ctx)
{
	struct player *p = NULL;
	struct hand *h = NULL;
	struct card *c = NULL, *last = NULL;
	int x, y, r;
	
	if(!ctx)
		return BJE_ARGS;
	
	setstate(ctx->rand_state);
	
	for(p = ctx->seats; p; p = p->next)
		free_hand(p);
	memset(&ctx->dealer, 0, sizeof(struct dealer));
	
	for(x ^= x; x < MAX_CARDS; x++)
		ctx->deck[x].next = NULL;
	
	for(x ^= x; x < MAX_CARDS; x++)
	{
		c = NULL;
		while(!c)
		{
			r = ((float)random() / RAND_MAX) * 52;
			if(&ctx->deck[r] != last && !ctx->deck[r].next)
			{
				if(!ctx->shuffled)
					ctx->shuffled = last = c = &ctx->deck[r];
				else
				{
					last->next = c = &ctx->deck[r];
					last = last->next;
				}
			}
		}
	}
	
	return 0;
}

int deal_game(struct blackjack_context *ctx)
{
	struct player *p = NULL;
	struct card *c = NULL;
	int x;

	if(!ctx)
		return BJE_ARGS;
	
	for(p = ctx->seats; p; p = p->next)
		if(!(c = p->hand.cards = deal_card(ctx)))
			return BJE_DEAL;
	if(!(c = ctx->dealer.cards = deal_card(ctx)))
		return BJE_DEAL;
	
	for(p = ctx->seats; p; p = p->next)
	{
		if(!(c = p->hand.cards->next = deal_card(ctx)))
			return BJE_DEAL;
		if((x = card_value_sum(p->hand.cards)) < 0)
			return BJE_COUNT;
		if(x == 21)
			p->hand.state = HAND_BLACKJACK;
	}
	if(!(c = ctx->dealer.cards->next = deal_card(ctx)))
		return BJE_DEAL;
	if((x = card_value_sum(ctx->dealer.cards)) < 0)
		return BJE_COUNT;
	if(x == 21)
		ctx->dealer.state = HAND_BLACKJACK;
	
	return 0;
}

int play_hand(struct blackjack_context *ctx, struct player *p, int action)
{
	struct hand *h = NULL;
	struct card *c = NULL;
	int x;
	
	if(!p)
		return BJE_ARGS;
	
	for(h = &p->hand; h; h = h->split)
		if(h->state == HAND_IN_PLAY)
			break;
	if(!h)
		return BJE_NOP;
	
	switch(action)
	{
		case ACT_HIT:
			for(c = h->cards; c->next; c = c->next);
			if(!(c->next = deal_card(ctx)))
				return BJE_DEAL;
			if((x = card_value_sum(h->cards)) < 0)
				return BJE_COUNT;
			if(x == 21)
				h->state = HAND_STAND;
			else if(x > 21)
				h->state = HAND_BUST;
			break;
		case ACT_STAND:
			h->state = HAND_STAND;
			break;
		case ACT_DOUBLE_DOWN:
			if(!(p->balance >= h->bet))
				return BJE_BET;
			p->balance -= h->bet;
			h->bet *= 2;
			for(c = h->cards; c->next; c = c->next);
			if(!(c->next = deal_card(ctx)))
				return BJE_DEAL;
			if((x = card_value_sum(h->cards)) < 0)
				return BJE_COUNT;
			if(x > 21)
				h->state = HAND_BUST;
			else
				h->state = HAND_STAND;
			break;
		case ACT_SPLIT:
			if(h->cards->next->next)
				return BJE_FIRST;
			if(!(p->balance >= h->bet))
				return BJE_BET;
			if(card_value(h->cards) != card_value(h->cards->next))
				return BJE_SPLIT;
			if(!(h->split = create_hand(h->bet)))
				return BJE_ALLOC;
			p->balance -= h->bet;
			for(c = h->cards; c->next; c = c->next);
			if(!(c->next = deal_card(ctx)))
				return BJE_DEAL;
			for(c = h->split->cards; c->next; c = c->next);
			if(!(c->next = deal_card(ctx)))
				return BJE_DEAL;
			break;
		case ACT_SURRENDER:
			if(h->cards->next->next)
				return BJE_FIRST;
			h->bet /= 2;
			p->balance += h->bet;
			h->state = HAND_SURRENDER;
			break;
		default:
			return BJE_ACTION;
	}
	
	return 0;
}

int play_dealer(struct blackjack_context *ctx)
{
	struct card *c = NULL;
	int x;
	
	if(!ctx)
		return BJE_ARGS;
	
	if(ctx->dealer.state != HAND_IN_PLAY)
		return BJE_NOP;
	
	if((x = card_value_sum(ctx->dealer.cards)) < 0)
		return BJE_COUNT;
	
	if(x < 17)
	{
		for(c = ctx->dealer.cards; c->next; c = c->next);
		if(!(c->next = deal_card(ctx)))
			return BJE_DEAL;
		if((x = card_value_sum(ctx->dealer.cards)) < 0)
			return BJE_COUNT;
		if(x > 21)
			ctx->dealer.state = HAND_BUST;
	}
	else
		ctx->dealer.state = HAND_STAND;
	
	return 0;
}

int card_value(struct card *c)
{
	if(!c)
		return BJE_ARGS;
	
	if(c->value > 10)
		return 10;
	else
		return c->value;
}

int card_value_sum(struct card *c)
{
	int x, aces = 0, total = 0;
	
	if(!c)
		return BJE_ARGS;
	
	for(; c; c = c->next)
	{
		if((x = card_value(c)) == 1)
		{
			aces++;
			x = 11;
		}
		total += x;
	}
	
	for(x ^= x; x < aces; x++)
		if(total > 21)
			total -= 10;
	
	return total;
}

int add_player(struct blackjack_context *ctx, char *name, float balance)
{
	struct player *p = NULL, *last = NULL;
	
	for(p = ctx->seats; p; p = p->next)
	{
		if(strcasecmp(p->name, name) == 0)
			return BJE_DUP;
		last = p;
	}
	if(!last)
	{
		if(!(ctx->seats = create_player(name, balance)))
			return BJE_ALLOC;
	}
	else
	{
		if(!(last->next = create_player(name, balance)))
			return BJE_ALLOC;
	}
	
	return 0;
}

int place_bet(struct player *p, float bet)
{
	if(!p)
		return BJE_ARGS;
	
	if(p->hand.bet != 0)
		return BJE_ACTION;
	
	if(bet > p->balance)
		return BJE_BET;
	
	p->hand.bet = bet;
	p->balance -= bet;
	
	return 0;
}

int playing(struct player *p)
{
	struct hand *h = NULL;
	
	if(!p)
		return 0;
	
	for(h = &p->hand; h; h = h->split)
		if(h->state == HAND_IN_PLAY)
			return 1;
	
	return 0;
}

int dealer_playing(struct blackjack_context *ctx)
{
	if(!ctx)
		return 0;
	
	if(ctx->dealer.state == HAND_IN_PLAY)
		return 1;
	
	return 0;
}

int resolve_game(struct blackjack_context *ctx)
{
	struct player *p = NULL;
	struct hand *h = NULL;
	int player, dealer;
	
	if(!ctx)
		return BJE_ARGS;
	
	for(p = ctx->seats; p; p = p->next)
	{
		for(h = &p->hand; h; h = h->split)
		{
			switch(h->state)
			{
				case HAND_BUST:
					h->bet = 0;
					break;
				case HAND_SURRENDER:
					h->bet /= 2;
					break;
				case HAND_BLACKJACK:
					if(ctx->dealer.state == HAND_BLACKJACK)
						h->state = HAND_PUSH;
					else
						h->bet *= (float)5/2;
					break;
				case HAND_STAND:
					switch(ctx->dealer.state)
					{
						case HAND_BLACKJACK:
							h->state = HAND_LOSS;
							h->bet = 0;
							break;
						case HAND_STAND:
							player = card_value_sum(h->cards);
							dealer = card_value_sum(ctx->dealer.cards);
							if(player < dealer)
							{
								h->state = HAND_LOSS;
								h->bet = 0;
								break;
							}
							if(player == dealer)
							{
								h->state = HAND_PUSH;
								break;
							}
						case HAND_BUST:
							h->state = HAND_WIN;
							h->bet *= 2;
							break;
						default:
							return BJE_NOP;
					}
					break;
				default:
					return BJE_NOP;
			}
			p->balance += h->bet;
		}
	}
	
	return 0;
}

struct card *deal_card(struct blackjack_context *ctx)
{
	struct card *c = NULL;
	
	if(!ctx)
		return NULL;
	
	c = ctx->shuffled;

	if(c)
	{
		ctx->shuffled = c->next;
		c->next = NULL;
	}
	
	return c;
}

void free_blackjack_context(struct blackjack_context *ctx)
{
	if(ctx)
	{
		if(ctx->seats)
			free_players(ctx->seats);
		free(ctx);
	}
	
	return;
}

void free_player(struct player *p)
{
	if(p)
	{
		if(p->name)
			free(p->name);
		free_hand(p);
		free(p);
	}
	
	return;
}

void free_players(struct player *p)
{
	struct player *tmp;

	while(p)
	{
		tmp = p->next;
		free_player(p);
		p = tmp;
	}
	
	return;
}

void free_hand(struct player *p)
{
	struct hand *h, *tmp;
	
	if(p)
	{
		for(h = p->hand.split; h; h = tmp)
		{
			tmp = h->split;
			free(h);
		}
		memset(&p->hand, 0, sizeof(struct hand));
	}
	
	return;
}

int str_to_suit(char *name)
{
	if(!name)
		return 0;
	
	if(strcasecmp(name, "hearts") == 0)
		return SUIT_HEARTS;
	if(strcasecmp(name, "diamonds") == 0)
		return SUIT_DIAMONDS;
	if(strcasecmp(name, "clubs") == 0)
		return SUIT_CLUBS;
	if(strcasecmp(name, "spades") == 0)
		return SUIT_SPADES;
	
	return 0;
}

int str_to_face(char *name)
{
	if(!name)
		return 0;
	
	if(strcasecmp(name, "one") == 0 || strcasecmp(name, "ace") == 0)
		return 1;
	if(strcasecmp(name, "two") == 0)
		return 2;
	if(strcasecmp(name, "three") == 0)
		return 3;
	if(strcasecmp(name, "four") == 0)
		return 4;
	if(strcasecmp(name, "five") == 0)
		return 5;
	if(strcasecmp(name, "six") == 0)
		return 6;
	if(strcasecmp(name, "seven") == 0)
		return 7;
	if(strcasecmp(name, "eight") == 0)
		return 8;
	if(strcasecmp(name, "nine") == 0)
		return 9;
	if(strcasecmp(name, "ten") == 0)
		return 10;
	if(strcasecmp(name, "jack") == 0)
		return 11;
	if(strcasecmp(name, "queen") == 0)
		return 12;
	if(strcasecmp(name, "king") == 0)
		return 13;
	
	return 0;
}

int str_to_value(char *name)
{
	int x;
	
	if((x = str_to_face(name)) >= 10)
		return 10;
	else
		return x;
}

int str_to_state(char *name)
{
	if(!name)
		return 0;
	
	if(strcasecmp(name, "in play") == 0)
		return HAND_IN_PLAY;
	if(strcasecmp(name, "stand") == 0)
		return HAND_STAND;
	if(strcasecmp(name, "win") == 0)
		return HAND_WIN;
	if(strcasecmp(name, "loss") == 0)
		return HAND_LOSS;
	if(strcasecmp(name, "blackjack") == 0 || strcasecmp(name, "black jack") == 0)
		return HAND_BLACKJACK;
	if(strcasecmp(name, "surrender") == 0)
		return HAND_SURRENDER;
	if(strcasecmp(name, "bust") == 0)
		return HAND_BUST;
	if(strcasecmp(name, "push") == 0)
		return HAND_PUSH;
	
	return 0;
}

int str_to_action(char *name)
{
	if(!name)
		return 0;
	
	if(strcasecmp(name, "hit") == 0 || strcasecmp(name, "h") == 0)
		return ACT_HIT;
	if(strcasecmp(name, "stand") == 0 || strcasecmp(name, "st") == 0)
		return ACT_STAND;
	if(strcasecmp(name, "double down") == 0 || strcasecmp(name, "double") == 0 || strcasecmp(name, "dd") == 0)
		return ACT_DOUBLE_DOWN;
	if(strcasecmp(name, "split") == 0 || strcasecmp(name, "sp") == 0)
		return ACT_SPLIT;
	if(strcasecmp(name, "surrender") == 0 || strcasecmp(name, "sur") == 0)
		return ACT_SURRENDER;
	
	return 0;
}

char *suit_to_str(int type)
{
	switch(type)
	{
		case SUIT_HEARTS:
			return "Hearts";
		case SUIT_DIAMONDS:
			return "Diamonds";
		case SUIT_CLUBS:
			return "Clubs";
		case SUIT_SPADES:
			return "Spades";
		default:
			return "<Invalid Suit>";
	}
}

char *face_to_str(int type)
{
	switch(type)
	{
		case 1:
			return "Ace";
		case 2:
			return "Two";
		case 3:
			return "Three";
		case 4:
			return "Four";
		case 5:
			return "Five";
		case 6:
			return "Six";
		case 7:
			return "Seven";
		case 8:
			return "Eight";
		case 9:
			return "Nine";
		case 10:
			return "Ten";
		case 11:
			return "Jack";
		case 12:
			return "Queen";
		case 13:
			return "King";
		default:
			return "<Invalid Value>";
	}
}

char *state_to_str(int type)
{
	switch(type)
	{
		case HAND_IN_PLAY:
			return "In Play";
		case HAND_STAND:
			return "Stand";
		case HAND_WIN:
			return "Win";
		case HAND_LOSS:
			return "loss";
		case HAND_BLACKJACK:
			return "Blackjack";
		case HAND_SURRENDER:
			return "Surrendered";
		case HAND_BUST:
			return "Bust";
		case HAND_PUSH:
			return "Push";
		default:
			return "<Invalid Hand State>";
	}
}

char *action_to_str(int type)
{
	switch(type)
	{
		case ACT_HIT:
			return "Hit";
		case ACT_STAND:
			return "Stand";
		case ACT_DOUBLE_DOWN:
			return "Double Down";
		case ACT_SPLIT:
			return "Split";
		case ACT_SURRENDER:
			return "Surrender";
		default:
			return "<Invalid Action>";
	}
}

char *error_to_str(int type)
{
	switch(type)
	{
		case BJE_ARGS:
			return "Invalid argument(s)";
		case BJE_FREE:
			return "Dealocator failure";
		case BJE_DEAL:
			return "Dealing failure";
		case BJE_ACTION:
			return "Invalid action";
		case BJE_NOP:
			return "Nohing to do";
		case BJE_COUNT:
			return "Counting algorithm failure";
		case BJE_BET:
			return "Insufficient funds for bet";
		case BJE_SPLIT:
			return "Cannot split cards of inequal value";
		case BJE_ALLOC:
			return "Allocator failure";
		case BJE_FIRST:
			return "Action must be performed as the first acton of a hand";
		case BJE_DUP:
			return "Duplicate entry";
		default:
			return "Unknown error";
	}
}

void print_card(struct card *c)
{
	if(c)
		printf("\t\t%s of %s\n", face_to_str(c->value), suit_to_str(c->suit));
}

void print_cards(struct card *c)
{
	while(c)
	{
		print_card(c);
		c = c->next;
	}
}

void print_hand(struct hand *h)
{
	if(h)
	{
		printf("\tWager: $%8.2f\n"
			"\tState: %s\n", h->bet, state_to_str(h->state));
		print_cards(h->cards);
	}
}

void print_hands(struct hand *h)
{
	while(h)
	{
		print_hand(h);
		h = h->split;
	}
}

void print_player(struct player *p)
{
	if(p)
	{
		printf("Player: %s\n"
			"\tBalance: $%8.2f\n", p->name, p->balance);
		print_hands(&p->hand);
	}
}

void print_players(struct player *p)
{
	while(p)
	{
		print_player(p);
		p = p->next;
	}
}

void print_dealer(struct blackjack_context *ctx)
{
	if(ctx)
	{
		printf("Dealer:\n"
			"\tState: %s\n", state_to_str(ctx->dealer.state));
		if(ctx->dealer.cards)
			print_cards(ctx->dealer.cards);
	}
}

void print_game(struct blackjack_context *ctx)
{
	if(ctx)
	{
		printf("[BlackJack]\n");
		print_dealer(ctx);
		print_players(ctx->seats);
	}
}

