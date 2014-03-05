#ifndef _BLACKJACK_INCLUDE
#define _BLACKJACK_INCLUDE	1

#define SUIT_HEARTS	1
#define SUIT_DIAMONDS	2
#define SUIT_CLUBS	3
#define SUIT_SPADES	4

#define FACE_JACK	11
#define FACE_QUEEN	12
#define FACE_KING	13

#define ACT_HIT		1
#define ACT_STAND	2
#define ACT_DOUBLE_DOWN	3
#define ACT_SPLIT	4
#define ACT_SURRENDER	5

#define HAND_IN_PLAY	0
#define HAND_WIN	1
#define HAND_LOSS	2

#define MAX_CARDS	52
#define MAX_RAND	64

#define BJE_ARGS	1
#define BJE_FREE	2
#define BJE_DEAL	3
#define BJE_ACTION	4

#ifdef __cplusplus
extern "C"
{
#endif

struct card
{
	unsigned int suit;
	unsigned int value;
	struct card *next;
};

struct hand
{
	int state;
	float bet;
	struct card *cards;
	struct hand *split;
};

struct player
{
	char *name;
	float balance;
	struct hand hand;
	struct player *next;
};

struct blackjack_context
{
	struct card deck[CARDS_MAX];
	struct card *shuffled;
	char rand_state[MAX_RAND];
	struct dealer
	{
		int state;
		struct cards *cards;
	} dealer;
	struct player *seats;
};

struct blackjack_context *create_blackjack_context();
struct player *create_player(char *name, float balance);

int shuffle_deck(struct blackjack_context *ctx);
int deal_game(struct blackjack_context *ctx);
int play_hand(struct player *p, int action);
int play_dealer(struct blackjack_context *ctx);

struct card *deal_card(struct blackjack_context *ctx);

int free_blackjack_context(struct *blackjack_context);
int free_player(struct player *p);
int free_hand(struct player *p);

#ifdef __cplusplus
}
#endif

#endif
