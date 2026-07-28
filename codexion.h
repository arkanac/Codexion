/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:05:37 by rem               #+#    #+#             */
/*   Updated: 2026/07/28 16:24:46 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
#define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_params t_params;

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_params 
{
    int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
    t_scheduler		scheduler;
    
} t_params;

typedef struct s_dongle
{
	int	id;
	pthread_mutex_t    mutex;
	pthread_cond_t cond;
	int owner;
	int available_at;
} t_dongle;


int init_params(int ac, char **av, t_params *params);
int main(int ac, char **av);

#endif