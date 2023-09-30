#include "hangman.h"

int hangman_init(Hangman *hangman, int allowedGuesses, const char *word)
{
	memset(hangman, 0, sizeof(*hangman));
	if (strlen(word) >= sizeof(hangman->word))
		return -1;
	strcpy(hangman->word, word);
	for (const char *w = hangman->word; *w != '\0'; w++) {
		if (isspace(*w))
			continue;
		if (!isalpha(*w))
			return -1;
		const int ind = tolower(*w) - 'a';
		if (hangman->letterStates[ind] == 0) {
			hangman->guesses.needed++;
			hangman->letterStates[ind] = HANGMAN_LETTER_USED;
		}
	}
	hangman->guesses.allowed = allowedGuesses + hangman->guesses.needed;
	hangman_printword(hangman);
	hangman_printjumble(hangman);
	return 0;
}

int hangman_guess(Hangman *hangman, char ch)
{
	if (!isalpha(ch))
		return -1;
	const int ind = tolower(ch) - 'a';
	hangman->guesses.total++;
	if (!(hangman->letterStates[ind] & HANGMAN_LETTER_GUESSED) &&
			(hangman->letterStates[ind] & HANGMAN_LETTER_USED)) {
		hangman->letterStates[ind] |= HANGMAN_LETTER_GUESSED;
		hangman->guesses.correct++;
		hangman_printword(hangman);
	} else {
		hangman->letterStates[ind] |= HANGMAN_LETTER_GUESSED;
		hangman_printimage(hangman);
		hangman_printjumble(hangman);
	}
	if (hangman->guesses.correct == hangman->guesses.needed)
		hangman->flags |= HANGMAN_WIN;
	else if (hangman->guesses.total - hangman->guesses.correct ==
			hangman->guesses.allowed - hangman->guesses.needed)
		hangman->flags |= HANGMAN_LOSE;
	hangman_printstatus(hangman);
	return 0;
}

void hangman_printimage(Hangman *hangman)
{
	const char *art =
		"      _____        \n"
		"     | /   |       \n"
		"     |/    |       \n"
		"     |    _-_      \n"
		"     |   (x x)     \n"
		"     |    \\ /      \n"
		"     |   --|--     \n"
		"     |  /  |  \\    \n"
		"     |  |  |  |    \n"
		"     |     |       \n"
		"     |    / \\      \n"
		"     |    | |      \n"
		"     |             \n"
		"    /-\\            \n"
		"   /   \\           \n";
	const int percentile = 100 * (hangman->guesses.total -
			hangman->guesses.correct) / (hangman->guesses.allowed -
				hangman->guesses.needed);
	switch (percentile) {
	case 100:
		mvaddstr(1, 0, art);
		break;
	/* fall through */
	case 81 ... 90:
		mvaddch(3, 11, '|');
	/* fall through */
	case 71 ... 80:
		mvaddch(2, 11, '|');
	/* fall through */
	case 51 ... 70:
		for (int i = 0; i < 5; i++) {
			if ((percentile - 51) * 5 / (70 - 51) == i)
				break;
			mvaddch(1, 6 + i, '_');
		}
	/* fall through */
	case 21 ... 50:
		for (int i = 0; i < 12; i++) {
			if ((percentile - 21) * 12 / (50 - 21) == i)
				break;
			mvaddch(13 - i, 5, '|');
		}
	/* fall through */
	case 18 ... 20:
		mvaddch(15, 5, '-');
	/* fall through */
	case 14 ... 17:
		mvaddch(14, 6, '\\');
	/* fall through */
	case 10 ... 13:
		mvaddch(15, 7, '\\');
	/* fall through */
	case 5 ... 9:
		mvaddch(14, 4, '/');
	/* fall through */
	case 0 ... 4:
		mvaddch(15, 3, '/');
	}
}

void hangman_printword(Hangman *hangman)
{
	move(16, 0);
	for (const char *w = hangman->word; *w != '\0'; w++) {
		if (isspace(*w)) {
			do
				w++;
			while (isspace(*w));
			addstr("   ");
		}
		if (isalpha(*w)) {
			attron(A_UNDERLINE);
			const int ind = tolower(*w) - 'a';
			if (hangman->letterStates[ind] &
					HANGMAN_LETTER_GUESSED)
				addch(*w);
			else
				addch(' ');
			attroff(A_UNDERLINE);
			addch(' ');
		}
	}
}

void hangman_printstatus(Hangman *hangman)
{
	move(4, 18);
	if (hangman->flags & HANGMAN_WIN) {
		printw("You win! %d%% of your tries were correct!",
			100 * hangman->guesses.needed /
				hangman->guesses.total);
	} else if (hangman->flags & HANGMAN_LOSE) {
		printw("You lose!", hangman->guesses.total);
	} else {
		const int remain = hangman->guesses.allowed -
				hangman->guesses.total +
				hangman->guesses.correct -
				hangman->guesses.needed;
		printw("%d tr%s remaining!", remain, remain == 1 ? "y" : "ies");
	}
	clrtoeol();
	if (hangman->flags & HANGMAN_GAME_OVER) {
		mvprintw(5, 18, "[q]uit [n]ew game [r]etry");
		clrtoeol();
	}
}

void hangman_printjumble(Hangman *hangman)
{
	move(18, 0);
	for (int i = 0; i <= 'z' - 'a'; i++) {
		if ((hangman->letterStates[i] & HANGMAN_LETTER_GUESSED) &&
				!(hangman->letterStates[i] &
					HANGMAN_LETTER_USED)) {
			addch('A' + i);
		} else {
			addch(' ');
		}
		addch(' ');
	}
}
