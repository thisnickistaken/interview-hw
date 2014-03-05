#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blackjack.h"

struct run_info
{
	struct blackjack_context *ctx;
};

void usage(char *name);
void crash(int signal);
void cleanup(int ret, struct run_info *run);

int main(int argc, char **argv)
{
	struct run_info *run = NULL;
	int players = 1;
	int x, y, z;
	
	run = malloc(sizeof(struct run_info));
	memset(run, 0, sizeof(struct run_info));
	on_exit((void(*)(int, void*))cleanup, run);
	
	for(x = 1; x < 31; x++)
		signal(x, crash);
	
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
								players = atoi(argv[x] + y + 1);
							else
							{
								if(argc > x + 1)
									players = atoi(argv[++x]);
								else
								{
									printf("Error: Number of players not specified.\n");
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
	
	exit(0);
}

void usage(char *name)
{
	if(!name)
		return;
	
	printf("Usage: %s [options]\n"
		"\n"
		"\tOptions:\n"
		"\t-p\t--\tSpecify number of players\n"
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
		default:
			printf("Unknown signal: %u\n", signal);
	}
	
	return;
}

void cleanup(int ret, struct run_info *run)
{
	free_blackjack_context(run->ctx);
	free(run);
}

