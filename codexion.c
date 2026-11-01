/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 14:24:36 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 18:26:58 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	start_simulation(t_args *args)
{
	t_coder		**coder;
	t_params	*params;

	params = args->params;
	coder = malloc(sizeof(t_coder *) * params->number_of_coders);
	args->coder = coder;
	threads_start(args);
	threads_finish(args);
}

int	main(int argc, char **argv)
{
	t_args	*args;

	if (!args_valid(argc, argv))
		return (0);
	args = create_args(argv);
	start_simulation(args);
	free_all(args);
	return (0);
}
