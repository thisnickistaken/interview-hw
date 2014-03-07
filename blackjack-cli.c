#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blackjack.h"

#define INPUT_BUFSZ	32

struct run_info
{
	struct blackjack_context *ctx;
	struct player *retired;
	char *buf;
};

void usage(char *name);
void crash(int signal);
void cleanup(int ret, struct run_info *run);

int main(int argc, char **argv)
{
	struct run_info *run = NULL;
	struct card *c = NULL;
	struct player *p = NULL;
	char *player = NULL;
	float balance = 1000;
	int x, y, z, ret;
	
	run = malloc(sizeof(struct run_info));
	memset(run, 0, sizeof(struct run_info));
	on_exit((void(*)(int, void*))cleanup, run);
	
	for(x = 1; x < 31; x++)
		signal(x, crash);
	
	if(!(run->ctx = create_blackjack_context()))
	{
		printf("Error: Failed to create context for libblackjack\n");
		exit(1);
	}
	
	for(x = 1; x < argc; x++)
	{
		switch((argv[x])[0])
		{
			case '-':
				z ^= z;
				for(y = 1; (argv[x])[y]; y++)
				{
					switch((argv[x])[y])
					{
						case 'p':
							if((argv[x])[y + 1])
								player = argv[x] + y + 1;
							else
							{
								if(argc > x + 1)
									player = argv[++x];
								else
								{
									printf("Error: Number of players not specified.\n");
									exit(1);
								}
							}
							if(ret = add_player(&run->ctx->seats, create_player(player, balance)))
							{
								printf("Error: Failed to add player \"%s\": %s\n", player, error_to_str(ret));
								exit(1);
							}
							z++;
							break;
						case 'b':
							if((argv[x])[y + 1])
								balance = atof(argv[x] + y + 1);
							else
							{
								if(argc > x + 1)
									balance = atof(argv[++x]);
								else
								{
									printf("Error: Default balance not secified\n");
									exit(1);
								}
							}
							z++;
							break;
						case 'h':
							usage(argv[0]);
							exit(0);
						default:
							printf("Error: Unknown option: \'%c\'\n", (argv[x])[y]);
							exit(1);
					}
					if(z)
						break;
				}
				break;
			default:
				printf("Error: unexpected argument: \"%s\"\n", argv[x]);
				exit(1);
		}
	}
	
	if(!(run->buf = malloc(INPUT_BUFSZ)))
	{
		printf("Error: Failed to allocate input buffer.\n");
		exit(1);
	}
	
	while(run->ctx->seats)
	{
		if(ret = shuffle_deck(run->ctx))
		{
			printf("Error: Failed to shuffle deck: %s\n", error_to_str(ret));
			exit(1);
		}
		
		p = run->ctx->seats;
		while(p)
		{
			printf("%s, please enter your wager: ", p->name);
			if(!fgets(run->buf, INPUT_BUFSZ, stdin))
			{
				printf("Error: Failed to read input\n");
				exit(1);
			}
			z ^= z;
			switch(x = place_bet(p, atof(run->buf)))
			{
				case 0:
					p = p->next;
					break;
				case BJE_BET:
				case BJE_NEG_BET:
					z++;
				default:
					printf("%s%s\n", z ? "" : "Fatal: ", error_to_str(x));
					if(!z)
						exit(1);
					break;
			}
		}
//		system("clear");
		
		if(ret = deal_game(run->ctx))
		{
			printf("Error: Failed to deal cards: %s\n", error_to_str((ret));
			exit(1);
		}
		
		for(p = run->ctx->seats; p; p = p->next)
		{
			while(playing(p))
			{
				print_player(p);
				printf("%s, what would you like to do? ", p->name);
				if(!fgets(run->buf, INPUT_BUFSZ, stdin))
				{
					printf("Error: Failed to read input\n");
					exit(1);
				}
				x = strlen(run->buf) - 1;
				if(run->buf[x] == '\n')
					run->buf[x] = 0;
				
				z ^= z;
				switch(x = play_hand(run->ctx, p, str_to_action(run->buf)))
				{
					case 0:
						break;
					case BJE_ACTION:
					case BJE_BET:
					case BJE_FIRST:
					case BJE_SPLIT:
						z++;
					default:
						printf("%s%s\n", z ? "" : "Fatal: ", error_to_str(x));
						if(!z)
							exit(1);
						break;
				}
//				system("clear");
			}
			print_player(p);
		}
		
		while(dealer_playing(run->ctx))
		{
			print_dealer(run->ctx);
			if(play_dealer(run->ctx))
			{
				printf("Error: Failed to play dealer's hand\n");
				exit(1);
			}
		}
		print_dealer(run->ctx);
	
		if(x = resolve_game(run->ctx))
		{
			printf("Error: Failed to resolve game: %s\n", error_to_str(x));
			exit(1);
		}
		
//		system("clear");
		print_game(run->ctx);
		
		for(p = run->ctx->seats; p; p = p->next)
			if(p->balance == 0)
			{
				printf("%s has gone broke! Retiring player.\n", p->name);
				if(x = remove_player(&run->ctx->seats, p))
				{
					printf("Error: Failed to remove player: %s\n", error_to_str(x));
					exit(1);
				}
				if(x = add_player(&run->retired, p))
				{
					printf("Error: Failed to add player: %s\n", error_to_str(x));
					exit(1);
				}
			}
	}

	printf("Game has ended.\n");
	
	exit(0);
}

void usage(char *name)
{
	if(!name)
		return;
	
	printf("Usage: %s [options]\n"
		"\n"
		"\tOptions:\n"
		"\t-p\t--\tSpecify player names. Can be used multiple times for more than one player.\n"
		"\t-b\t--\tSpecify player starting balance. Specify before players to be affected. (Default: 1000.00)\n"
		"\t-h\t--\tDisplays this screen.\n", name);
	
	return;
}

void crash(int signal)
{
	switch(signal)
	{
		case SIGINT:
			printf("Interupted.\n");
			exit(1);
		case SIGILL:
			printf("Illegal instruction.\n");
			break;
		case SIGFPE:
			printf("Floating point error.\n");
			break;
		case SIGSEGV:
			printf("Segmentation fault!!!!\n");
			exit(1);
		case SIGPIPE:
			printf("Broken pipe.\n");
			break;
		case SIGSTKFLT:
			printf("Stack fault.\n");
			break;
		case SIGURG:
			printf("Urgent condition on socket.\n");
			break;
		case SIGWINCH:
			break;
		default:
			printf("Unknown signal: %u\n", signal);
	}
	
	return;
}

void cleanup(int ret, struct run_info *run)
{
	if(run->buf)
		free(run->buf);
	if(run->retired)
		free_players(run->retired);
	if(run->ctx)
		free_blackjack_context(run->ctx);
	free(run);
}

