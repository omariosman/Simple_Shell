#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<dirent.h>
#include<fcntl.h>
#include<stdbool.h>

#define BUFFER_LEN 1024


size_t read_command(char *cmd) {
if(!fgets(cmd, BUFFER_LEN, stdin)) 
return 0; 
size_t length = strlen(cmd);
if (cmd[length - 1] == '\n') cmd[length - 1] = '\0'; 
return strlen(cmd); 
}

int build_args(char * cmd, char ** argv) {
	char *token;
	token = strtok(cmd," ");
	int i=0;
	while(token!=NULL){
		argv[i]=token; 
		token = strtok(NULL," "); 
		i++;
	}
	argv[i]=NULL; 
	return i;
}

void set_program_path (char * path, char * bin, char * prog) {
	memset (path,0,1024); 
	strcpy(path, bin); 
	strcat(path, prog);
	int i;
	for(i=0; i<strlen(path); i++) 
		if(path[i]=='\n') path[i]='\0';
}


int main(){
	char line[BUFFER_LEN];
	char line2[BUFFER_LEN]; 
	char pipe_line[BUFFER_LEN]; 
	char* argv[100]; 
	char* bin= "/bin/"; 
	char path[1024]; 
	int argc; 
	char *var[2];
	while(1){
	
		printf("My shell>> ");
		char s[100];
		printf("%s ", getcwd(s,100));
		if (read_command(line) == 0 )
		{printf("\n"); break;} 
		if (strcmp(line, "exit") == 0) break;
		
		
		strcpy(line2, line);
		strcpy(pipe_line, line);
	
		

		argc = build_args (line,argv); 
		set_program_path (path,bin,argv[0]);		
		

		
		int pid= fork(); 
		if(pid==0){	
			char *cd_str = argv[0];
			/* Change Directory = cd */
			if (argv[0] == cd_str){	
				chdir(argv[1]);
			}
	
			/* loop over arguments to get redirection operators */
			
			bool output_redirect_flag, input_redirect_flag, grep_found = false;
			char *file_name;
			char *file_name_input;
			int i;
			int inputredirection, outputredirection;
			inputredirection=100;
			outputredirection=100;
						
			for(i = 0; i < argc; i++){
				/* input redirection */
		
				if(strcmp(argv[i], "<") == 0){
				
					inputredirection=i;
					input_redirect_flag = true;
					file_name_input = argv[i+1];
					/*argv[i] = '\0';*/
					
				}

				
			}
			
			
			if (input_redirect_flag){
				printf("%s", file_name_input);
				int file_input = open(file_name_input, O_RDONLY);
				if (file_input == -1){
					"Opening file failed";
					return 1;
				}
				/*int restore_input = dup(0);*/
				dup2(file_input, 0);
				/*close(file_input);*/
				/*execve(path,argv,0);*/
				/*dup2(restore_input, 0);*/
			}	
			for(i = 0; i < argc; i++){
				/* output redirection */
		
				if(strcmp(argv[i], ">") == 0){
				
					outputredirection=i;
					output_redirect_flag = true;
					file_name = argv[i+1];
					/*argv[i] = '\0';*/
					
				}

				
			}

			if (output_redirect_flag){
				printf("%s", file_name);
				int file = open(file_name, O_WRONLY | O_CREAT, 0777);
				if (file == -1){
					"Opening file failed";
					return 1;
				}
				/*int restore = dup(1);*/
				dup2(file, 1);
				/*close(file);*/
				/*execve(path,argv,0);*/
				/*dup2(restore, 1);*/
			}

			if (inputredirection != 100){
				argv[inputredirection] = "\0";
			
			}else if (outputredirection != 100){
				argv[outputredirection] = "\0";
			}

		/* Simulating the pipe operator */
		
		bool pipe_found = false;
		int j;
		int pipe_counter = 0;
		for (i = 0; i < argc; i++){
			if (strcmp(argv[i], "|") == 0){
				j = i + 1;
				pipe_counter++;
				
				pipe_found = true;
			}
		}
		
		
		char* argv_cmd1[100]; 
		char* argv_cmd2[100];
		char* argv_cmd3[100]; 
		char path_cmd1[1024]; 
		char path_cmd2[1024]; 
		char path_cmd3[1024]; 
		char *cmd1, *cmd2, *cmd3, *pipe_token;
		
		if (pipe_found){
		
		if (pipe_counter == 2){
		/* Start of Two pipes */
		
			cmd1 = strtok(pipe_line, "|");
			cmd2 = strtok(NULL, "|");
			cmd3 = strtok(NULL, "|");
			/*
			printf("cmd: %s\n", cmd1);
			printf("cmd: %s\n", cmd2);
			printf("cmd: %s\n", cmd3);
			*/

		int argc_cmd1, argc_cmd2, argc_cmd3;
		
		argc_cmd1 = build_args(cmd1, argv_cmd1);
		set_program_path(path_cmd1, bin, argv_cmd1[0]);

		
		
		argc_cmd2 = build_args(cmd2, argv_cmd2);
		set_program_path(path_cmd2, bin, argv_cmd2[0]);
		
		
		argc_cmd3 = build_args(cmd3, argv_cmd3);
		set_program_path(path_cmd3, bin, argv_cmd3[0]);	
		
				
		int multiple_pipes[2][2];
		
		if (pipe(multiple_pipes[0]) == -1){
			printf("Fail");
			return 4;
		}
		if (pipe(multiple_pipes[1]) == -1){
			printf("Fail");
			return 5;
		}
		
		int first = fork();
		if (first == 0){
			dup2(multiple_pipes[0][1], 1);
			close(multiple_pipes[0][0]);
			close(multiple_pipes[1][0]);
			close(multiple_pipes[1][1]);
		
			execve(path_cmd1, argv_cmd1, 0);
		}
		
		int second = fork();
		if (second == 0){
			dup2(multiple_pipes[0][0], 0);
			dup2(multiple_pipes[1][1], 1);
			close(multiple_pipes[0][1]);
			close(multiple_pipes[1][0]);
		
			close(multiple_pipes[1][1]);
			execve(path_cmd2, argv_cmd2, 0);
		}		
	
		int third = fork();
		if (third == 0){
		
			dup2(multiple_pipes[1][0], 0);
			close(multiple_pipes[0][0]);
			close(multiple_pipes[0][1]);
			close(multiple_pipes[1][1]);
		
			execve(path_cmd3, argv_cmd3, 0);
		
		}
		
		close(multiple_pipes[0][0]);
		close(multiple_pipes[0][1]);
		close(multiple_pipes[1][0]);		
		close(multiple_pipes[1][1]);
		waitpid(first, NULL, 0);
		waitpid(second, NULL, 0);
		waitpid(third, NULL, 0);


/* End of Two pipes */
		}
		
		else if (pipe_counter == 1){
		/* Start of one pipe */
		
			cmd1 = strtok(pipe_line, "|");
			cmd2 = strtok(NULL, "|");
		
			/*
			printf("cmd: %s\n", cmd1);
			printf("cmd: %s\n", cmd2);
			printf("cmd: %s\n", cmd3);
			*/

		int argc_cmd1, argc_cmd2, argc_cmd3;
		
		argc_cmd1 = build_args(cmd1, argv_cmd1);
		set_program_path(path_cmd1, bin, argv_cmd1[0]);

		
		
		argc_cmd2 = build_args(cmd2, argv_cmd2);
		set_program_path(path_cmd2, bin, argv_cmd2[0]);
		

		int one_pipe[2];
		
		if (pipe(one_pipe) == -1){
			printf("Fail");
			return 6;
		}
	
		
		int first = fork();
		if (first == 0){
			dup2(one_pipe[1], 1);
		
			close(one_pipe[0]);
			close(one_pipe[1]);
		
			execve(path_cmd1, argv_cmd1, 0);
		}
		
		int second = fork();
		if (second == 0){
			dup2(one_pipe[0], 0);
		
			close(one_pipe[0]);
			close(one_pipe[1]);
			execve(path_cmd2, argv_cmd2, 0);
		}		
	

		
		close(one_pipe[0]);
		close(one_pipe[1]);
		waitpid(first, NULL, 0);
		waitpid(second, NULL, 0);



		
			
		}
		}


		
		
			
		


	
		
		/*
		if (grep_found) {
		/* copying the grep command into another array */
		/*
		char *parsed_cmd[100];
		for(i = 0; i < argc; i++){
			parsed_cmd[i] = argv[j];
			j++;
		}
		parsed_cmd[i] = '\0';
*/
		
		/* Opening a pipe between this child and another new child */
		/*
		int fd[2];
		if (pipe(fd) == -1){
			printf("%s", "Pipe Failed");
			return 2;
		}
		
		
*/
		
		/*execve(path, argv, 0);*/
		/*
		int pid_ch1 = fork();
		if (pid_ch1 == 0){
		
			dup2(fd[1], 1);
			/*close(fd[1]);*/
			/*
			close(fd[0]);
			execve(path, argv, 0);
			
				
		}
		
		int pid_ch2 = fork();
		if (pid_ch2 == 0){
			dup2(fd[0], 0);
			/*close(fd[0]);*/
			/*
			close(fd[1]);
			
			execve(path, parsed_cmd, 0);
		
		}
		waitpid(pid_ch1, NULL, 0);
		waitpid(pid_ch2, NULL, 0);
		
		}
		*/
		
		/***** Environment variables *****/
		bool equal_flag = false;
		for (i = 0; i < argc; i++){
			if (strcmp(argv[i], "=") == 0){
				var[0] = argv[i-1]; /* X */
				var[1] = argv[i+1]; /* 100 */ 
					/*char *endPtr;
					val = strtol(argv[i+1], &endPtr, 10);*/
					equal_flag = true;
					
			}
		}
		

			

		

		
		
		/***** Environment Variables with Commands *****/
		bool tick_flag = false;
		char x;
		int k;
		for (k = 0; k < strlen(line); k++){
			if (line[k] == '`'){
				tick_flag = true;
				x = line[k-2];
				
				break;
			} 

		}
		int argc2;
		char* argv2[100];
		char path2[1024]; 
		char *var_env;
		if (tick_flag){
			char *my_token = strtok(line2, "`");
			
			
			my_token = strtok(NULL, "`");		
			
			argc2 = build_args (my_token,argv2); 
			set_program_path (path2,bin,argv2[0]);
			
		int tick_pipe[2];
		if (pipe(tick_pipe) == -1){
			printf("%s\n", "Pipe Failed");
			return 3;
		}
		int tick_child = fork();
		if (tick_child == 0){
			close(tick_pipe[0]);
			dup2(tick_pipe[1], 1);
			execve(path2,argv2,0);
		
		}
		
		
		waitpid(tick_child, NULL, 0);
		

		
		char my_buffer[4096];
		read(tick_pipe[0], my_buffer, 4096);
		
		
		/*printf("buffer: %s", my_buffer);*/
		setenv(&x, my_buffer, 1);
		char *test;
		test = getenv(&x);
		/*printf("%s", test);*/
		close(tick_pipe[0]);
		close(tick_pipe[1]);
		/*
		
		
		char * value1;
		char *parsed_var1;
		/*printf("%s", test);*/
		/*
		if (strcmp(argv[0], "echo") == 0){
			parsed_var1 = &argv[1][1];
			value1 = getenv(parsed_var1);
			printf("%s\n", value1);
			
		}
		*/
			
		}
		
		bool env_flag, dollar_flag = false;
		if (equal_flag){
			setenv(var[0], var[1], 1);
		}
		char * value;
		char *parsed_var;
		if (strcmp(argv[0], "echo") == 0){
			parsed_var = &argv[1][1];
			if (strchr(argv[1], '$') != NULL){
			
				dollar_flag = true;
			}
			
			value = getenv(parsed_var);
			printf("%s\n", value);
			env_flag = true;
		}
		
		if (!dollar_flag && !pipe_found){
			execve(path,argv,0);
		}
		
		/*execve(path,argv,0);*/
		
		/*fprintf(stderr, "Child process could not do execve\n");*/
	}else wait(NULL); 
	} return 0;
}
