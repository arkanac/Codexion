/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 09:27:50 by repichan          #+#    #+#             */
/*   Updated: 2026/08/12 13:31:59 by repichan         ###   ########.fr       */
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

int	make_threads(t_params *params)
{
	int	i;
	int	j;

	i = 0;
	while (i < params->number_of_coders)
	{
		if (pthread_create(&params->coders[i].thread,
				NULL, &routine, &params->coders[i]) != 0)
			return (1);
		i++;
	}
	pthread_create(&params->monitor, NULL, monitor, params);
	j = 0;
	while (j < params->number_of_coders)
	{
		if (pthread_join(params->coders[j].thread, NULL) != 0)
			return (1);
		j++;
	}
	pthread_join(params->monitor, NULL);
	return (0);
}
