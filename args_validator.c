/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args_validator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 16:23:38 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 18:33:19 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

static int	isint(char *str)
{
	int		i;
	long	value;

	i = 0;
	value = 0;
	if (!str[0])
		return (0);
	while (str[i])
	{
		if (!is_digit(str[i]))
			return (0);
		value = value * 10 + (str[i] - '0');
		if (value > INT_MAX)
			return (0);
		i++;
	}
	return (1);
}

static int	scheduler_arg_valid(char *str)
{
	if (strcmp(str, "fifo") && strcmp(str, "edf"))
		return (0);
	return (1);
}

int	args_valid(int argc, char **argv)
{
	int	i;

	i = 1;
	if (argc != 9)
		return (0);
	else
		while (i < argc - 1)
			if (!isint(argv[i++]))
				return (0);
	if (!scheduler_arg_valid(argv[i]))
		return (0);
	return (1);
}
