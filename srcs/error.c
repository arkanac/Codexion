/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 14:06:43 by repichan          #+#    #+#             */
/*   Updated: 2026/08/14 15:30:58 by repichan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	handle_error(t_error code)
{
	if (code == ERR_ARGS)
		fprintf(stderr, "Error: invalid arguments,"
				"must be a positive integer\n");
	if (code == ERR_NB_ARGS)
		fprintf(stderr, "Error: invalid number "
				"of arguments\n");
	else if (code == ERR_SCHEDULER)
		fprintf(stderr, "Error: scheduler must be fifo or edf\n");
	else if (code == ERR_MALLOC)
		fprintf(stderr, "Error: memory allocation failed\n");
	else if (code == ERR_MUTEX_INIT)
		fprintf(stderr, "Error: mutex initialization failed\n");
	else if (code == ERR_THREAD_INIT)
		fprintf(stderr, "Error: thread initialization failed\n");
	else if (code == ERR_COND_INIT)
		fprintf(stderr, "Error: condition variable initialization failed\n");\
	else if (code == ERR_THREAD_JOIN)
		fprintf(stderr, "Error: thread join failed\n");
	return (1);
}
