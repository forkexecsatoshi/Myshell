
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "lexer.h"
#include "command.h"
 

void DELETE(p)	{
    if (p != NULL) {
        free(p);
        p = NULL;
    }
}

single_command *new_single_command()
{
    single_command *result ;
    	result = (single_command *)malloc( sizeof(single_command) );
        if( result == NULL )
	{
	    perror("no memory.");
	    exit( 1 );
	}
	memset( result, 0, sizeof(single_command) );
	result->id = CL_SINGLE_COMMAND ;
	return( result );
}

void delete_single_command( single_command *p )
{
    int i ;
    char *s ;
	if( p == NULL )
	    return;
	if( p->id != CL_SINGLE_COMMAND )
	{
	    fprintf(stderr, "0x%x(%d): not single_command\n", p,p->id );
	    exit( 2 );
	}
	for( i=0 ; i<p->argc ; i++ )
	{
	    s = p->argv[i] ;
	    free( s );
	    p->argv[i] = 0 ;
	}
}

pipe_line *new_pipe_line()
{
    pipe_line *result ;
    	result = (pipe_line *)malloc( sizeof(pipe_line) );
        if( result == NULL )
	{
	    perror("no memory.");
	    exit( 1 );
	}
	memset( result, 0, sizeof(pipe_line) );
	result->id = CL_PIPELINE ;
	return( result );
}

void delete_pipe_line( pipe_line *p )
{
    int i ;
    single_command *scom ;
	if( p == NULL )
	    return;
	if( p->id != CL_PIPELINE )
	{
	    fprintf(stderr, "0x%x(%d): not pipe_line\n", p,p->id );
	    exit( 2 );
	}
	for( i=0 ; i<p->ncommands ; i++ )
	{
	    scom = p->commands[i] ;
	    delete_single_command( scom );
	    p->commands[i] = 0 ;
	}
	free( p->commands );
	p->commands = 0 ;
}

pl_list *new_pl_list()
{
    pl_list *result ;
    	result = (pl_list *)malloc( sizeof(pl_list) );
        if( result == NULL )
	{
	    perror("no memory.");
	    exit( 1 );
	}
	memset( result, 0, sizeof(pl_list) );
	result->id = CL_PL_LIST ;
	return( result );
}

void delete_pl_list( pl_list *p )
{
    int i ;
    pipe_line *pline ;
	if( p == NULL )
	    return;
	if( p->id != CL_PL_LIST )
	{
	    fprintf(stderr, "0x%x(%d): not pl_list\n", p,p->id );
	    exit( 2 );
	}
	for( i=0 ; i<p->nplines ; i++ )
	{
	    pline = p->plines[i] ;
	    delete_pipe_line( pline );
	    p->plines[i] = 0 ;
	}
	free( p->plines );
	p->plines = 0 ;
}

void add_command_arg(single_command* scom, char* arg)
{
    if (scom->argv == NULL) {
	scom->argv = (char**) malloc(sizeof(char*)*2);
    } else {
	int newsize = scom->argc + 2;
	scom->argv = (char**) realloc(scom->argv, sizeof(char*) * newsize);
    }
    scom->argv[scom->argc++] = arg;
    scom->argv[scom->argc] = 0;
}

void add_pipe_line(pl_list *pll, pipe_line *pline)
{
    if (pll->plines == NULL) {
	pll->plines = (pipe_line **) malloc(sizeof(pipe_line *)*2);
    } else {
	int newsize = pll->nplines + 2;
	pll->plines = (char**) realloc(pll->plines, sizeof(char*) * newsize);
    }
    pll->plines[pll->nplines++] = pline;
    pll->plines[pll->nplines] = 0;
}

void add_single_command(pipe_line *pline, single_command *scom)
{
    if (pline->commands == NULL) {
	pline->commands = (single_command **) malloc(sizeof(single_command *)*2);
    } else {
	int newsize = pline->ncommands + 2;
	pline->commands = (char**) realloc(pline->commands, sizeof(char*) * newsize);
    }
    pline->commands[pline->ncommands++] = scom;
    pline->commands[pline->ncommands] = 0;
}

