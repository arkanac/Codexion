/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rem <rem@student.42lyon.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:05:37 by rem               #+#    #+#             */
/*   Updated: 2026/08/04 10:12:13 by rem              ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_params		t_params;
typedef struct s_dongle		t_dongle;
typedef struct s_coder		t_coder;

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
	long long		start_time;
	int				is_running;
	t_scheduler		scheduler;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	state_mutex;
	pthread_t		monitor;
}	t_params;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				id;
	int				owner;
	long long		available_at;
}	t_dongle;

typedef struct s_coder
{
	t_params			*params;
	pthread_t			thread;
	pthread_mutex_t		mutex;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	int					id;
	int					compile_count;
	long long			last_compile_start;
}	t_coder;

// Cleaning
void		clean_dongles(t_dongle *dongles, int count);
void		clean_coders(t_coder *coders, int count);
void		clean_global_mutex(t_params *params);
void		clean_all(t_params *params);

// Init
int			init_params(int ac, char *av[], t_params *params);
int			init_all(t_params *params);

// Time
long long	calculate_time(void);
long long	get_time(t_params *params);
struct timespec	get_future_timespec(long milliseconds);

// Log
void		print_log(t_params *params, int id, char *str);

// Threads
int			make_threads(t_params *params);

// Monitoring
void 		*monitor(void *arg);

//Coder action
int			coder_action(t_coder *coder);

//Dongle
int			take_dongle(t_coder *coder, t_dongle *dongle);
int			drop_dongle(t_coder *coder, t_dongle *dongle);

//Utils
void		sleep_until_ms(t_params *params, long long ms_duration);
int			is_it_running(t_params *params);

// Main
int			main(int ac, char **av);

#endif