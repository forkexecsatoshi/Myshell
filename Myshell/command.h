#ifndef _command_h_
#define _command_h_

#define CL_UNKNOWN		0x00
#define CL_EMPTY		0x01
#define CL_SINGLE_COMMAND	0x02
#define CL_PIPELINE		0x03
#define CL_PL_LIST		0x04

#define CL_CTYPE_EOL		0x00
#define CL_CTYPE_SEQ		0x01
#define CL_CTYPE_AND		0x02
#define CL_CTYPE_OR		0x03
#define CL_CTYPE_BACKGROUND	0x04

#define CL_OUT_NORMAL	0x00
#define CL_OUT_ERROR	0x01
#define CL_OUT_APPEND	0x02

typedef struct _single_command {
    int id;
    char* sinfile;
    char* soutfile;
    int out_mode;
    int argc;
    char** argv;
} single_command;

#define PIPE_LINE_MAX_0COMMANDS 100
typedef struct _pipe_line {
    int id;
    int ctype ;
    int ncommands ;
    single_command **commands;
} pipe_line;

typedef struct _pl_list {
    int id;
    int nplines ;
    pipe_line **plines;
} pl_list;

single_command	*new_single_command();
pipe_line	*new_pipe_line();
pl_list	*new_pl_list();
void		 delete_single_command(single_command*);
void		 delete_pipe_line(pipe_line*);
void		 delete_pl_list(pl_list*);

void add_command_arg(single_command* scom, char* arg);
void add_pipe_line(pl_list *pll, pipe_line *pline);
void add_single_command(pipe_line *pline, single_command *scom);

#endif /*_command_h_*/