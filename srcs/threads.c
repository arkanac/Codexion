/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:50 by repichan          #+#    #+#             */
/*   Updated: 2026/08/14 16:21:13 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	coder_action(coder);
	return (NULL);
}

int thread_failsafe(t_params *params, int nb)
{
	int i;

	i = 0;
	stop_all_coders(params);
	while (i < nb)
	{
		if (pthread_join(params->coders[i].thread, NULL) != 0)
			return (1);
		i++;
	}
	return(0);
}

int	make_threads(t_params *params)
{
	int	i;
	int	j;

	i = 0;
	while (i < params->number_of_coders)
	{
		if (pthread_create(&params->coders[i].thread,
				NULL, &routine, &params->coders[i]) != 0)
		{
			thread_failsafe(params, i);
			return (handle_error(ERR_THREAD_INIT));
		}	
		i++;
	}
	if (pthread_create(&params->monitor, NULL, monitor, params) != 0)
		return(1);
	j = 0;
	while (j < params->number_of_coders)
	{
		if (pthread_join(params->coders[j].thread, NULL) != 0)
			return (handle_error(ERR_THREAD_JOIN));
		j++;
	}
	if (pthread_join(params->monitor, NULL) != 0)
		return (1);
	return (0);
}
