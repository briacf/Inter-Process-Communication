#include <stdio.h>
#include <fcntl.h>

typedef struct Card {
	char suit;
	int rank;
} Card;

typedef struct Hand {
	Card *cards;
} Hand;

typedef struct Move {

} Move;

typedef struct Round {
	int leadPlayer;
	// Number of d suit cards played
	int dCardsPlayed;
	// The suit of the first card played is the 'lead suit'
	char leadSuit;
	Card *cardsPlayed;
} Round;

typedef struct Player {
	int label;
} Player;

/*
Exit with a certain errorcode and message
*/
void exit_with_errorcode(int error) {
	switch(error) {
		case 1:
			fprintf(stderr, "Usage: player players myid threshold handsize\n");
			break;
		case 2:
			fprintf(stderr, "Invalid players\n");
			break;
		case 3:
			fprintf(stderr, "Invalid position\n");
			break;
		case 4:
			fprintf(stderr, "Invalid theshold\n");
			break;
		case 5:
			fprintf(stderr, "Invalid hand size\n");
			break;
		case 6:
			fprintf(stderr, "Invalid message\n");
			break;
		case 7:
			fprintf(stderr, "EOF\n");
			break;
		default:
			exit(error);
	}
}

Round get_round_winner(Round currentRound) {

	return currentRound;
}

int get_score() {

	return 0;
}

Move make_move() {
	Move move;

	return move;
}

int run_game(Hand hand) {
	return 0;
}

int main() {
	int fd[2];
 	pipe(fd);
	write(fd[1], "hey pal", 32);

	return 0;
}