/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   params.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 14:55:57 by repichan          #+#    #+#             */
/*   Updated: 2026/08/22 12:27:00 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '\0')
		return (1);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (1);
		i++;
	}
	return (0);
}

static int	is_int(char *str)
{
	if (strlen(str) > 10)
		return (1);
	if (strcmp(str, "2147483647") > 0 && strlen(str) == 10)
		return (1);
	return (0);
}

static int	validation(int ac, char *av[])
{
	int	i;

	if (ac != 9)
		return (handle_error(ERR_NB_ARGS));
	i = 1;
	while (i < ac - 1)
	{
		if (is_number(av[i]) != 0)
			return (handle_error(ERR_ARGS));
		if (is_int(av[i]) != 0)
			return (handle_error(ERR_ARGS));
		i++;
	}
	if (strcmp(av[i], "fifo") != 0 && strcmp(av[i], "edf") != 0)
		return (handle_error(ERR_SCHEDULER));
	return (0);
}

int	init_params(int ac, char *av[], t_params *params)
{
	if (validation(ac, av) != 0)
		return (1);
	params->number_of_coders = atoi(av[1]);
	params->time_to_burnout = atoi(av[2]);
	params->time_to_compile = atoi(av[3]);
	params->time_to_debug = atoi(av[4]);
	params->time_to_refactor = atoi(av[5]);
	params->number_of_compiles_required = atoi(av[6]);
	params->dongle_cooldown = atoi(av[7]);
	if (strcmp(av[8], "fifo") == 0)
		params->scheduler = FIFO;
	else
		params->scheduler = EDF;
	params->is_running = 1;
	return (0);
}
