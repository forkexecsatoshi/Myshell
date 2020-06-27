#define END		0x00 /* end of source string		*/
#define WORD		0x01 /* a certain word			*/
#define STD_OUT		0x10 /* '>'				*/
#define STD_IN		0x11 /* '<'				*/
#define STD_OUT_ERR	0x12 /* '>&' (regexp '>[ \t]*&')	*/
#define APPEND		0x15 /* '>>'				*/
#define APPEND_ERR	0x17 /* '>>&' (regexp '>>[ \t]*&')	*/
#define PIPE		0x20 /* '|'				*/
#define PIPE_ERR	0x21 /* '|&' (regexp '|[ \t]*&')	*/
#define BACKGROUND	0x31 /* '&'				*/
#define SEMICOLON	0x32 /* ';'				*/
#define LPAR		0x41 /* '('				*/
#define RPAR		0x42 /* ')'				*/
#define COND_AND	0x51 /* '&&'				*/
#define COND_OR		0x52 /* '||'				*/

#define ERR_UNKNOWN		0x80 /* unknown error			*/
#define ERR_UNMATCHED_QUOTE	0x81 /* single quotations unmatched	*/
#define ERR_UNMATCHED_DQUOTE	0x82 /* double quotations unmatched	*/


void lexer_init(char* src);
int lexer();
char* token_text();
