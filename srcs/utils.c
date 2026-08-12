/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:53 by repichan          #+#    #+#             */
/*   Updated: 2026/08/12 11:53:48 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_log(t_params *params, int id, char *str)
{
	long long	now_time;

	now_time = get_time(params);
	pthread_mutex_lock(&params->print_mutex);
	fprintf(stdout, "%lld %d %s\n", now_time, id, str);
	pthread_mutex_unlock(&params->print_mutex);
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

int	not_in_queue(t_coder *coder, t_dongle *dongle)
{
	if (dongle->queue[0].id != coder->id && dongle->queue[1].id != coder->id)
		return(0);
	return (1);
}

