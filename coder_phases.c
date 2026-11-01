/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_phases.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 12:32:00 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 15:29:38 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	debug(t_coder *coder)
{
	t_args	*args;
	long	elapsed;

	args = coder->args;
	elapsed = get_timestamp() - args->timestart;
	pthread_mutex_lock(&args->output_mutex);
	printf("%ld %d is debugging\n", elapsed, coder->id);
	pthread_mutex_unlock(&args->output_mutex);
	usleep(args->params->time_to_debug * 1000);
}

void	refactor(t_coder *coder)
{
	t_args	*args;
	long	elapsed;

	args = coder->args;
	elapsed = get_timestamp() - args->timestart;
	pthread_mutex_lock(&args->output_mutex);
	printf("%ld %d is refactoring\n", elapsed, coder->id);
	pthread_mutex_unlock(&args->output_mutex);
	usleep(args->params->time_to_refactor * 1000);
}
