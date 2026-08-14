/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: repichan <repichan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 14:56:24 by repichan          #+#    #+#             */
/*   Updated: 2026/08/14 16:30:28 by repichan         ###   ########.fr       */
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
typedef struct s_queue		t_queue;
typedef struct timespec		t_timespec;

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef enum e_error
{
	ERR_ARGS,
	ERR_NB_ARGS,
	ERR_SCHEDULER,
	ERR_MALLOC,
	ERR_MUTEX_INIT,
	ERR_THREAD_INIT,
	ERR_COND_INIT,
	ERR_THREAD_JOIN
}	t_error;

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
	t_queue			*queue;
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

typedef struct s_queue
{
	int			id;
	long long	last_compile_start;
}	t_queue;

// Cleaning
void		clean_dongles(t_dongle *dongles, int count);
void		clean_coders(t_coder *coders, int count);
void		clean_global_mutex(t_params *params);
void		clean_all(t_params *params);

//Error
int			handle_error(t_error code);

// Init
int			init_params(int ac, char *av[], t_params *params);
int			init_all(t_params *params);

// Time
long long	calculate_time(void);
long long	get_time(t_params *params);
void		sleep_until_ms(t_params *params, long long ms_duration);
t_timespec	get_future_timespec(long milliseconds);
// Log
int		print_log(t_params *params, int id, char *str);

// Threads
int			make_threads(t_params *params);

// Monitoring
void		*monitor(void *arg);
void		stop_all_coders(t_params *params);

//Coder action
int			coder_action(t_coder *coder);

//Dongle
int 		take_dongles(t_coder *coder);
int			take_dongle(t_coder *coder, t_dongle *dongle);
int			drop_dongle(t_coder *coder, t_dongle *dongle);

//Scheduler
int			scheduler(t_params *params, t_coder *coder, t_dongle *dongle);
int			add_to_queue(t_coder *coder, t_dongle *dongle);
int			remove_from_queue(t_coder *coder, t_dongle *dongle);

//Utils
int			is_it_running(t_params *params);
int			not_in_queue(t_coder *coder, t_dongle *dongle);

// Main
int			main(int ac, char **av);

#endif