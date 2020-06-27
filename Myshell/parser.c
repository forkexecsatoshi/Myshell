#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "lexer.h"
#include "parser.h"


#define ERROR(msg)	{\
    last_error = (msg);\
    return NULL;\
}
#define CHECK_ERROR(retvalue)	{\
    if ((retvalue) == NULL)  return NULL;\
}
#define CHECK_ERROR_D(retvalue, disposal) {\
    if ((retvalue) == NULL) {\
        delete_command(disposal);\
        return NULL;\
    }\
}
#define PANIC(token) {\
    fprintf(stderr, "Unexpected token %d\n", token);\
    exit(1);\
}
#define NEXT_TOKEN() {\
    current_token = lexer();\
    if (current_token == ERR_UNMATCHED_QUOTE) {\
        ERROR("Unmatched '.");\
    } else if (current_token == ERR_UNMATCHED_DQUOTE) {\
        ERROR("Unmatched \".");\
    } else if (current_token == ERR_UNKNOWN) {\
        ERROR("Unknown character.");\
    }\
}

static pl_list* parse_pl_list();
static void pipe_line_setctype( pipe_line *pline, int token );
static pipe_line *parse_pipe_line();
static single_command* parse_single_command();
static single_command* parse_single_command_loop( single_command *result );

static int current_token = ERR_UNKNOWN;
static char* last_error = "Unknown error.";


char* parser_error()
{
    return last_error;
}


pl_list* parse(char* src_text)
{
    pl_list* result;

    lexer_init(src_text);
    last_error = NULL;

    NEXT_TOKEN();
    result = parse_pl_list();
    CHECK_ERROR(result);

    if (current_token != END) {
	if (current_token == RPAR) {
	    ERROR("Too many )'s.");
	}
	ERROR("Syntax error.");
    }

    return result;
}

static pl_list* parse_pl_list()
{
    pl_list* result;
    pipe_line* pline;
    int i ;

    pline  = parse_pipe_line();
    if( pline == NULL )
	return( NULL );
    result = new_pl_list();
    add_pipe_line( result, pline );

    /* pline holds the previous pipe_line */
    while( current_token == BACKGROUND ||
	   current_token == SEMICOLON ||
	   current_token == COND_AND ||
	   current_token == COND_OR )
    {
	pline = result->plines[result->nplines-1] ;
	pipe_line_setctype( pline, current_token );
	NEXT_TOKEN(); /* discard BACKGROUND */
	pline = parse_pipe_line();
	if( pline == NULL )
	{
	    delete_pl_list( result );
	    return( NULL );
	}
	add_pipe_line( result, pline );
    }
    return result;
}

static void pipe_line_setctype( pipe_line *pline, int token )
{
	switch( token )
	{
	case BACKGROUND:
	    pline->ctype = CL_CTYPE_BACKGROUND;
	    break;
	case SEMICOLON:
	    pline->ctype = CL_CTYPE_SEQ ;
	    break;
	case COND_AND:
	    pline->ctype = CL_CTYPE_AND ;
	    break;
	case COND_OR:
	    pline->ctype = CL_CTYPE_OR ;
	    break;
	default:
	    PANIC(token);
	}
}

static pipe_line *parse_pipe_line()
{
    pipe_line* result;
    single_command* scom;

    scom = parse_single_command();
    if( scom == NULL )
	return( NULL );
    result = new_pipe_line();
    add_single_command( result, scom );
    
    if( scom->soutfile && (current_token==PIPE||current_token == PIPE_ERR) )
    {
	last_error = "Ambiguous input redirect.";
	return( NULL );
    }

    /* scom holds the previous single_command. */
    while( current_token == PIPE ||
	   current_token == PIPE_ERR )
    {
	if( current_token == PIPE_ERR )
	    scom->out_mode |= CL_OUT_ERROR;
	NEXT_TOKEN(); /* discard PIPE or PIPE_ERR */
	scom = parse_single_command();
	if( scom == NULL )
	{
	    delete_pipe_line( result );
	    return( NULL );
	}
	if( scom->sinfile )
	{
	    last_error = "Ambiguous input redirect.";
	    delete_single_command( scom );
	    delete_pipe_line( result );
	    return( NULL );
	}
    	add_single_command( result, scom );
    }
    return result;
}

static single_command* parse_single_command()
{
    single_command* result ;
    result = new_single_command();
    if( parse_single_command_loop(result) == NULL )
    {
	delete_single_command(result);
	return( NULL );
    }
    return( result );
}

static single_command* parse_single_command_loop( single_command *result )
{
    int lookahead;
    switch (lookahead = current_token) {
    case WORD :
	add_command_arg(result, token_text());
	NEXT_TOKEN(); /* discard WORD */
	break;
    case STD_IN :
	if (result->sinfile != NULL) {
	    ERROR("Ambiguous input redirect.");
	}
	NEXT_TOKEN(); /* discard STD_IN */
	if (current_token != WORD) {
	    ERROR("Missing name for redirect.");
	}
	result->sinfile = token_text();
	NEXT_TOKEN(); /* discard WORD */
	break;
    case STD_OUT :
    case STD_OUT_ERR :
    case APPEND :
    case APPEND_ERR :
	if (result->soutfile != NULL) {
	    ERROR("Ambiguous output redirect.");
	}
	NEXT_TOKEN(); /* discard STD_OUT, STD_OUT_ERR, APPEND or APPEND_ERR */
	if (current_token != WORD) {
	    ERROR("Missing name for redirect.");
	}
	result->soutfile = token_text();
	NEXT_TOKEN(); /* discard WORD */
	if (lookahead == STD_OUT_ERR || lookahead == APPEND_ERR) {
	    result->out_mode |= CL_OUT_ERROR;
	}
	if (lookahead == APPEND || lookahead == APPEND_ERR) {
	    result->out_mode |= CL_OUT_APPEND;
	}
	break;
    default:
	return result;
    }
    return parse_single_command_loop(result);
}

