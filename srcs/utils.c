/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:53 by repichan          #+#    #+#             */
/*   Updated: 2026/08/22 12:30:57 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	print_log(t_params *params, int id, char *str)
{
	long long	now_time;

	pthread_mutex_lock(&params->print_mutex);
	if (!is_it_running(params))
	{
		pthread_mutex_unlock(&params->print_mutex);
		return (1);
	}
	now_time = get_time(params);
	fprintf(stdout, "%lld %d %s\n", now_time, id, str);
	pthread_mutex_unlock(&params->print_mutex);
	return (0);
}

int	is_it_running(t_params *params)
{
	int	running;

	if (pthread_mutex_lock(&params->state_mutex) != 0)
		return (0);
	running = params->is_running;
	pthread_mutex_unlock(&params->state_mutex);
	return (running);
}
