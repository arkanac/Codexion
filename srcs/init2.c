/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 16:27:03 by repichan          #+#    #+#             */
/*   Updated: 2026/08/20 16:27:04 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "codexion.h"

int	init_mutexes(t_params *params)
{
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
		clean_global_mutex(params);
		return (handle_error(ERR_COND_INIT));
	}
	return (0);
}

int	init_params_struct(t_params *params)
{
	int	nb;

	nb = params->number_of_coders;
	params->is_ready = 0;
	params->start_time = calculate_time();
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
