#include "hangman.h"

static const char *const default_dictionary = "words_alpha.txt";

ProgramArgs program_args = {
	.tries = 6,
	.word = NULL,
	.dictionary = default_dictionary,
};

static void version(void)
{
	endwin();
	printf("Hangman version 1.0.0\n");
	exit(0);
}

static void usage(void)
{
	endwin();
	printf("Hang version 1.0.0, built 09/30/2023\n");
	printf("Play hangman within your terminal!\n\n");
	printf("--version\tShow the version\n");
	printf("--usage | --help\tShow this help\n");
	printf("The following program arguments can be used to change the experience:\n");
	printf("-tries=number\tSet the maximum number of failed attempts\n");
	printf("-word=word\tSet the used word\n");
	printf("-dict=path\tSet the used dictionary\n");
	exit(0);
}

static const char *possible_args[] = {
	"&", (char*) version, "-version", "v",
	"&", (char*) usage, "-usage", "usage",
	"&", (char*) usage, "-help", "help", "h",
	"&i", (char*) &program_args.tries, "tries",
	"&s", (char*) &program_args.word, "word",
	"&s", (char*) &program_args.dictionary, "dict",
};

int parse_args(int argc, char **argv)
{
	const char **pinfo, *info, *pos;
	char *arg;
	int p;
	size_t plen;

	for(int argi = 0; argi < argc; argi++) {
		arg = argv[argi];
		if(*arg != '-')
			return argi;
		while(*arg == '-')
			arg++;

		for(p = 0; p < (int) ARRLEN(possible_args); p++) {
			pos = possible_args[p];
			if(pos[0] == '&') {
				pinfo = possible_args + p;
				p += pos[1] == 0 ? 1 : strlen(pos) - 1;
				continue;
			}
			plen = strlen(pos);
			if(!strncmp(arg, pos, plen))
				break;
		}
		if(p == (int) ARRLEN(possible_args))
			return argi;
		arg += plen;
		info = *pinfo + 1;
		if((info[0] != 0) != (*arg == '='))
			return argi;
		pinfo++;
		if(info[0] == 0) {
			((void (*)(void)) *pinfo)();
			continue;
		}
		arg++;
		for(; *info; info++, pinfo++) switch(*info) {
		case 'i':
			*(int*) *pinfo = strtol(arg, (char**) &arg, 10);
			while(*arg == ',')
				arg++;
			break;
		case 's':
			*(char**) *pinfo = arg;
			break;
		}
	}
	return 0;
}
