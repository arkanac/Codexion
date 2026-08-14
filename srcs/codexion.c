/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:21:35 by rem               #+#    #+#             */
/*   Updated: 2026/08/14 15:34:28 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int ac, char **av)
{
	t_params	params;

	if (init_params(ac, av, &params))
		return (1);
	if (init_all(&params) != 0)
		return (1);
	if (make_threads(&params) != 0)
	{
		clean_all(&params);
		return(0);
	}
	clean_all(&params);
	return (0);
}
