/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 11:17:35 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 18:51:56 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_all(t_args *args)
{
	int			counter;
	t_params	*params;
	t_coder		**coder;

	params = args->params;
	coder = args->coder;
	counter = 0;
	while (counter < params->number_of_coders)
	{
		pthread_mutex_destroy(&args->mutex[counter]);
		pthread_cond_destroy(&args->dongle[counter].cond);
		counter++;
	}
	pthread_mutex_destroy(&args->arbiter_mutex);
	pthread_mutex_destroy(&args->output_mutex);
	pthread_cond_destroy(&args->dongles_cond);
	counter = 0;
	while (counter < params->number_of_coders)
		free(coder[counter++]);
	free(coder);
	free(args->params);
	free(args->dongle);
	free(args->mutex);
	free(args->requests);
	free(args);
}
