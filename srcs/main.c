/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdarmaya <vdarmaya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 18:34:42 by vdarmaya          #+#    #+#             */
/*   Updated: 2018/08/14 20:14:37 by vdarmaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int		main(int argc, char **argv)
{
	if (argc < 2)
		return (0);
	manage_file(argv[1]);
	return (0);
}
