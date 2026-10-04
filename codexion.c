/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 14:24:36 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/04 19:22:04 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void* hello(void* arg) {
    printf("Coder nr = %d\n", *(int*)arg);
    return NULL;
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

t_coder** create_coders(t_params *params)
{
    int counter;
    t_coder **coder;

    coder = malloc(sizeof(t_coder*) * params->number_of_coders);
    counter = 0;
    while (counter < params->number_of_coders)
    {
        coder[counter] = malloc(sizeof(t_coder));
        coder[counter]->id = counter+1;
        pthread_create(&(coder[counter]->thread), NULL, hello, &coder[counter]->id);
        counter++;
    }
    counter = 0;
    while (counter < params->number_of_coders)
    {
        pthread_join(coder[counter++]->thread, NULL);
    }
    counter = 0;
    while (counter < params->number_of_coders)
        free(coder[counter++]);
    free(coder);
    return coder;
}

int main(int argc, char **argv)
{
    long timestart;
    int zero;
    t_params *params;
    t_coder **coder;

    if (!args_valid(argc, argv))
        return(0);

    params = set_params(argv);
    coder = create_coders(params);
    zero = 0;
    timestart = get_timestamp();

    //checkers(argc, argv, timestart);
    free(params);

    return (0);
}