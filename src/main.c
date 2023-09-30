#include "hangman.h"

char *get_random_word(void)
{
	static char word[256];
	size_t nWord = 0;
	FILE *fp;
	uint32_t r;
	uint32_t lines = 1;
	int c;

	fp = fopen(program_args.dictionary, "r");
	if (fp == NULL) {
		endwin();
		fprintf(stderr, "Unable to open 'words_alpha.txt': %s\n",
				strerror(errno));
		exit(-1);
	}
	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')
			lines++;
	fseek(fp, 0, SEEK_SET);
	for (r = ((rand() << 16) | rand()) % lines; r != 0; r--) {
		if ((c = fgetc(fp)) == EOF)
			break;
		nWord = 0;
		while (c != '\n' && nWord < sizeof(word)) {
			word[nWord++] = c;
			c = fgetc(fp);
		}
		if (nWord == sizeof(word)) {
			while (c != EOF && c != '\n')
				c = fgetc(fp);
			continue;
		}
		word[nWord] = 0;
	}

	fclose(fp);
	return word;
}

int main(int argc, char **argv)
{
	Hangman hangman;

	srand(time(NULL));

	parse_args(argc - 1, argv + 1);

	if (program_args.tries <= 0)
		program_args.tries = 1;
	if (program_args.word == NULL)
		program_args.word = get_random_word();
	if (program_args.word == NULL) {
		fprintf(stderr, "Unable to get random word\n");
		return -1;
	}

	initscr();

	if (hangman_init(&hangman, program_args.tries,
				program_args.word) < 0) {
		endwin();
		fprintf(stderr, "Invalid word! A word consists of letters, white space and a maximum of %zu characters\n",
				(size_t) sizeof(hangman.word));
		return -1;
	}

	while (1) {
		move(0, 0);
		const int c = getch();
		if (!(hangman.flags & HANGMAN_GAME_OVER))
			hangman_guess(&hangman, c);
		else {
			if (c == 'q')
				break;
			switch (c) {
			case 'r': {
				char word[sizeof(hangman.word)];

				clear();
				strcpy(word, hangman.word);
				hangman_init(&hangman, program_args.tries,
						word);
				break;
			}
			case 'n':
				clear();
				hangman_init(&hangman, program_args.tries,
						get_random_word());
				break;
			}
		}
	}

	endwin();
	return 0;
}
