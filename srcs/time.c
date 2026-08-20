/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 11:27:18 by repichan          #+#    #+#             */
/*   Updated: 2026/08/20 16:27:47 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	calculate_time(void)
{
	struct timeval	start_time;

	gettimeofday(&start_time, NULL);
	return ((long long)start_time.tv_sec * 1000 + start_time.tv_usec / 1000);
}

long long	get_time(t_params *params)
{
	return (calculate_time() - params->start_time);
}

void	sleep_until_ms(t_params *params, long long ms_duration)
{
	long long	start;

	start = get_time(params);
	while (get_time(params) - start < ms_duration && is_it_running(params))
		usleep(500);
}
