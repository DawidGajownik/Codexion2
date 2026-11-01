/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/31 12:32:00 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	should_stop_after_compile(t_coder *coder)
{
	t_args	*args;
	int		stop;

	args = coder->args;
	pthread_mutex_lock(&args->arbiter_mutex);
	stop = args->stop
		|| coder->compiles_done
		>= args->params->number_of_compiles_required;
	pthread_mutex_unlock(&args->arbiter_mutex);
	return (stop);
}

static int	simulation_stopped(t_args *args)
{
	int	stop;

	pthread_mutex_lock(&args->arbiter_mutex);
	stop = args->stop;
	pthread_mutex_unlock(&args->arbiter_mutex);
	return (stop);
}

void	*routine(void *arg)
{
	t_coder	*coder;
	t_args	*args;

	coder = arg;
	args = coder->args;
	while (1)
	{
		if (!compile(coder) || should_stop_after_compile(coder))
			break ;
		debug(coder);
		if (simulation_stopped(args))
			break ;
		refactor(coder);
	}
	return (NULL);
}
