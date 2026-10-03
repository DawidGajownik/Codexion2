/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args_validator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 16:23:38 by dgajowni          #+#    #+#             */
/*   Updated: 2026/10/03 17:18:29 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int isdigit(char c)
{
    return (c > 47 && c < 58);
}

static int isint(char *str)
{
    int c;

    c = 0;
    while (str[c])
        if (!isdigit(str[c++]))
            return (0);
    return (1);
}

static int scheduler_arg_valid(char *str)
{
    if (strcmp(str, "fifo") && strcmp(str, "edf"))
        return(0);
    return(1);
}

static int numeric_arg_valid(char *num)
{
    return((atoi(num) >= 0 && isint(num)));
}

int args_valid(int argc, char **argv)
{
    int i;

    i = 1;
    if (argc != 9)
        return(0);
    else
        while (i < argc - 1)
            if (!numeric_arg_valid(argv[i++]))
                return(0);
    if (!scheduler_arg_valid(argv[i]))
        return(0);
    return (1);
}