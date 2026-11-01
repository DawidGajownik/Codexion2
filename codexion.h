/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 16:23:42 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 18:29:31 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>

typedef enum e_request_state
{
	REQUEST_NONE,
	REQUEST_PENDING,
	REQUEST_GRANTED
}	t_request_state;

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
}	t_params;

typedef struct dongle
{
	int				id;
	int				free;
	long			locktime;
	pthread_cond_t	cond;
	pthread_mutex_t	*mutex;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				compiles_done;
	long			last_comp;
	pthread_t		thread;
	void			*args;
	t_dongle		*dongle_l;
	t_dongle		*dongle_r;
	t_request_state	request_state;
}	t_coder;

typedef struct s_request
{
	t_coder				*coder;
	unsigned long long	arrival_seq;
	long				deadline;
}	t_request;

typedef struct s_args
{
	t_params			*params;
	t_coder				**coder;
	t_dongle			*dongle;
	pthread_mutex_t		*mutex;
	long				timestart;
	pthread_mutex_t		arbiter_mutex;
	pthread_mutex_t		output_mutex;
	pthread_cond_t		dongles_cond;
	pthread_t			monitor_thread;
	int					stop;
	t_request			*requests;
	int					request_count;
	int					request_capacity;
	unsigned long long	next_arrival_seq;
}	t_args;

/* Argument handling and memory management */
int			args_valid(int argc, char **argv);
t_args		*create_args(char **argv);
void		free_all(t_args *args);

/* Thread and simulation lifecycle */
void		threads_start(t_args *args);
void		threads_finish(t_args *args);
void		*monitor_routine(void *arg);
void		*routine(void *arg);

/* Dongle assignment, locking, and cooldown */
void		match_dongles(t_coder *coder);
int			request_dongles(t_coder *coder);
void		lock_dongles(t_coder *coder);
void		unlock_dongles(t_coder *coder);
void		release_dongle_reservation(t_coder *coder);
int			cool_enough(t_coder *coder);
void		get_deadline(t_coder *coder, struct timespec *deadline);

/* Request queue and dispatch */
void		dispatch_locked(t_args *args);
int			add_request(t_coder *coder);
void		remove_request_at(t_args *args, int index);
int			request_has_higher_priority(t_args *args, t_request *a,
				t_request *b);
void		heap_sift_up(t_args *args, int index);

/* Coder phases and time */
int			compile(t_coder *coder);
void		debug(t_coder *coder);
void		refactor(t_coder *coder);
long		get_timestamp(void);
