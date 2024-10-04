#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>



int newline = 1;
int num_commands = 0;
int redirection = 0;
int num_phrases = 0;
int background = 0;
int redirection_error = 0;


char* read_file = NULL;
char* write_file = NULL;
char** arg = NULL;
char* trim(char* my_token){
    char *end;
    while (*my_token == ' '){
        my_token++;
    }
    end = my_token + strlen(my_token) - 1;
    while (end > my_token && *end == ' '){
        end--;
    }

    *(end + 1) = '\0';
    return my_token;
}



char** parse(char *input){
    char **output = (char**)malloc(100*sizeof(char*));
    char *token = strtok(input, "|");
    char *new_token;
    int count = 0;
    while(token != NULL){
        new_token = trim(token);
        //printf("%s\n", token);
        output[count] = (char*)malloc(strlen(new_token)+ 1);
        strcpy(output[count], new_token);
        output[count][strlen(new_token)] = '\0';
        //printf("%s\n", output[count]);
        fflush(stdout);
        token = strtok(NULL, "|");
        count++;

    }
    output[count] = NULL;
    num_commands = count;
    return output;
}

void remove_quotes(char* arg){
    char buffer[100];
    int i=0, j=0;
    while (arg[i] != '\0'){
        if (arg[i] != '\"' && arg[i] != '\''){
            buffer[j] = arg[i];
            j++;
        }
        i++;
    }
    buffer[j] = '\0';
    strcpy(arg, buffer);
}

char** parse_command(char* input){
    char **output = (char**)malloc(100*sizeof(char*));
    char *token = strtok(input, " ");
    char *new_token;
    int count = 0;

    while (token != NULL){
        new_token = trim(token);
        output[count] = (char*)malloc(strlen(new_token) + 1);
        strcpy(output[count], new_token);
        remove_quotes(output[count]);
        token = strtok(NULL, " ");
        //printf("%s\n", output[count]);
        count++;

    }
    output[count] = NULL;
    num_phrases = count;
    return output;
}

char* prompt(){
    static char input[100];
    if (newline){
        printf("my_shell$");
    }

    if (fgets(input, sizeof(input), stdin) == NULL){
        return NULL;
    }
    if (strlen(input) > 0 && input[strlen(input) - 1] == '\n'){
        input[strlen(input) - 1] = '\0';
    }
    //int k = 0;
    /*
    while (input[k] != '\0'){
        if (input[k] == '>'){
            printf(">");
            redirection = 1;
        }
        if (input[k] == '<'){
            printf("<");
            redirection = 2;
        }
    }*/

    return input;
}



char* get_read_file(char* arg)
{
    char* file = (char*)malloc(strlen(arg));
    int l = 0;
    while (arg[l] != '<' && arg[l] != '\0'){
        l++;
    }

    l++;
    if (arg[l-1] == '\0'){
        l = 0;
    }
    int curr = 0;
    while (arg[l] != '\0' && arg[l] != ' ' && arg[l] != '>' && arg[l] != '<'){
        file[curr] = arg[l];
        l++;
        curr++;
    }
    file[curr] = '\0';
    return file;
}

char* get_write_file(char* arg)
{
    char* file = (char*)malloc(strlen(arg));
    int l = 0;
    while (arg[l] != '>' && arg[l] != '\0'){
        l++;
    }
    l++;
    if (arg[l-1] == '\0'){
        l = 0;
    }
    int curr = 0;
    while (arg[l] != '\0' && arg[l] != ' ' && arg[l] != '>' && arg[l] != '<'){
        file[curr] = arg[l];
        l++;
        curr++;
    }
    file[curr] = '\0';
    return file;
}


/*
char* get_write_file(char* arg)
{
    char* file = (char*)malloc(strlen(arg));
    int l = 0;
    int curr = 0;
    while (arg[l] != '\0'){
        if (arg[l] != '>' && arg[l] != '<'){
            file[curr] = arg[l];
            l++;
            curr++;
        }
        else{
            l++;
        }
    }
    return file;
}
*/

int containsChar(const char *str, char c) {
    while (*str != '\0') { 
        if (*str == c) {    
            return 1;      
        }
        str++;              
    }
    return 0;            
}

void check_chars(char** current, int index){
    
        /*
        //printf("%s\n", current[1]);
    if (current[index+1][0] == '<') {
        printf("<");
        redirection = 1;
        if (current[index+1][1] != '\0'){
            read_file = get_read_file(current[index+1]);
        }
        else{
            read_file = current[index+2];
        }
    }
    if (current[index+1][0] == '>') {
        printf(">");
        redirection = 2;
        if (current[index+1][1] != '\0'){
            write_file = get_write_file(current[index+1]);
        }
        else{
            write_file = current[index+2];
    }
    }
    }
    */
    int k = 0;
    //printf("index: %d\n", index);
    while (current[index][k] != '\0'){
        if (current[index][k] == '<'){
            if (current[index][k+1] == '<'){
                redirection_error = 1;
                return;
            }
            //printf("<");
            if (redirection == 2){
                redirection = 4;
            }
            else{
                redirection = 1;
            }
            if (current[index][k+1] != '\0'){
                read_file = get_read_file(current[index]);
            }
            else{
                read_file = get_read_file(current[index+1]);
            }
        }
        if (current[index][k] == '>'){
            //printf(">");
            if (current[index][k+1] == '>'){
                redirection_error = 1;
                return;
            }
            if (redirection == 1){
                redirection = 3;
            }
            else{
            redirection = 2;
            }
            if (current[index][k+1] != '\0'){
                write_file = get_write_file(current[index]);
            }
            else{
                write_file = get_write_file(current[index+1]);

            }
        }
        k++;
    }
}


void find_and_char(char** current){
    int k = 0;
    for (int i = num_phrases-1; i > -1; i--){
        if (current[i] == NULL) {
            continue;
        }
        if (strcmp(current[i], "&") == 0){
            current[i] = NULL;
            background = 1;
            num_phrases--;
            break;
        }
        else{
        k = 0;
        while (current[i][k] != '\0'){
            if (current[i][k] == '&'){
                current[i][k] = '\0';
                background = 1;
                break;
            }
            k++;
        }
        }
    }
}

void sigchld_handler(int sig){
    int status;
    pid_t background_pid;
    if ((background_pid = waitpid(-1, &status, WNOHANG)) > 0){
        printf("Process %d is done\n", background_pid);
    }
}


int main(int argc, char *argv[])
{
    newline = 1;

    if (argc > 1 && strcmp(argv[1], "-n") == 0){
        newline = 0;
    }

    struct sigaction my_action;
    my_action.sa_handler = sigchld_handler;
    sigemptyset(&my_action.sa_mask);
    my_action.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &my_action, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }


    sigset_t block_set, old_set;
    sigemptyset(&block_set);  
    sigaddset(&block_set, SIGCHLD);


    while(1){    
        char user_input[512];
        char** parsed_input;
        int status;
        num_commands = 0;
        
        char* input = prompt();
        if (input == NULL){
            break;
        }
        strcpy(user_input, input);
        parsed_input = parse(user_input);
        
        //printf("Number of commands: %d\n", num_commands);  

        if (num_commands == 0){
            continue;
        }
        
        int my_pipes[num_commands-1][2];
        pid_t my_pid[num_commands];

        for (int i = 0; i < num_commands; i++){
            num_phrases = 0;
            redirection = 0;
            read_file = NULL;
            write_file = NULL;
            background = 0;
            redirection_error = 0;
            char** current = parse_command(parsed_input[i]);
            find_and_char(current);
            //printf("Command %d: %s\n", i, current[0]);  
            if (i < num_commands - 1){
                if (pipe(my_pipes[i]) == -1){
                    perror("Error in pipe(): ");
                    exit(1);
                }
            }
            
            for (int j = 0; j < num_phrases; j++){
                check_chars(current, j);
            }

            if (redirection_error)
            {
                printf("Error: Invalid redirection\n");
                break;
            }
            // printf("Redirection: %d\n", redirection);
            // printf("Read file: %s\n", read_file);
            // printf("Write file: %s\n", write_file);
            // fflush(stdout);
            if (sigprocmask(SIG_BLOCK, &block_set, &old_set) == -1) {
                perror("sigprocmask");
                exit(EXIT_FAILURE);
            }
            my_pid[i] = fork();
            if (my_pid[i] == -1){
                perror("Error in fork(): ");
                exit(1);
            }
            if (my_pid[i] != 0){
                // Parent process
                if (i != 0)
                {
                    close(my_pipes[i-1][0]);
                    close(my_pipes[i-1][1]);
                }
                if (background == 0){
                    waitpid(my_pid[i], &status, 0);
                }
                else{
                    //printf("Process %d is running in the background\n", my_pid[i]);
                    if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1) {
                        perror("sigprocmask");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else{
                // Child process
                if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1) {
                    perror("sigprocmask");
                    exit(EXIT_FAILURE);
                }

                if (redirection == 1 || redirection == 3 || redirection == 4) {
                    int input_file = open(read_file, O_RDONLY);
                    if (input_file == -1) {
                        perror("Error opening input file");
                        exit(1);
                    }
                    dup2(input_file, STDIN_FILENO);
                    close(input_file);
                } else if (i > 0) {
                    dup2(my_pipes[i-1][0], STDIN_FILENO);
                }

                if (redirection == 2 || redirection == 3 || redirection == 4) {
                    int output_file = open(write_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (output_file == -1) {
                        perror("Error opening output file");
                        exit(1);
                    }
                    dup2(output_file, STDOUT_FILENO);
                    close(output_file);
                } else if (i < num_commands - 1) {
                    dup2(my_pipes[i][1], STDOUT_FILENO);
                }

                if (i == 0){
                    //printf("i = %d\n", i);
                    close(my_pipes[0][0]);
                    close(my_pipes[0][1]);
                }
                else if (i == num_commands - 1){
                    //printf("i = %d\n", i);
                    close(my_pipes[i-1][0]);
                    close(my_pipes[i-1][1]);
                }
                else {
                    //printf("i = %d\n", i);
                    close(my_pipes[i-1][0]);
                    close(my_pipes[i-1][1]);
                    close(my_pipes[i][0]);
                    close(my_pipes[i][1]);
                }
                // for (int k = 0; k < num_phrases; k++){
                //     printf("Command %d: %s\n", k, current[k]);
                //     fflush(stdout);
                // }

                char **new_args = (char **)malloc(100 * sizeof(char *));
                int new_arg_index = 0;
                int j = 0;

                while (j < num_phrases) {
                    if ((strcmp(current[j], "<") == 0 || strcmp(current[j], ">") == 0) && j + 1 < num_phrases) {
                        j += 2; 
                        continue;
                    }                    
                    if ((read_file && strcmp(current[j], read_file) == 0) ||
                        (write_file && strcmp(current[j], write_file) == 0)) {
                        j++;
                        continue;
                    }

                    char* curr = (char*)malloc(100 * sizeof(char));
                    int k = 0;
                    while (current[j][k] != '\0' && current[j][k] != '<' && current[j][k] != '>' && k < 99) {
                        curr[k] = current[j][k];
                        k++;
                    }
                    curr[k] = '\0';
                    
                    if (curr[0] != ' ' && curr[0] != '\0') {
                        new_args[new_arg_index] = curr;
                        new_arg_index++;
                    } else {
                        free(curr);
                    }
                    
                    j++;
                }



                // if (redirection == 1 || redirection == 3 || redirection == 4){
                //     new_args[new_arg_index] = read_file;
                //     new_arg_index++;
                // }

                new_args[new_arg_index] = NULL;

                // Execute command
                // for (int j = 0; j < new_arg_index+1; j++){
                //     printf("Command %d: %s\n", j, new_args[j]);
                // }
                if (execvp(new_args[0], new_args) == -1) {
                    perror("Error executing command");
                    exit(1);
                }
                

                /*
                for (int j = 0; j < num_commands - 1; j++){
                    close(my_pipes[j][0]);
                    close(my_pipes[j][1]);
                }*/
                //char* args[] = {"cat", "output.txt", NULL};
                /*
                printf("Executing command: %s\n", current[0]);  // Debug print
                if (execvp(current[0], current) == -1){
                    perror("Error");
                    exit(1);
                }
                */
            }
        }
    }
    return 0;
}
