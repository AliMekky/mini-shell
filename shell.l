
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	

">" 	{
		return GREAT;
	}

"<"		{
		return LESS;
	}

">>"	{
		return DGREAT;
}		

"|"		{
		return PIPE;
}

">&"	{
		return OUT_ERROR;
}

">>&"	{
		return OUT_ERROR_APPEND;
}

"exit"	{
		return EXIT;
}

"&"		{
		return BACKGROUND;
}



[^ \t\n\|&><][^ \t\n\|&><]*  {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}

.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
