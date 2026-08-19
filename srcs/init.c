/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:42 by repichan          #+#    #+#             */
/*   Updated: 2026/08/19 18:26:32 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongle(int id, t_dongle *dongle)
{
	dongle->id = id;
	dongle->owner = -1;
	dongle->available_at = 0;
	dongle->queue[0] = 0;
	dongle->queue[1] = 0;
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (handle_error(ERR_MUTEX_INIT));
	return (0);
}

t_dongle	*create_dongles(int nb)
{
	int			i;
	t_dongle	*dongles;

	i = 0;
	dongles = malloc(sizeof(t_dongle) * (nb));
	if (!dongles)
	{
		handle_error(ERR_MALLOC);
		return (NULL);
	}
	while (i < nb)
	{
		if (init_dongle(i, &dongles[i]) != 0)
		{
			clean_dongles(dongles, i);
			return (NULL);
		}
		i++;
	}
	return (dongles);
}

static int	init_coder(int i, t_coder *coder, t_dongle *dongles,
						t_params *params)
{
	int	id;
	int	nb;

	id = i + 1;
	nb = params->number_of_coders;
	if (pthread_mutex_init(&coder->mutex, NULL) != 0)
		return (handle_error(ERR_MUTEX_INIT));
	coder->id = id;
	coder->compile_count = 0;
	coder->last_compile_start = 0;
	coder->left_dongle = &dongles[i];
	coder->right_dongle = &dongles[(i + 1) % nb];
	coder->params = params;
	coder->requesting = 0;
	coder->request_time = 0;
	return (0);
}

t_coder	*create_coders(int nb, t_dongle *dongles, t_params *params)
{
	int		i;
	t_coder	*coders;

	i = 0;
	coders = malloc(sizeof(t_coder) * (nb));
	if (!coders)
	{
		handle_error(ERR_MALLOC);
		return (NULL);
	}
	while (i < nb)
	{
		if (init_coder(i, &coders[i], dongles, params) != 0)
		{
			clean_coders(coders, i);
			return (NULL);
		}
		i++;
	}
	return (coders);
}

int	init_all(t_params *params)
{
	if (init_mutexes(params) != 0)
		return (1);
	if (init_params_struct(params) != 0)
		return (1);
	return (0);
}
