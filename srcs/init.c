/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:42 by repichan          #+#    #+#             */
/*   Updated: 2026/08/19 00:00:08 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongle(int id, t_dongle *dongle)
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

int	init_coder(int i, t_coder *coder, t_dongle *dongles, t_params *params)
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
	int	nb;

	if (pthread_mutex_init(&params->print_mutex, NULL) != 0)
		return (handle_error(ERR_MUTEX_INIT));
	if (pthread_mutex_init(&params->state_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&params->print_mutex);
		return (handle_error(ERR_MUTEX_INIT));
	}
	if (pthread_mutex_init(&params->start_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&params->print_mutex);
		pthread_mutex_destroy(&params->state_mutex);
		return (handle_error(ERR_MUTEX_INIT));
	}
	if (pthread_cond_init(&params->start_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&params->print_mutex);
		pthread_mutex_destroy(&params->state_mutex);
		pthread_mutex_destroy(&params->start_mutex);
		return (handle_error(ERR_COND_INIT));
	}
	params->is_ready = 0;
	params->start_time = calculate_time();
	nb = params->number_of_coders;
	params->dongles = create_dongles(nb);
	if (!params->dongles)
	{
		clean_global_mutex(params);
		return (1);
	}
	params->coders = create_coders(nb, params->dongles, params);
	if (!params->coders)
	{
		clean_global_mutex(params);
		clean_dongles(params->dongles, nb);
		return (1);
	}
	return (0);
}
