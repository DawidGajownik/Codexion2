/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 11:17:35 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/10 16:08:30 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void free_all(t_args *args)
{
    int counter;
    t_params *params;
    t_coder **coder;

    params = args->params;
    coder = args->coder;
    counter = 0;
    while (counter < params->number_of_coders)
        free(coder[counter++]);
    free(coder);
    free(args->params);
    free(args->dongle);
    free(args->mutex);
    free(args);
}
