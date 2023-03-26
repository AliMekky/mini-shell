
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token 	NOTOKEN GREAT NEWLINE DGREAT LESS PIPE BACKGROUND	OUT_ERROR OUT_ERROR_APPEND EXIT CD

%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdlib.h>
#include <stdio.h>
#include "command.h"
#include <unistd.h>
#include <string.h>
#include <glob.h>
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;




simple_command:	
/*	command_and_args iomodifier_opt NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| command_and_args iomodifier_in NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| command_and_args check_background NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();		
	}
	| command_and_args iomodifier_opt_append NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();		
	}*/
	pipe_struct NEWLINE{
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();		
	}
	|
	pipe_struct check_background NEWLINE{
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();		
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;


pipe_struct:
	pipe_commands iomodifier_opt
	| pipe_commands
	| pipe_commands iomodifier_in
	| pipe_commands iomodifier_opt iomodifier_in
	| pipe_commands iomodifier_in  
	| pipe_commands iomodifier_in iomodifier_opt
	;


pipe_commands:
	| pipe_commands PIPE command_and_args
	/*| command_and_args PIPE command_and_args*/
	| command_and_args	
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {	    
			char* found1 = strchr($1,'*');
		   	char* found2 = strchr($1,'?');
		   	glob_t wildcardStruct ;
			printf("   Yacc: insert argument \"%s\"\n", $1);
	
			int r = glob($1,0 , NULL, &wildcardStruct);
			if( r!= 0){
				if(r == GLOB_NOMATCH){
					Command::_currentSimpleCommand->insertArgument( $1 );
				}
				else{
					perror("glob error");
				}
			}
			else{
				for(int i = 0;i<wildcardStruct.gl_pathc;i++){
					Command::_currentSimpleCommand->insertArgument( wildcardStruct.gl_pathv[i] );
				}
					
			}
		   
		}
	;

command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);
			   Command::_currentSimpleCommand = new SimpleCommand();
			   Command::_currentSimpleCommand->insertArgument( $1 );	       
	}
	|
	EXIT {
		printf("\n GOODBYE !!\n");
		exit(0);
	}
	;

iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	| DGREAT WORD {
		printf("   Yacc: insert output and append \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._append = 1;
	}	
	| OUT_ERROR WORD {
		printf("   Yacc: insert output and append \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._errFile = $2;	
	}
	| OUT_ERROR_APPEND WORD {
		printf("   Yacc: insert output and append \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand._append = 1;

	}				
	| /* can be empty */ 
	;

iomodifier_in:
	LESS WORD {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	}
	| /* can be empty */ 
	;


check_background:
	BACKGROUND {
		printf("   Yacc: Setting BACKGROUND flag \n");
		Command::_currentCommand._background = 1;
	}
	| /* can be empty */ 
	;




%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif

