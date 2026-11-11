/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 15:29:46 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	all_coders_done(t_args *args)
{
	int	i;

	i = 0;
	while (i < args->params->number_of_coders)
	{
		if (args->coder[i]->compiles_done
			< args->params->number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

static int	check_burnout(t_args *args, long now)
{
	int	i;

	i = 0;
	while (i < args->params->number_of_coders)
	{
		if (args->coder[i]->compiles_done
			< args->params->number_of_compiles_required
			&& now - args->coder[i]->last_comp
			>= args->params->time_to_burnout)
		{
			pthread_mutex_lock(&args->output_mutex);
			printf("%ld %d burned out\n", now - args->timestart,
				args->coder[i]->id);
			pthread_mutex_unlock(&args->output_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

static void	finish_monitor(t_args *args)
{
	args->stop = 1;
	pthread_cond_broadcast(&args->dongles_cond);
	pthread_mutex_unlock(&args->arbiter_mutex);
}

void	*monitor_routine(void *arg)
{
	t_args	*args;
	long	now;

	args = arg;
	while (1)
	{
		usleep(1000);
		pthread_mutex_lock(&args->arbiter_mutex);
		now = get_timestamp();
		if (all_coders_done(args)
			|| check_burnout(args, now))
		{
			finish_monitor(args);
			return (NULL);
		}
		pthread_mutex_unlock(&args->arbiter_mutex);
	}
}
