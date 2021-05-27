#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


#define MSGLEN 32 // need to make this a variable depending on how big messages can go e.g sending hand requires a length greatr than tha hand syze

typedef struct Card {
	char suit;
	int rank;
} Card;

typedef struct Deck {
	int deckSize;
	Card *cards;
} Deck;

typedef struct Hand {
	int handSize;
	Card *cards;
} Hand;

typedef struct Player {
	int label;
	int fd;
	int wfd;
} Player;

typedef struct Pipeline {
    int fdRead;
    int fdWrite;
} Pipeline;

typedef struct Game {
    int fd[2];
	int totalPlayers;
	int threshold;
	int cardsRemaining;
	int leadPlayer;
	Deck deck;
	Player *players;
	Card *currentRoundCards;
    Pipeline pipeline;
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
Load a deck from a file, throw error 3 if there's any problems
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

	deck.deckSize = atoi(line);
	deck.cards = malloc(deck.deckSize*sizeof(Card));
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
			if (count >= deck.deckSize) {
				exit_with_errorcode(3);
			}
		}
	}

	fclose(deckFile);
	return deck;
}

/*
Create a hand for each player 
*/

Hand create_hand(Game game, int playerNumber) {
	Hand hand;
	hand.handSize = game.deck.deckSize / game.totalPlayers;
	hand.cards = malloc(hand.handSize*sizeof(Card));

	game.cardsRemaining = hand.handSize;

	int cardIndex = 0;
	for (int i = playerNumber * hand.handSize; i < playerNumber * (hand.handSize * 2); i++) {
		hand.cards[cardIndex] = game.deck.cards[i];
	}

	return hand;
}

/*

*/

Pipeline create_pipe() {
    int fd[2];
    pipe(fd[2]);

    Pipeline pipeline = {fd[0], fd[1]};
    return pipeline;
}

/*

*/

Player create_player(int fd, int label, char* playerType) {
	Player player;

	player.label = label;

	return player;
}


/*

*/

void write_pipe(int wfd, char *message) {
	write(wfd, &message, MSGLEN);
}

/*

*/

void pipe_send_hand(int playerLabel, Game game, Hand hand) {
	char message[hand.handSize + 100]; //need to make length variable so different sized hands work

	message[0] = 'H';
	message[1] = 'A';
	message[2] = 'N';
	message[3] = 'D';

	write_pipe(game.players[playerLabel].wfd, message);
}

/*

*/

void pipe_newround(int label) {

}

/*

*/

void pipe_played(int playerNumber, Card card) {

}

/*

*/

void pipe_gameover(void) {

}

/*

*/

void read_pipe(int fd) {

}

/*

*/

int get_round_winner() {
	

	// Lead player for the next round
	return 0;
}

/*

*/

int run_game(Game game) {
    if (!fork()) {
        //dup2(game.fd[1], 1);

		execl('./2310alice', 5, 0, 11, 5);
		exit(0);
	}

	char inbuf[32]; 
	printf("plug\n");
    //dup2(game.fd[0], 0);
	read(game.fd[0], inbuf, 32); 
	printf("% s\n", inbuf);
	printf("really\n");

	// Send each player their starting hand
	for (int i = 0; i < game.totalPlayers; i++) {
		Hand hand = create_hand(game, i);

		// Create pipe for player here
		//pipe_send_hand(i, game, hand);
	}

	printf("k\n");
	exit(0);

	// The lead player is the player who starts each round
	// It starts at 0, and after that is the player who won the round
	game.leadPlayer = 0;

	// Loop for each round
	while (game.cardsRemaining > 0) {
		int currentPlayer = game.leadPlayer;

		// Loop clockwise, starting at the lead player
		for (int i = 0; i < game.totalPlayers; i++) {
			// Tell each player a round is starting

			currentPlayer += 1;
			if (currentPlayer >= game.totalPlayers) {
				currentPlayer = 0;
			}
		}

		game.leadPlayer = get_round_winner(game.cardsRemaining);
		game.cardsRemaining -= 1;
	}

	return 0;
}

/*

*/

int main(int argc, char *argv[]) {
	FILE *deckFile;

	if ((deckFile = fopen("cards.deck","r"))==NULL) {
		exit_with_errorcode(3);
	} else {
		Deck deck = load_deckfile(deckFile);
	}

	if (argc < 5) {
		exit_with_errorcode(1);
	} else {
		int threshold = atoi(argv[2]);

		//Iterate over every char in argv2 to make sure isdigit();

		
		// Check threshold
		if (threshold < 2) {
			exit_with_errorcode(2);
		} else {
			Game game;
			game.threshold = threshold;
			game.totalPlayers = argc - 3;
			game.players = malloc(game.totalPlayers * sizeof(Card));

            int fd[2];
 	        pipe(fd);
            game.fd[0] = fd[0];
            game.fd[1] = fd[1];

			// Check players are valid and create necessary pipes
			for (int i = 3; i < argc; i++) {
				if (strncmp(argv[i], "./2310alice", 11) == 0 ||
						strncmp(argv[i], "./2310bob", 9) == 0) {
					int playerLabel = i - 3;
					//Hand playerHand = create_hand(game, playerLabel);
					Player player = create_player(game.fd[1], playerLabel, argv[i]);
					game.players[playerLabel] = player;
				} else {
					exit_with_errorcode(5);
				}
				printf("%i\n", i);
			}

			// Get deck from deckfile
			FILE *deckFile;

			if((deckFile = fopen(argv[1],"r"))==NULL) {
				exit_with_errorcode(3);
			} else {
				Deck deck = load_deckfile(deckFile);
				game.deck = deck;
			}

			run_game(game);
		}
	}

	return 0;
}