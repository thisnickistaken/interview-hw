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
#define HAND_STAND	1
#define HAND_WIN	2
#define HAND_LOSS	3
#define HAND_BLACKJACK	4
#define HAND_SURRENDER	5
#define HAND_BUST	6
#define HAND_PUSH	7
#define HAND_WATCHING	8

#define MAX_CARDS	52
#define MAX_RAND	64

#define BJE_ARGS	-1
#define BJE_FREE	-2
#define BJE_DEAL	-3
#define BJE_ACTION	-4
#define BJE_NOP		-5
#define BJE_COUNT	-6
#define BJE_BET		-7
#define BJE_SPLIT	-8
#define BJE_ALLOC	-9
#define BJE_FIRST	-10
#define BJE_DUP		-11
#define BJE_NOT_FOUND	-12
#define BJE_NEG_BET	-13
#define BJE_LOCKED	-14

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
	struct card deck[MAX_CARDS];
	struct card *shuffled;
	char rand_state[MAX_RAND];
	struct dealer
	{
		int state;
		struct card *cards;
	} dealer;
	struct player *seats;
};

struct blackjack_context *create_blackjack_context();
struct player *create_player(char *name, float balance);
struct hand *create_hand(float bet);

int shuffle_deck(struct blackjack_context *ctx);
int deal_game(struct blackjack_context *ctx);
int play_hand(struct blackjack_context *ctx, struct player *p, int action);
int play_dealer(struct blackjack_context *ctx);
int card_value(struct card *c);
int card_value_sum(struct card *c);
int add_player(struct player **plist, struct player *p);
int remove_player(struct player **plist, struct player *p);
int place_bet(struct player *p, float bet);
int playing(struct player *p);
int dealer_playing(struct blackjack_context *ctx);
int resolve_game(struct blackjack_context *ctx);

struct card *deal_card(struct blackjack_context *ctx);

struct player *find_player(struct player *plist, char *name);

void free_blackjack_context(struct blackjack_context *ctx);
void free_player(struct player *p);
void free_players(struct player *p);
void free_hand(struct player *p);

int str_to_suit(char *name);
int str_to_face(char *name);
int str_to_value(char *name);
int str_to_state(char *name);
int str_to_action(char *name);
char *suit_to_str(int type);
char *face_to_str(int type);
char *state_to_str(int type);
char *action_to_str(int type);
char *error_to_str(int type);

void print_card(struct card *c);
void print_cards(struct card *c);
void print_hand(struct hand *h);
void print_hands(struct hand *h);
void print_player(struct player *p);
void print_players(struct player *p);
void print_dealer(struct blackjack_context *ctx);
void print_game(struct blackjack_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
