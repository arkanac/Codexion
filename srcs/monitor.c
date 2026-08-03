/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 18:45:19 by rem               #+#    #+#             */
/*   Updated: 2026/08/03 20:19:24 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int monitor(t_coder *coder, t_params *params)
{
    int i;
    

    i = 0;
    while(make_threads(&params))
    {
        while (&params->coders[i])
        {
            pthread_mutex_lock(&coder->mutex);
            if ((get_time(&params) - coder->last_compile_start) > &params->time_to_burnout)
            {
                print_log(&params, &coder->id, "burned out");
                
            }
            
        }
    }
}