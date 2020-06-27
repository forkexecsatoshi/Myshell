
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"


#define AMP_FOUND	1
#define AMP_NOTFOUND	0

static int analyze_token();
static void skip_space();
static char consume_amp();
static char is_keychar(char c);
static void set_token_begin();
static void set_token_end();
static char next_char();
static void putback_char();


static char* src_text = "";
static int next_ptr = 0;
static int token_begin = 0;
static int token_end = 0;


void lexer_init(char* src) {
    src_text = src;
    next_ptr = 0;
    token_begin = 0;
    token_end = -1;
}


int lexer()
{
    int result;
    char c;

    skip_space();

    set_token_begin();
    result = analyze_token();
    set_token_end();

#ifdef DEBUG
    {
	char* txt = token_text();
	fprintf(stderr, "lexer():\"%s\"\n", txt);
	free(txt);
    }
#endif /*DEBUG*/

    return result;
}


char* token_text()
{
    int i, ptr;
    char* result
	= (char*) malloc(sizeof(char) * (token_end - token_begin + 2));
    for (i = 0, ptr = token_begin; ptr <= token_end; ++i, ++ptr) {
	result[i] = src_text[ptr];
    }
    result[i] = '\0';

    return result;
}


/* obtains a token from source text */
static int analyze_token()
{
    char c = next_char();

    switch (c) {
    case '\0' :	return END;
    case '>' :
	if (next_char() == '>') {
	    skip_space();
	    if (consume_amp() == AMP_FOUND)  return APPEND_ERR;
	    return APPEND;
	}
	putback_char();

	skip_space();
	if (consume_amp() == AMP_FOUND)  return STD_OUT_ERR;

	return STD_OUT;
    case '<' :	return STD_IN;
    case '|' :
	if (next_char() == '|')  return COND_OR;
	putback_char();

	skip_space();
	if (consume_amp() == AMP_FOUND)  return PIPE_ERR;

	return PIPE;
    case '&' :
	if (next_char() == '&')  return COND_AND;
	putback_char();

	return BACKGROUND;
    case ';' :	return SEMICOLON;
    case '(' :	return LPAR;
    case ')' :	return RPAR;
    default :
	do {
	    if (c == '\\') {
		c = next_char();
		if (c == '\0')  return ERR_UNKNOWN;
	    } else if (c == '\'') {
		do {
		    c = next_char();
		} while (c != '\'' && c != '\0');
		if (c == '\0')  return ERR_UNMATCHED_QUOTE;
	    } else if (c == '"') {
		do {
		    c = next_char();
		} while (c != '"' && c != '\0');
		if (c == '\0')  return ERR_UNMATCHED_DQUOTE;
	    }
	    c = next_char();
	} while (! is_keychar(c));
	putback_char();
	return WORD;
    }

    return ERR_UNKNOWN;
}


/* skips any space */
static void skip_space() {
    char c;
    do {
	c = next_char();
    } while (c == ' ' || c == '\t');
    putback_char();
}


/* consumes a single ampersand if it exists */
static char consume_amp() {
    if (next_char() == '&')  return AMP_FOUND;
    putback_char();
    return AMP_NOTFOUND;
}


/* tests if given character is a reserved key character or a space */
static char is_keychar(char c)
{
    return (c == ' ' || c == '\t' || c == '\0' || c == '>' || c == '<'
	    || c == '&' || c == '|' || c == ';' || c == '(' || c == ')');
}


static void set_token_begin()
{
    token_begin = next_ptr;
}

static void set_token_end()
{
    token_end = next_ptr - 1;
}

static char next_char()
{
    return src_text[next_ptr++];
}

static void putback_char()
{
    --next_ptr;
}
