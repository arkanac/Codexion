/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 13:16:05 by repichan          #+#    #+#             */
/*   Updated: 2026/08/21 15:55:36 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	compile_count_checker(t_params *params, int nb)
{
	int	count;

	count = params->coders[nb].compile_count;
	if (count >= params->number_of_compiles_required)
		return (1);
	else
		return (0);
}

void	stop_all_coders(t_params *params)
{
	pthread_mutex_lock(&params->state_mutex);
	params->is_running = 0;
	pthread_mutex_unlock(&params->state_mutex);
}

static int	check_coder_status(t_params *params, int i, int *enough_compilation)
{
	long long	coder_time;
	int			finished;

	if (pthread_mutex_lock(&params->coders[i].mutex) != 0)
		return (1);
	coder_time = params->coders[i].last_compile_start;
	finished = compile_count_checker(params, i);
	*enough_compilation += finished;
	pthread_mutex_unlock(&params->coders[i].mutex);
	if ((finished == 0 && (get_time(params) - coder_time)
			> params->time_to_burnout) || params->number_of_coders == 1)
	{
		stop_all_coders(params);
		pthread_mutex_lock(&params->print_mutex);
		printf("%lld %d has burned out\n", get_time(params), params->coders[i].id);
		pthread_mutex_unlock(&params->print_mutex);
		return (1);
	}
	return (0);
}

void	*monitor(void *arg)
{
	t_params	*params;
	int			i;
	int			enough_compilation;

	params = (t_params *)arg;
	while (is_it_running(params))
	{
		i = 0;
		enough_compilation = 0;
		while (i < params->number_of_coders)
		{
			if (check_coder_status(params, i, &enough_compilation))
				return (NULL);
			i++;
		}
		if (enough_compilation == params->number_of_coders)
		{
			stop_all_coders(params);
			return (NULL);
		}
		usleep(10);
	}
	return (NULL);
}
