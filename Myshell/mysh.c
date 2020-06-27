
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "executer.h"
#include "command.h"
#include<signal.h>

#define COMMANDLINE_CHAR_MAX 1024
#define PROMPT "@ "
#define CONTINUING_PROMPT "? "

int mysh(FILE* sin);
static void eval_oneline(char* src_text);
static void chop(char* text);
static void show_prompt(char* prompt_text);


int main(int argc, char* argv[])
{
	struct sigaction sa_ignore1;
	struct sigaction sa_ignore2;

	memset(&sa_ignore1, 0, sizeof(sa_ignore1));
	sa_ignore1.sa_handler = SIG_IGN;

	if (sigaction(SIGINT, &sa_ignore1, NULL) < 0) {
		perror("sigaction");
		exit(1);
	}
	
		return mysh(stdin);
}


int mysh(FILE* input_fp)
{
	
    char* buf = (char*) malloc(sizeof(char) * COMMANDLINE_CHAR_MAX);
	char* check_buf = (char*)malloc(sizeof(char) * COMMANDLINE_CHAR_MAX);

    show_prompt(PROMPT);
	    while (fgets(buf, COMMANDLINE_CHAR_MAX, input_fp)!=NULL) {
			chop(buf);
			eval_oneline(buf);
			show_prompt(PROMPT);
    }

    free(buf);
}


static void show_prompt(char* prompt_text)
{
    fprintf(stdout, "%s", prompt_text);
    fflush(stdout);
}


static void eval_oneline(char* src_text)
{
    pl_list* pll = parse(src_text);

    if (pll != NULL) {
	execute_pl_list(pll,0);
	delete_pl_list(pll);
    } else {
	fprintf(stderr, "%s\n", parser_error());
    }
}


static void chop(char* text)
{
    int i;
    size_t length = strlen(text);

    for (i = 1; i <= length; ++i) {
	if (text[length - i] != '\n' && text[length - i] != '\r')  return;
	text[length - i] = '\0';
    }
}
