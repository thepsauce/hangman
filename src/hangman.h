#include <errno.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <curses.h>

#define ARRLEN(a) (sizeof(a)/sizeof*(a))

typedef struct program_args {
	int tries;
	const char *word;
	const char *dictionary;
} ProgramArgs;

extern ProgramArgs program_args;

int parse_args(int argc,  char **argv);

#define HANGMAN_LOSE 0x01
#define HANGMAN_WIN 0x02
#define HANGMAN_GAME_OVER 0x03

#define HANGMAN_LETTER_USED 0x01
#define HANGMAN_LETTER_GUESSED 0x02

typedef struct hangman {
	uint64_t flags;
	char word[256];
	uint8_t letterStates[26 + 6];
	struct {
		int total;
		int correct;
		int allowed;
		int needed;
	} guesses;
} Hangman;

int hangman_init(Hangman *hangman, int allowedGuesses, const char *word);
int hangman_guess(Hangman *hangman, char ch);
void hangman_printstatus(Hangman *hangman);
void hangman_printimage(Hangman *hangman);
void hangman_printword(Hangman *hangman);
void hangman_printjumble(Hangman *hangman);
