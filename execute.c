/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 05:57:09 by merilhan          #+#    #+#             */
/*   Updated: 2025/07/30 05:53:51 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "lexer/shell.h"

typedef struct s_exec_data
{
    int in_fd;
    int pipefd[2];
    int *pids;
    int i;
    int pids_size;
    char **env;
    t_env **env_list;
} t_exec_data;

void child_process_exec(t_parser *cmd, t_exec_data *data);

char *copy_dir(const char *dir, int len) 
{
    char *res;
    int i;

    i = 0;
    res = malloc(len + 1);
    if (!res)
        return NULL;
    while (i < len)
    {
        res[i] = dir[i];
        i++;
    }
    res[len] = '\0';
    return res;
}

char *join_path(char *dir, char *file)
{
    int dlen;
    int flen;
    char *res;
    int i;

    i = 0;
    dlen = ft_strlen(dir);
    flen = ft_strlen(file);
    res = malloc(dlen + flen + 2);
    if (!res)
        return (NULL);
    while(i < dlen)
    {
        res[i] = dir[i];
        i++;
    }    
     res[dlen] = '/';
    i = 0;
    while (i  < flen)
    {
     res[dlen + 1 + i] = file[i];
     i++;
    }
    res[dlen + i + 1] = '\0'; 
    return res;
}

int is_exec_other(char *path) 
{
    struct stat sb;
    if (stat(path, &sb) == -1)
        return (0);
    if (!S_ISREG(sb.st_mode)) 
        return (0);
    if ((sb.st_mode & S_IXOTH) == 0)
        return (0);
    return (1);
}

char *check_cmd_in_dir(const char *dir_start, int len, char *cmd)
{
    char *dir;
    char *result;

    dir = copy_dir((char *)dir_start, len); 
    if (!dir) 
        return NULL;

    result = join_path(dir, cmd);
    free(dir);

    if (is_exec_other(result))
        return result;

    free(result);
    return NULL;
}

char *search_in_path(char *cmd,char *path)
{
   char *finish;
    char *start;
    int len;
    char *found;

    start = path;
    finish = path;
    while (1) 
    {
        if (*start == ':' || *start == '\0') 
        {
            len = start - finish;
            if (len > 0) 
            {
                found = check_cmd_in_dir(finish, len, cmd);
                if (found)
                    return found;
            }
            if (*start == '\0')
                break;
            finish = start + 1;
        }
        start++;
    }
    return NULL;
}
char *find_executable(char *cmd)
{
    if (is_exec_other(cmd))
        return ft_strdup(cmd);
    char *path;
    path = getenv("PATH");
    if (!path) 
        return (NULL);
    return (search_in_path(cmd,path));
}

void redir_in(t_redirection *redir) 
{
    int fd;

    fd = open(redir->filename, O_RDONLY);
    if (fd < 0) 
    { perror("open <"); 
        exit(1); 
    }
    dup2(fd, 0);
    close(fd);
}

void redir_out(t_redirection *redir) 
{
    int fd;

    fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) 
    { 
        perror("open >"); 
        exit(1); 
    }
    dup2(fd, 1);
    close(fd);
}

void redir_append(t_redirection *redir) 
{
    int fd;

    fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (fd < 0) 
    { 
        perror("open >>"); 
        exit(1); 
    }
    dup2(fd, 1);
    close(fd);
}
char *remove_quotes_exec(char *str)
{
    if (!str)
        return NULL;
    
    int len = strlen(str);
    if (len == 0)
        return str;
    
    // Önce quote'ların doğru eşleşip eşleşmediğini kontrol et
    int i = 0;
    char in_quote = 0;
    int quote_count_single = 0;
    int quote_count_double = 0;
    
    // Quote sayısını ve eşleşmesini kontrol et
    while (i < len)
    {
        if (str[i] == '"' && in_quote != '\'')
        {
            if (in_quote == '"')
                in_quote = 0;
            else if (in_quote == 0)
                in_quote = '"';
            quote_count_double++;
        }
        else if (str[i] == '\'' && in_quote != '"')
        {
            if (in_quote == '\'')
                in_quote = 0;
            else if (in_quote == 0)
                in_quote = '\'';
            quote_count_single++;
        }
        i++;
    }
    
    // Eğer quote açık kalmışsa veya tek sayıda quote varsa hata
    if (in_quote != 0 || (quote_count_single % 2 != 0) || (quote_count_double % 2 != 0))
    {
        printf("bash: syntax error: unclosed quotes\n");
        return NULL;
    }
    
    // Quote'ları kaldır
    char *result = malloc(len + 1);
    if (!result)
        return str;
    
    i = 0;
    int j = 0;
    in_quote = 0;
    
    while (i < len)
    {
        char current = str[i];
        
        if (current == '"' || current == '\'')
        {
            if (in_quote == 0)
            {
                in_quote = current;
            }
            else if (in_quote == current)
            {
                in_quote = 0;
            }
            else
            {
                result[j++] = current;
            }
        }
        else
        {
            result[j++] = current;
        }
        i++;
    }
    
    result[j] = '\0';
    
    if (j == len)
    {
        free(result);
        return str;
    }
    
    return result;
}


void setup_redirections(t_redirection *redirs)
{
    while (redirs) 
    {
        char *filename = remove_quotes_exec(redirs->filename);
        
        if (redirs->type == REDIR_IN)
        {
            int fd = open(filename, O_RDONLY);
            if (fd < 0) 
            { 
                perror("open <"); 
                if (filename != redirs->filename)
                    free(filename);
                exit(1); 
            }
            dup2(fd, 0);
            close(fd);
        }
        else if (redirs->type == REDIR_OUT)
        {
            int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if (fd < 0) 
            { 
                perror("open >"); 
                if (filename != redirs->filename)
                    free(filename);
                exit(1); 
            }
            dup2(fd, 1);
            close(fd);
        }
        else if (redirs->type == REDIR_APPEND)
        {
            int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0777);
            if (fd < 0) 
            { 
                perror("open >>"); 
                if (filename != redirs->filename)
                    free(filename);
                exit(1); 
            }
            dup2(fd, 1);
            close(fd);
        }
        
        if (filename != redirs->filename)
            free(filename);
            
        redirs = redirs->next;
    }
}
int ft_builtin_2(t_parser *cmd, t_env **env_list)
{
    (void)env_list; // echo için env_list gerekmiyor
    if (ft_strcmp(cmd->argv[0], "echo") == 0) 
    {
       int saved_stdout = -1;
       if (cmd->redirs) 
       {
           saved_stdout = dup(1);
           setup_redirections(cmd->redirs);
       }
       built_echo(cmd);
       if (saved_stdout != -1) 
       {
           dup2(saved_stdout, 1);
           close(saved_stdout);
       }
       return 1;
    }
    return (0);
}

int ft_builtin(t_parser *cmd, t_env **env_list)
{
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return 0;
        
    if (ft_strcmp(cmd->argv[0], "exit") == 0)
    {
        builtin_exit(cmd);
        return 1;
    }
    if (ft_strcmp(cmd->argv[0], "cd") == 0) 
    {
        g_last_exit_status = built_cd(cmd, env_list); 
        return 1;
    }
    if (ft_strcmp(cmd->argv[0], "pwd") == 0)
    {
        g_last_exit_status = builtin_pwd(); 
        return 1;
    }
    if(ft_strcmp(cmd->argv[0], "export") == 0)
    {
        g_last_exit_status = builtin_export(cmd, env_list);
        return 1;
    }
    if(ft_strcmp(cmd->argv[0], "unset") == 0)
    {
        g_last_exit_status = builtin_unset(cmd, env_list);
        return 1;
    }
    if(ft_strcmp(cmd->argv[0], "env") == 0)
    {
        g_last_exit_status = builtin_env(cmd, *env_list);
        return 1;
    }
    if (ft_builtin_2(cmd, env_list))
        return 1;
    return 0;
}

int count_commands(t_parser *cmd)
{
    int count = 0;
    while (cmd) 
    {
        count++;
        cmd = cmd->next;
    }
    return count;
}

void child_process_exec(t_parser *cmd, t_exec_data *data)
{
    char *exec_path;

    // Child process için signal'ları default'a çeviricez önemli
    setup_child_signals();

    if (data->in_fd != 0) 
    {
        dup2(data->in_fd, 0);
        close(data->in_fd);
    }
    if (cmd->next) 
    {
        dup2(data->pipefd[1], 1);
        close(data->pipefd[0]);
        close(data->pipefd[1]);
    }
    setup_redirections(cmd->redirs);
    exec_path = find_executable(cmd->argv[0]);
    if (!exec_path) 
    {
        fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
        exit(127);
    }
    execve(exec_path, cmd->argv, data->env);
    perror("execve");
    exit(1);
}

int setup_pipe_if_needed(t_parser *cmd, t_exec_data *data)
{
    if (!cmd->next) 
        return 0;
    if (pipe(data->pipefd) == -1) 
    {
        perror("pipe");
        return -1;
    }
    return (0);
}

int fork_pipe_exec(t_parser *cmd, t_exec_data *data)
{
    int pid;
    
    pid = fork();
    if (pid == -1) 
    {
        perror("fork");
        return -1;
    }
    if (pid == 0) 
    {
        child_process_exec(cmd, data);
        exit(1);
    }
    return pid;
}

int setup_and_fork(t_parser *cmd, t_exec_data *data)
{
    int pid;

    if (setup_pipe_if_needed(cmd, data) == -1)
        return -1;
    pid = fork_pipe_exec(cmd, data);
    if (pid == -1)
        return -1;
    if (data->i >= data->pids_size) 
    {
        fprintf(stderr, "Too many commands to execute\n");
        return -1;
    }
    data->pids[data->i++] = pid;
    return pid;
}

int process_command(t_parser *cmd, t_exec_data *data)
{
    if (!cmd->argv || !cmd->argv[0])
        return (0);

    if (ft_builtin(cmd, data->env_list))
    {
        return (1); 
    }
    else 
    {
        if (setup_and_fork(cmd, data) == -1) 
        {
            free(data->pids);
            exit(1);
        }
        if (data->in_fd != 0)
            close(data->in_fd);
        if (cmd->next)
         {
            close(data->pipefd[1]);
            data->in_fd = data->pipefd[0];
        }
         else 
            data->in_fd = 0;
        return 1;
    }
}

void execute_loop(t_parser *cmd_list, t_exec_data *data)
{
    data->in_fd = 0;
    t_parser *cmd;
    
    cmd = cmd_list;
    while (cmd)
    {
        if (!process_command(cmd, data))
            break;
        cmd = cmd->next;
    }
}

void wait_pids(t_exec_data *data)
{
    int j = 0;
    int last_status = 0;
    
    while (j < data->i) 
    {
        int status;
        waitpid(data->pids[j], &status, 0);
        
        if (WIFEXITED(status)) 
        {
            last_status = WEXITSTATUS(status);
        } 
        else if (WIFSIGNALED(status)) 
        {
            last_status = 128 + WTERMSIG(status);
        }
        j++;
    }
    
    g_last_exit_status = last_status;
}

void execute_cmds(t_parser *cmd_list, char **env, t_env **env_list)
{
    t_exec_data data;
    data.pids_size = count_commands(cmd_list);
    data.pids = malloc(sizeof(int) * data.pids_size);
    if (!data.pids) 
    { 
        perror("malloc");
        exit(1); 
    }
    data.i = 0;
    data.env = env;
    data.env_list = env_list;  
    data.in_fd = 0;
    
    execute_loop(cmd_list, &data);
    wait_pids(&data);
    free(data.pids);
}