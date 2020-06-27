#include <stdio.h>
#include<stdlib.h>
#include "command.h"
#include "executer.h"
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<signal.h>
static int execute_pipe_line( pipe_line *pline, int level, int nth, int nplines );
static int execute_single_command( single_command *scom, int level, int nth, int ncoms ,pipe_line *pline);
static void indent(int level);
int flag = 0;

int execute_pl_list( pl_list *pll, int level )
{
    int i;
    int result ;
    pipe_line *pline ;
	for( i=0 ; i<pll->nplines ; i++ )
	{
	    pline = pll->plines[i] ;
	    result = execute_pipe_line( pline, level+1, i, pll->nplines );
	}
	return( result );
}

static int
execute_pipe_line( pipe_line *pline, int level, int nth, int nplines )
{
    int i;
    int result ;
    single_command *scom ;
	for( i=0 ; i<pline->ncommands ; i++ )
	{
	    scom = pline->commands[i] ;
	    result = execute_single_command( scom, level+1, i,
					     pline->ncommands,pline);
	}
    
	return( result );
}

static int
execute_single_command(single_command* scom, int level, int nth, int ncoms, pipe_line* pline)
{
	int i;
	pid_t pid;
	int result;
	int j;

	int fd[4][2];
	int outfd;
	int infd;
	int erfd;
	int out = 0;
	int in = 0;
	int status;
	char* word;
	extern char** environ;

	if(scom->argc >= 1){
		if (strcmp(scom->argv[0],"exit") == 0){
			exit(0);
		}
	} 

	int pipe_num = ncoms - 1;

	if (pipe_num == 0) {
		if ((pid = fork()) == 0) {
			if (scom->sinfile) {
				infd = open(scom->sinfile, O_RDONLY);

				if (infd < 0) {
					perror("open");
					exit(1);
				}
				close(0);

				if (dup2(infd, 0) < 0) {
					perror("dup2");
					close(infd);
					exit(1);
				}
				close(infd);
			}
            if (scom->soutfile && !(scom->out_mode)) {
                outfd = open(scom->soutfile,O_WRONLY|O_CREAT|O_TRUNC,0666);
                if (outfd < 0) {
                    perror("open");
                    exit(1);
                }
                close(1);
                if (dup2(outfd, 1) < 0) {
                    perror("dup2");
                    close(outfd);
                }
                close(outfd);
            }
            else if (scom->out_mode) {
					erfd = open(scom->soutfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
					if (erfd < 0){
						perror("open");
						exit(1);
					}
					close(2);
					if (dup2(erfd, 2) < 0) {
						perror("dup2");
						close(erfd);
					}
					close(erfd);
				}
			
			execvp(pline->commands[0]->argv[0], pline->commands[0]->argv);
			exit(1);
		}
		else {
			wait(&status);
		}
	}
	else if (flag == 0) {
		flag = 1;
		for (j = 0; j < pipe_num + 1 && pipe_num != 0; j++) {
			if (j != pipe_num) {
				pipe(fd[j]);
			}
			if ((pid = fork()) == 0) {
				if (j == 0) {
					dup2(fd[j][1], 1);
					close(fd[j][0]);
					close(fd[j][1]);
					if (pline->commands[0]->sinfile) {
						infd = open(pline->commands[0]->sinfile, O_RDONLY);
						if (infd < 0) {
							perror("open");
							exit(1);
						}
						close(0);
						dup2(infd, 0);
						close(infd);
					}
				}
				else if (j == pipe_num) {
					dup2(fd[j - 1][0], 0);
					close(fd[j - 1][0]);
					close(fd[j - 1][1]);
					if (pline->commands[j]->soutfile) {
						outfd = open(pline->commands[j]->soutfile, O_WRONLY|O_CREAT|O_TRUNC,0666);
						if (outfd < 0) {
							perror("open");
							exit(1);
						}
						close(1);
						dup2(outfd, 1);
						close(outfd);
					}


				}
				else {
					dup2(fd[j - 1][0], 0);
					dup2(fd[j][1], 1);
					close(fd[j - 1][0]);
					close(fd[j - 1][1]);
					close(fd[j][0]);
					close(fd[j][1]);
				}

				
				execvp(pline->commands[j]->argv[0], pline->commands[j]->argv);
				exit(0);
			}else if (j > 0) {
				close(fd[j - 1][0]);
				close(fd[j - 1][1]);
			}
		}
	

	}
	if (pipe_num > 0) {
		wait(&status);
	}

    if (nth == ncoms - 1) {
        flag = 0;
    }
	

	return(result);

    }
	

static void
indent( int level )
{
    int i ; 
	for( i=0 ; i<level ; i++ )
	{
	    printf("    ");
	}
}
