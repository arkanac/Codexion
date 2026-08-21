/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 16:26:43 by repichan          #+#    #+#             */
/*   Updated: 2026/08/21 10:50:26 by rem              ###   ########lyon.fr   */
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
		return (0);
	}
	clean_all(&params);
	return (0);
}
