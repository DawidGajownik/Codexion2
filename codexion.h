/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 16:23:42 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/10 17:32:15 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>





typedef struct s_params
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	char*           scheduler;
}	t_params;


typedef struct dongle
{
	int id;
	int free;
	pthread_mutex_t *mutex;
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
}	t_coder;


typedef struct s_args
{
	t_params *params;
	t_coder	**coder;
	t_dongle *dongle;
	pthread_mutex_t *mutex;
	pthread_mutex_t *mutex_ckecker;
	long timestart;
}	t_args;

void    threads_start(t_args *args);
void    threads_finnish(t_args *args);
long    get_timestamp();
int     args_valid(int argc, char **argv);
void checkers(int argc, char **argv, long timestart);
t_args* set_args(char **argv);
t_params* set_params(char **argv);
void free_all(t_args *args);
void* routine(void* arg);

