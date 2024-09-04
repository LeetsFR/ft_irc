/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scely <scely@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 20:56:24 by scely             #+#    #+#             */
/*   Updated: 2024/09/03 22:02:40 by scely            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

void	signal_c(int signal_code)
{
	g_stop = true;
	(void)signal_code;
	cout << "\r";
}

void handler_signal()
{
	struct sigaction	c_signal;

	c_signal.sa_handler = signal_c;
	c_signal.sa_flags = 0;
	sigemptyset(&c_signal.sa_mask);
	sigaction(SIGINT, &c_signal, NULL);
}