/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_parse.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfrade <dfrade@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 17:32:03 by csilva-m          #+#    #+#             */
/*   Updated: 2024/06/13 15:08:53 by dfrade           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd	*create_cmd_table(void)
{
	t_cmd	*commands;
	int		nb_of_cmds;

	nb_of_cmds = cmd_count();
	if (nb_of_cmds == 0)
		return (NULL);
	get_core()->cmd_table_len = nb_of_cmds;
	commands = malloc(nb_of_cmds * sizeof(t_cmd));
	if (commands == NULL)
		return (NULL);
	while (nb_of_cmds > 0)
	{
		commands[nb_of_cmds - 1].cmd = NULL;
		commands[nb_of_cmds - 1].args = NULL;
		commands[nb_of_cmds - 1].envp = NULL;
		commands[nb_of_cmds - 1].redir_in = NULL;
		commands[nb_of_cmds - 1].redir_out = NULL;
		commands[nb_of_cmds - 1].fork_pid = 0;
		commands[nb_of_cmds - 1].is_builtin = FALSE;
		nb_of_cmds--;
	}
	return (commands);
}

void save_last_redir_in(t_redir_in **redir)
{
    t_redir_in *cur;
	t_redir_in *next;
    if (!redir || !*redir)
		return;
    cur = *redir;
    while (cur->next)
    {
        next = cur->next;
        if (cur->tkn_type == HEREDOC)
        {
            close(cur->fd);
            unlink("heredoc_tmp");
        }
        else if (cur->tkn_type == TRUNC)
            close(cur->fd);
        free(cur->file_name);
        free(cur);
        cur = next;
    }
    *redir = cur;
}

void save_last_redir_out(t_redir_out **redir)
{
    t_redir_out *cur;
	t_redir_out *next;
    if (!redir || !*redir)
		return;
    cur = *redir;
    while (cur->next)
    {
        next = cur->next;
        if (cur->tkn_type == REDIRECT || cur->tkn_type == APPEND)
        {
            close(cur->fd);
            free(cur->file_name);
            free(cur);
        }
        cur = next;
    }
    *redir = cur;
}


void	fill_cmd_table(void)
{
	t_cmd	*cmd_table;
	t_token	*ptr_temp;
	int		nb_of_cmds;
	int		i;

	i = 0;
	nb_of_cmds = cmd_count();
	if (nb_of_cmds == 0)
		return ;
	get_core()->cmd_table = create_cmd_table();
	cmd_table = get_core()->cmd_table;
	if (cmd_table == NULL)
		return ;
	ptr_temp = get_core()->token;
	while (i < nb_of_cmds)
	{
		handle_redirects(&cmd_table[i]);
		save_last_redir_in(&cmd_table[i].redir_in);
		save_last_redir_out(&cmd_table[i].redir_out);
		cmd_table[i].cmd = ft_strdup(ptr_temp->str);
		cmd_table[i].args = cmd_to_matrix(&ptr_temp);
		cmd_table[i].envp = env_to_matrix();
		cmd_table[i].is_builtin = is_builtin(cmd_table[i].cmd);
		i++;
	}
}

t_bool	is_builtin(char *cmd)
{
	int			i;
	const char	*builtins[] = {
		"echo", "exit", "pwd", "unset", "export", "env", "cd", NULL
	};

	i = 0;
	while (builtins[i] != NULL)
	{
		if (ft_strncmp(cmd, builtins[i], ft_strlen(cmd)) == 0)
			return (TRUE);
		i++;
	}
	return (FALSE);
}

int	cmd_count(void)
{
	t_token	*list;
	int		i;

	list = get_core()->token;
	if (list == NULL)
		return (0);
	i = 1;
	while (list != NULL)
	{
		if (list->token == (int)PIPE)
			i++;
		list = list->next;
	}
	return (i);
}

int	malloc_len(char *key, char *value)
{
	return ((ft_strlen(key) + ft_strlen(value) + 2) * sizeof(char *));
}
