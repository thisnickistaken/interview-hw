#include <stdio.h>
#include <blackjack.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	struct blackjack_context *ctx = NULL, *ctx2 = NULL, *ctx3 = NULL, *ctx4 = NULL, *ctx5 = NULL;
	struct gamestate_header *head = NULL;
	FILE *f = NULL;
	
	ctx = create_blackjack_context();
	add_player(&ctx->seats, create_player("Steven", 1000.0f));
	add_player(&ctx->seats, create_player("Moo Cow", 2000.0f));
	shuffle_deck(ctx);
	place_bet(ctx, find_player(ctx->seats, "Steven"), 1000.0f);
	place_bet(ctx, find_player(ctx->seats, "Moo Cow"), 1000.0f);
	deal_game(ctx);
	print_game(ctx);
	head = create_gamestate_image(ctx);
	f = fopen("tmp", "w");
	fwrite(head->data, head->length, 1, f);
	fclose(f);
	f = NULL;
	ctx2 = read_gamestate_image(head);
	free(head);

	play_hand(ctx2, find_player(ctx2->seats, "Steven"), ACT_HIT);
	printf("--------------------------------------------------------------\n");
	print_game(ctx2);
	head = create_gamestate_image(ctx2);
	f = fopen("tmp2", "w");
	fwrite(head->data, head->length, 1, f);
	fclose(f);
	f = NULL;
	ctx3 = read_gamestate_image(head);
	free(head);
	
	play_hand(ctx3, find_player(ctx3->seats, "Steven"), ACT_HIT);
	printf("--------------------------------------------------------------\n");
	print_game(ctx3);
	head = create_gamestate_image(ctx3);
	f = fopen("tmp3", "w");
	fwrite(head->data, head->length, 1, f);
	fclose(f);
	f = NULL;
	ctx4 = read_gamestate_image(head);
	free(head);
	
	play_hand(ctx4, find_player(ctx4->seats, "Steven"), ACT_HIT);
	printf("--------------------------------------------------------------\n");
	print_game(ctx4);
	head = create_gamestate_image(ctx4);
	f = fopen("tmp4", "w");
	fwrite(head->data, head->length, 1, f);
	fclose(f);
	f = NULL;
	ctx5 = read_gamestate_image(head);
	free(head);
	
	play_hand(ctx5, find_player(ctx5->seats, "Steven"), ACT_HIT);
	printf("--------------------------------------------------------------\n");
	print_game(ctx5);
	head = create_gamestate_image(ctx5);
	f = fopen("tmp5", "w");
	fwrite(head->data, head->length, 1, f);
	fclose(f);
	f = NULL;
	free(head);
	
	free_blackjack_context(ctx);
	free_blackjack_context(ctx2);
	free_blackjack_context(ctx3);
	free_blackjack_context(ctx4);
	free_blackjack_context(ctx5);
	
	return 0;
}

