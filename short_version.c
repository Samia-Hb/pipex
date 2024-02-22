#include "pipex.h"

void ft_putstr(char *str) // done
{
	int i = 0;
	while(str[i])
	{
		write(1,&str[i],1);
		i++;
	}
}
char *find_path(char **envp) //done
{
    char *path_env = NULL;
    int i = 0;
    while(envp[i])
    {
        if(strncmp(envp[i], "PATH=", 5) == 0)
        {
            path_env = envp[i] + 5;
            break;
        }
        i++;
    }
    return path_env;
}

char *get_executable(char *command, char *path) //done
{
    int i = 0;
    char *full_path = NULL;
    char **dir = ft_split(path, ':');
    while (dir[i])
    {
        full_path = malloc(strlen(dir[i]) + strlen(command) + 2);
        if(full_path == NULL)
        {
            perror("malloc");
        	exit(1);
        }
        ft_strcpy(full_path, dir[i]);
        ft_strcat(full_path, "/");
        ft_strcat(full_path, command);
        if (access(full_path, X_OK) == 0)
        {
            break;
        }
        free(full_path);
        full_path = NULL;
        i++;
    }
    free(dir);
    return full_path;
}
char *execute_command(char *full_command, char **envp, char *file_name) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
    int pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        char **arr = ft_split(full_command, ' ');
        if (arr == NULL)
            exit(1);
        int arr_count = 0;
        while (arr[arr_count] != NULL)
            arr_count++;
        char **new_arr = malloc((arr_count + 2) * sizeof(char *));
        if (new_arr == NULL) {
            perror("malloc");
            exit(1);
        }
        int i = 0;
        while(i < arr_count)
        {
            new_arr[i] = arr[i];
            i++;
        }
        new_arr[arr_count] = file_name;
        new_arr[arr_count + 1] = NULL;
        char *command_path = get_executable(new_arr[0], find_path(envp));
        int ex = execve(command_path, new_arr, envp);
        if (ex == -1) {
            perror("execve");
            exit(1);
        }
    } else {
        // Parent process
        close(pipefd[1]);  // Close the write end of the pipe

        // Read from the read end of the pipe
        char buf[BUFSIZ];
        int nbytes = read(pipefd[0], buf, BUFSIZ);
        if (nbytes == -1)
        {
            perror("read");
            exit(1);
        }
        buf[nbytes] = '\0';
        // Copy the buffer to a new dynamically allocated buffer
        char *output = ft_strdup(buf);
        if (output == NULL) {
            perror("strdup");
            exit(1);
        }
        // Close the read end of the pipe
        close(pipefd[0]);
        wait(NULL);
        return output;
    }
    return NULL;
}

int main(int argc, char **argv, char **envp) {
    if (argc == 5)
    {
        if (access(argv[1], F_OK) == 0){
            char *buff = execute_command(argv[2], envp, argv[1]);
            if (buff == NULL)
            {
                fprintf(stderr, "Error executing command\n");
                return 1;
            }
            int fd = open("M_FILE.txt", O_CREAT | O_WRONLY | O_TRUNC, 0777);
            if (fd == -1) {
                perror("open");
                free(buff);
                return 1;
            }
            write(fd, buff, strlen(buff));
            close(fd);
            free(buff);

            buff = execute_command(argv[3], envp, argv[4]);
            if (buff == NULL)
            {
                fprintf(stderr, "Error executing command\n");
                return 1;
            }
            int fd1 = open(argv[4], O_CREAT | O_WRONLY | O_TRUNC, 0777);
            if(fd1 == -1)
            {
                perror("open_last_file");
                free(buff);
                return 1;
            }
            write(fd1, buff, strlen(buff));
            close(fd1);
            free(buff);
        } else {
            perror("access");
            return 1;
        }
    } else {
        fprintf(stderr, "Incorrect number of arguments.\n");
        return 1;
    }

    return 0;
}
