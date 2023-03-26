
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "command.h"
#include <fstream>
#include <time.h>


void handleLogFile(int signum){
	
	//errfd = open(_errFile, O_WRONLY|O_APPEND);
	FILE* out = fopen("file.log","a");
	time_t t = time(NULL);
	fprintf(out, "Process is terminated in %s",ctime(&t));
}

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::print()
{
	
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
		printf("\n");
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf("append flag = %d", _append);	
	printf( "\n\n" );
	
}

void
Command::execute()
{
	// steps according to lab 3 pdf : 
	// for each simple command : 	
	// 1- fork a new process
	// 2- call execv  to execute the command specified
	// 3- check the background flag : waitPID


	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
	print();

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec
	/* *****************Execution*********************** */

	// //Creating pipe for all commands

		


		//Redirecting input, output and error files





	int defaultin = dup(0); // copying the FD of default input
	int defaultout = dup(1); // copying the FD of default output
	int defaulterr = dup(2); // copying the FD of default error	
	dup2(defaulterr,2);
	close(defaulterr);
	
	int infd;
	int outfd;
	int errfd;
	//checking if there is input file specified
	if(_inputFile != NULL){
		infd = open( _inputFile, O_RDONLY );
	

		if ( infd < 0 ) {
			perror("create input file" );
 			exit( 2 );
		}
		dup2(infd,0);
		close(infd);
	}else{
		infd = dup(defaultin);
		close(infd);
	}



	int fdpipe[2];
	int pipe_check;
	int pid;
	for(int i = 0; i < _numberOfSimpleCommands; i++){

		//redirecting input 
		dup2(infd,0);
		// if first command it will be assigned to the default or the input file specified 
		// if it is not the first command it will be assigned to input in in the next iteration.
		close(infd);


	

		if(strcmp(_simpleCommands[i]->_arguments[0],"cd") == 0){
			if(_simpleCommands[i]->_arguments[1] == NULL){
				chdir(getenv("HOME"));
			}
			else{
				chdir(_simpleCommands[i]->_arguments[1]);
			}
		}
		
		
		if(i == _numberOfSimpleCommands-1){
			if(_outFile != NULL && _append == 1) {
				outfd = open(_outFile, O_WRONLY|O_APPEND);
				if(outfd < 0){
					perror("error in opening output file");
					exit(2);
				}
			}
			else if(_outFile != NULL && _append == 0){
				outfd = creat( _outFile, 0666 );
	

				if ( outfd < 0 ) {
					perror("create outfile" );
					exit( 2 );	
				}

			}
			else {
				outfd = dup(defaultout);
			}

			if(_errFile != NULL && _append == 1) {
				errfd = open(_errFile, O_WRONLY|O_APPEND);
				if(errfd < 0) {
					perror("error in opening error file");
					exit(2);
				}
				dup2(errfd,2);
				close(errfd);
			}

			else if (_errFile != NULL && _append == 0) {
				errfd = creat(_errFile,0666);
				if(errfd < 0) {
					perror("error creating error file");
					exit(2);
				}
				dup2(errfd,2);
				close(errfd);				
			}
		
		}

		else {
			//Create the pipe
			pipe_check = pipe(fdpipe);
			if(pipe_check== -1) {
				perror("error creating pipe");
				exit(2);
			}
			else {
				infd = fdpipe[0];
				outfd = fdpipe[1];
				
			}
		}

		dup2(outfd,1);
		//dup2(infd,0);
		//close(infd);
		close(outfd);
		signal(SIGCHLD, handleLogFile);

		pid = fork();

		if(pid < 0){
			perror( "failed to fork\n");
			exit( 2 );
		}
		else if (pid == 0) { //Child
			

			//Closing unnecessary files
			close(defaultin);
			close(defaultout);
			close(defaulterr);

			execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);
			
			// perror( "exec error");
			exit( 2 );	

		}


		
	}

			//restoring
		dup2(defaultin,0);
		dup2(defaultout,1);
		dup2(defaulterr,2);
		

		

		//closing
		close(defaultin);
		close(defaultout);
		close(defaulterr);


		if(!_background) {
			waitpid(pid,0,0);
			
		}
	/* ************************************************* */

	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation

void
Command::prompt()
{
	printf("myshell>");
	signal(SIGINT, SIG_IGN);
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

int 
main()
{
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

