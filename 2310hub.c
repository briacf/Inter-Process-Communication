#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <ctype.h>

typedef struct Card {
	char suit;
	int rank;
} Card;

typedef struct Deck {
	int size;
	Card *cards;
} Deck;

typedef struct Pipeline {
	// Use for reading from pipe
	int rfd;
	// Use to write to pipe
	int wfd;
	// Saved stdout so things can be printed to the screen again
	int stdoutValue;
} Pipeline;

typedef struct Hand {
	int size;
	Card *cards;
} Hand;

typedef struct Player {
	int label;
	Hand hand;
	Pipeline pipeline;
} Player;

typedef struct Game {
	int totalPlayers;
	int threshold;
	int leadPlayer;

	Deck deck;
    Player *players;
} Game;


/*
Exit with a certain errorcode and message
*/

void exit_with_errorcode(int error) {
	switch(error) {
		case 1:
			fprintf(stderr, "Usage: 2310hub deck threshold player0 {player1}\n");
			break;
		case 2:
			fprintf(stderr, "Invalid theshold\n");
			break;
		case 3:
			fprintf(stderr, "Deck error\n");
			break;
		case 4:
			fprintf(stderr, "Not enough cards\n");
			break;
		case 5:
			fprintf(stderr, "Player error\n");
			break;
		case 6:
			fprintf(stderr, "Player EOF\n");
			break;
		case 7:
			fprintf(stderr, "Invalid message\n");
			break;
		case 8:
			fprintf(stderr, "Invalid card choice\n");
			break;
		case 9:
			fprintf(stderr, "Ended due to signal\n");
			break;
		default:
			exit(error);
	}

	exit(error);
}


/*
Check if a char array contains a number
Return 1 if it is a number, otherwise return 0
*/

int is_number(char number[]) {
	int i, digits = 0;
	for (; i < strlen(number); i++) {
		if (!isdigit(number[i])) {
			return 0;
		}
	}

	return 1;
}


/*
Exits with error 4 if deck can't be loaded
*/

Deck load_deckfile(FILE *deckFile) {
	Deck deck;
	char c;
	int index = 0;
	char line[5];

	while((c = fgetc(deckFile)) != '\n') {
		line[index] = c;
		index += 1;
	}

	deck.size = atoi(line);
	deck.cards = malloc(deck.size*sizeof(Card));
	int count = 0;
	index = 0;
	Card card;

	while((c = fgetc(deckFile)) != EOF) {
		if (index == 0) {
			if (c == 'S' || c == 'C' || c == 'D' || c == 'H') {
				card.suit = c;
				index += 1;
			} else {
				exit_with_errorcode(3);
			}
		} else if (index == 1) {
			if (isxdigit(c)) {
				card.rank = c;
				index += 1;
			} else {
				exit_with_errorcode(3);
			}
		} else {
			deck.cards[count] = card;
			index = 0;
		}

		if (c == '\n' || c == EOF) {
			count += 1;

			// Too many cards in deck
			if (count >= deck.size) {
				exit_with_errorcode(3);
			}
		}
	}

	fclose(deckFile);
	return deck;
}

/* 
Create a hand struct
*/

Hand create_hand(int label, int totalPlayers, Deck deck) {
	Hand hand;
	hand.size = deck.size / totalPlayers;
	hand.cards = malloc(hand.size*sizeof(Card));

	for (int i = label * hand.size; i < label * (hand.size * 2); i++) {
		hand.cards[i - (label * hand.size)] = deck.cards[i];
	}

	return hand;
}

/*

*/

Pipeline create_pipe() {
	int fd[2];
	pipe(fd);

	int stdoutValue = dup(1);

	Pipeline pipeline = {fd[0], fd[1], stdoutValue};
	return pipeline;
}

/*
Create a player struct

Exits with error 5 if a player can't be created
*/

Player create_player(int label, char *filename, Game game) {
	// Check that the player is valid (Alice or Bob)
	char *playerFile;
	if (strncmp("./2310alice", filename, 11) == 0) {
		playerFile = "2310alice";
	} else if (strncmp("./2310bob", filename, 9) == 0) {
		playerFile = "2310bob";
	} else {
		exit_with_errorcode(5);
	}

	// Create the player struct
	Player player;
	player.label = label;
	player.hand = create_hand(label, game.totalPlayers, game.deck);
	player.pipeline = create_pipe();

	// Fork and execute the player program
	if (!fork()) {
		printf("Created player %d\n", label);
		//dup2(game.pipeline.rfd, 0);
		dup2(player.pipeline.wfd, 1);

		close(player.pipeline.rfd);
		close(player.pipeline.wfd);

    	// Convert integers to char arrays to be passed into player argv
    	char totalPlayersChar[12];
    	char labelChar[12];
    	char thresholdChar[12];
    	char handSizeChar[12];

    	sprintf(totalPlayersChar, "%d", game.totalPlayers);
    	sprintf(labelChar, "%d", label);
    	sprintf(thresholdChar, "%d", game.threshold);
    	sprintf(handSizeChar, "%d", player.hand.size);

        execl(playerFile, totalPlayersChar, labelChar, thresholdChar, handSizeChar, NULL);

        //Child finished, kill this fork
		exit(0);
	}

	// Return the player struct
	return player;
}


/*

*/

void run_game(Game game) {
    dup2(game.players[0].pipeline.wfd, 1);
    printf("!ya");

    dup2(game.players[0].pipeline.rfd, 0);
    char inbuf[32]; 
	read(game.players[0].pipeline.rfd, inbuf, 32);

	dup2(game.players[0].pipeline.stdoutValue, 1);
	printf("Received: %s\n", inbuf);
}

/*

*/
  
int main(int argc, char *argv[]) {
	//Check the correct number of arguments is given
	if (argc < 5) {
		exit_with_errorcode(1);
	} else {
		// Load cards from deck and check there's any issues with the deckfile
		FILE *deckFile;

		if ((deckFile = fopen(argv[1],"r"))==NULL) {
			exit_with_errorcode(3);
		} else {
			Deck deck = load_deckfile(deckFile);

			// Check there are enough cards in the deck
			int totalPlayers = argc - 3;
			if (deck.size < totalPlayers) {
				exit_with_errorcode(4);
			} else {
				int threshold = atoi(argv[2]);

				// Check threshold is a number above 1
				if (!is_number(argv[2]) || threshold < 2) {
					exit_with_errorcode(2);
				} else {
					Game game;
					game.totalPlayers = totalPlayers;
					game.threshold = threshold;
					game.deck = deck;
					game.leadPlayer = 0;
					game.players = malloc(game.totalPlayers * sizeof(Player));

					// Check players are valid and create necessary pipes
					int i = 3;
					for (; i < argc; i++) {
						int label = i - 3;

						Player player = create_player(label, argv[i], game);
						game.players[label] = player;
					}

					//Start the main game loop
					run_game(game);
				}
			}
		}
	}

    return 0; 
} 
