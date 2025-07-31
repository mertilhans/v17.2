/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: merilhan <merilhan@42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 03:36:25 by husarpka          #+#    #+#             */
/*   Updated: 2025/07/31 14:49:09 by merilhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

t_tokenizer *tokenizer_init(char *input)
{
    t_tokenizer *tokenizer;
    
    if (!input)
    return (NULL);
    
    tokenizer = malloc(sizeof(t_tokenizer));
    if (!tokenizer)
    return (NULL);
    
    tokenizer->input = ft_strdup(input);
    if (!tokenizer->input)
    {
        free(tokenizer);
        return (NULL);
    }
    
    tokenizer->pos = 0;
    tokenizer->len = ft_strlen(input);
    
    if(tokenizer->len > 0)
    tokenizer->current = input[0];
    else
    tokenizer->current = '\0';  
    return (tokenizer);
}

t_token *create_token(t_token_type type, char *value)
{
    t_token *token;
    
    token = malloc(sizeof(t_token));
    if (!token)
    return (NULL);
    
    token->type = type;
    if (value)
        token->value = ft_strdup(value);
    else
        token->value = NULL;
    token->next = NULL;
    return (token);
}


void print_tokens(t_token *tokens)
{
    t_token *current = tokens;
    const char *type_names[] = {
        "WORD", "PIPE", "REDIR_IN", "REDIR_OUT", "REDIR_APPEND",
        "HEREDOC", "AND", "OR", "SEMICOLON", "LPAREN", "RPAREN",
        "EOF", "ERROR"
    };
    
    printf("=== TOKENS ===\n");
    while (current)
    {
        printf("Type: %-12s Value: '%s'\n", 
               type_names[current->type], 
               current->value ? current->value : "NULL");
        current = current->next;
    }
    printf("==============\n");
}

void ft_error_syntax(t_token *token)
{
    printf("---tokenlerrrrrrrr(%c)---",token->value[0]);
    printf("---tokenlerrrrrrrr(%c)---",token->value[1]);
    printf("---tokenlerrrrrrrr(%c)---",token->value[2]);

    
    printf("token 1 %c \n token 2%c \n",token->value[0],token->value[1]);
    if (token->value[0] == '|')
        printf("bash: syntax error near unexpected token `|'\n");
    else if (token->value[0] == '>' && token->value[1] == '<')
        printf("bash: syntax error near unexpected token `<'\n");
    else if (token->value[0] == '<' && token->value[1] == '>')
        printf("bash: syntax error near unexpected token `newline'\n");
    else if (token->value[0] == '>' || token->value[0] == '<')
        printf("bash: syntax error near unexpected token `newline'\n");
        
}

int red_len(char *str)
{
    int i = 0;
    while(str[i])
        i++;
    return (i);
}

int ft_ctrl(char *str)
{
    int i = 0;
    if (str[i] && str[i + 1])
    {
        if (str[i] == '>' && str[i + 1] == '<')
            i++;
    }
    while(str[i] == '<')
        i++;
    if(str[i] == '\0')
        return (1);
    else
        return (0);
}

int ft_ctrl_2(char *str)
{
    int i = 0;
    if (str[i] && str[i + 1])
    {
        if (str[i] == '<' && str[i + 1] == '>')
            i = 1;
    }
    else
        return (i);
    return (i);
}
int ft_ctr_3(char *str)
{
	int i = 0;
	str++;
	str++;
	if (str[i] == '<' && str[i + 1] == '>')
		return (1);
	else 
		return (i);
	return (0);
}

int ft_alphabet(char c)
{
	if (c >= 'a' && c <= 'z')
		return (1);
	else if (c >= 'A'  && c <= 'Z')
		return (1);
	else 
		return (0);
}
void ft_error_2(char *str)
{
	int i = 0;
	if (str[i] == '~')
		printf("bash: /home/husarpka: Is a directory\n");
}

void error(char *str)
{
    int i;
    int a = 0;

    i = 0;
    char *res = ft_strdup(str);
    while(res)
    {
        if (*res == 32)
            res++;
        else 
            break;
    }
    if (res[i] == '|')
        printf("bash: syntax error near unexpected token `|'\n");
    else if (res[i] == '>' && res[i + 1] == '<')
    {
        a = red_len(str);
        if (ft_ctrl(str))
        {
            printf("bash: syntax error near unexpected token `");
            if (a > 3)
                a = 4;
            while(--a)
                printf("<");
        }
        str++;
        if (ft_ctrl_2(str))
        {

           printf("bash: syntax error near unexpected token `<>'");
           
        }
    
    
        printf("\n");
        
        
        
    }
        //printf("bash: syntax error near unexpected token `<'\n");
    else if (res[i] == '<' && res[i + 1] == '>' )
	{
		if (ft_ctr_3(str))
			printf("bash: syntax error near unexpected token `<>'\n");
		else
        	printf("bash: syntax error near unexpected token `newline'\n");
		
	}
	else if (res[i] == '<' && ft_alphabet(res[i + 1]))
	{
		printf("bash: %c: No such file or directory\n",res[i + 1]);
	}
    else if (res[i] == '<' || res[i] == '>')
        printf("bash: syntax error near unexpected token `newline'\n");
    ft_error_2(res);
	    //printf("%s \n",res);
        
}
int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}
char	*ft_strchr(const char *s, int c)
{
	while (*s != '\0')
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return (NULL);
}

// int is_pure_variable_expansion(char *input)
// {
//     if (!input)
//         return 0;
//     if(input[0] != '$')
//         return 0;
//     if(ft_strchr(input,'|') || ft_strchr(input,'<') || ft_strchr(input,'>'))
//         return 0;
    
//     // $ dan sonra geçerli değişken karakteri var mı kontrol et
//     int i = 1;
//     if (!is_valid_char(input[i]) && input[i] != '?' && input[i] != '$')
//         return 0; // $ dan sonra geçersiz karakter varsa expansion değil
        
//     return 1;
// }

int is_only_variable_name(char *input)
{
    if (!input || input[0] != '$')
        return 0;
    
    // Boşluk kontrolü - tek word olmalı
    if (ft_strchr(input, ' ') || ft_strchr(input, '\t'))
        return 0;
    
    int i = 1;
    if (input[i] == '?' && input[i+1] == '\0')
        return 1;
    if (input[i] == '$' && input[i+1] == '\0')
        return 1;
    if (!((input[i] >= 'a' && input[i] <= 'z') || 
          (input[i] >= 'A' && input[i] <= 'Z') || 
          input[i] == '_'))
        return 0;
    i++;
    while (input[i])
    {
        if (!((input[i] >= 'a' && input[i] <= 'z') || 
              (input[i] >= 'A' && input[i] <= 'Z') || 
              (input[i] >= '0' && input[i] <= '9') || 
              input[i] == '_'))
            return 0;
        i++;
    }
    return 1;
}

void process_and_execute(char *input, char **env, t_env *env_list)
{
    if (!input || strlen(input) == 0)
        return;
    check_and_handle_signal();
    
    // Debug: Gelen input'u yazdır
    printf("DEBUG: Input = '%s'\n", input);
    
    // Sadece değişken adı mı kontrol et (örn: $USER, $NABER, $?, $$)
    if (is_only_variable_name(input))
    {
        printf("DEBUG: Only variable name detected\n");
        
        if (input[1] == '?')
        {
            printf("%d: command not found\n", g_last_exit_status);
            g_last_exit_status = 127;
            return;
        }
        if (input[1] == '$')
        {
            printf("%d: command not found\n", getpid());
            g_last_exit_status = 127;
            return;
        }
        
        char *var_name = input + 1; // $ işaretini atla
        t_env *env_var = find_env(env_list, var_name);
        
        printf("DEBUG: Looking for variable '%s'\n", var_name);
        
        if (env_var && env_var->value && strlen(env_var->value) > 0)
        {
            printf("DEBUG: Found variable value: '%s'\n", env_var->value);
            printf("%s: command not found\n", env_var->value);
        }
        else
        {
            printf("DEBUG: Variable not found or empty\n");
            printf(": command not found\n");
        }
        g_last_exit_status = 127;
        return;
    }
    
    // Variable expansion içeren komutları çalıştır
    if (ft_strchr(input, '$'))
    {
        printf("DEBUG: Variable expansion detected\n");
        
        // Expansion yap ve sonucu komut olarak çalıştır
        char *expanded = expand_with_quotes(input, env_list);
        
        printf("DEBUG: Expanded result = '%s'\n", expanded ? expanded : "NULL");
        
        if (expanded && ft_strlen(expanded) > 0)
        {
            printf("DEBUG: Recursively calling with expanded string\n");
            // Expanded string'i yeni bir komut olarak işle
            process_and_execute(expanded, env, env_list);
            free(expanded);  // gc_free yerine free kullan
            return;
        }
        else
        {
            if (expanded)
                free(expanded);  // gc_free yerine free kullan
            printf(": command not found\n");
            g_last_exit_status = 127;
            return;
        }
    }
    
    printf("DEBUG: Normal command processing\n");
    
    t_token *tokens = tokenize_input(input);
    if (!tokens)
    {
        printf("tokenization failed\n");
        return;
    }
    
    t_parser *cmd_list = parse_tokens(tokens);
    if (!cmd_list)
    {
        gc_free(tokens);
        return;
    }
    
    expand_parser_list(cmd_list, env_list);
    
    execute_cmds(cmd_list, env, &env_list);
    
    gc_free(tokens);
    gc_free(cmd_list);
}
int main(int ac , char **av, char **env)
{
	(void)ac;
	(void)av;
	
	
	t_env *env_list = init_env(env);
    
    setup_interactive_signals();
	
	char *line;
	
	while(1)
	{
        check_and_handle_signal();
		line = readline("MiniShell->>>   ");
		if (!line)
        {   
            printf("exit\n");
            break;
        }
        if(ft_strlen(line) == 0)
        {
            free(line);
            continue;
        }
        add_history(line);
		
		// Direkt git
		process_and_execute(line, env,env_list);
		
		free(line);
	}
    
    return (0);
}
