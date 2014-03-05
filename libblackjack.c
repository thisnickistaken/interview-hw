#include <time.h>
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
		p->name = name;
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
			r = (random() / RAND_MAX) * 52;
			if(&ctx->deck[r] != last && !ctx->deck[r].next)
			{
				if(!ctx->shuffled)
					ctx->shuffled = last = &ctx->deck[r];
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
		{
			p->hand.state = HAND_BLACKJACK;
//			p->hand.bet = (p->hand.bet * 3) / 2.0f;
//			p->balance += p->hand.bet;
		}
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
			if(x > 21)
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
			if(!(p->balance >= h->bet))
				return BJE_BET;
			if(h->cards->next->next)
				return BJE_FIRST;
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
		if(ctx->dealer.cards)
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

