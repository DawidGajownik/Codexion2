/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cooldown.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgajowni <dgajowni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/01 18:11:16 by dgajowni          #+#    #+#             */
/*   Updated: 2026/11/01 18:11:51 by dgajowni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	cool_enough(t_coder *coder)
{
	t_args	*args;
	long	now;

	args = coder->args;
	now = get_timestamp();
	return (now - coder->dongle_l->locktime
		>= args->params->dongle_cooldown
		&& now - coder->dongle_r->locktime
		>= args->params->dongle_cooldown);
}

void	get_deadline(t_coder *coder, struct timespec *deadline)
{
	t_args	*args;
	long	ready_ms;
	long	right_ready_ms;

	args = coder->args;
	ready_ms = coder->dongle_l->locktime
		+ args->params->dongle_cooldown;
	right_ready_ms = coder->dongle_r->locktime
		+ args->params->dongle_cooldown;
	if (right_ready_ms > ready_ms)
		ready_ms = right_ready_ms;
	deadline->tv_sec = ready_ms / 1000;
	deadline->tv_nsec = (ready_ms % 1000) * 1000000L;
}
