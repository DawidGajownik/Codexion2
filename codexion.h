/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 16:23:42 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/04 17:46:32 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>


typedef struct s_coder
{
	int				id;
	pthread_t		thread;
}	t_coder;


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

long    get_timestamp();
int     args_valid(int argc, char **argv);
void checkers(int argc, char **argv, long timestart);

