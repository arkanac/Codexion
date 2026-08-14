/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 11:27:18 by repichan          #+#    #+#             */
/*   Updated: 2026/08/14 14:56:10 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	calculate_time(void)
{
	struct timeval	start_time;

	gettimeofday(&start_time, NULL);
	return ((long long)start_time.tv_sec * 1000 + start_time.tv_usec / 1000);
}

struct timespec	get_future_timespec(long milliseconds)
{
	struct timeval	now;
	struct timespec	ts;

	gettimeofday(&now, NULL);
	ts.tv_sec = now.tv_sec + (milliseconds / 1000);
	ts.tv_nsec = (long)now.tv_usec * 1000 + (milliseconds % 1000) * 1000000;
	ts.tv_sec += ts.tv_nsec / 1000000000;
	ts.tv_nsec %= 1000000000;
	return (ts);
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
