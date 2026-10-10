/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setters.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 11:14:53 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/10 16:53:44 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


t_args* set_args(char **argv)
{
    t_params *params;
    t_args *args;

    params = set_params(argv);
    args = malloc(sizeof(t_args));
    args->params = params;

    return args;
}

t_params* set_params(char **argv)
{
    t_params *params;

    params = malloc(sizeof(t_params));
    params->number_of_coders = atoi(argv[1]);
    params->time_to_burnout = atoi(argv[2]);
    params->time_to_compile = atoi(argv[3]);
    params->time_to_debug = atoi(argv[4]);
    params->time_to_refactor = atoi(argv[5]);
    params->number_of_compiles_required = atoi(argv[6]);
    params->dongle_cooldown = atoi(argv[7]);
    params->scheduler = argv[8];

    return params;
}