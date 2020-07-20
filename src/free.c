/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   free.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tblaudez <tblaudez@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/07/07 17:46:56 by tblaudez      #+#    #+#                 */
/*   Updated: 2020/07/20 14:51:20 by tblaudez      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include "malloc.h"

static bool	is_zone_empty(t_zone *zone)
{
	t_block	*block;

	block = zone->block;
	while (block)
	{
		if (block->free == false)
			return (false);
		block = block->next;
	}
	return (true);
}

static void	remove_zone_from_list(t_zone *to_delete)
{
	t_zone *zone;

	if (to_delete == g_malloc)
	{
		g_malloc = to_delete->next;
		return ;
	}
	zone = g_malloc;
	while (zone->next != to_delete)
		zone = zone->next;
	zone->next = to_delete->next;
}

static void	start_defragmentation(t_zone *zone)
{
	t_block	*block;

	block = zone->block;
	while (block)
	{
		if (block->free == true && block->next && block->next->free == true)
		{
			block->true_size += (block->next->true_size + sizeof(t_block));
			block->next = block->next->next;
			continue ;
		}
		block = block->next;
	}
}

void	free(void *ptr)
{
	t_zone	*zone;
	t_block	*block;

	if (ptr == NULL)
		return ;
	find_block_by_ptr(&zone, &block, ptr);
	if (block == NULL)
		return ;
	block->free = true;
	block->alloc_size = 0;
	if  (zone->kind == LARGE || is_zone_empty(zone) == true)
	{	
		remove_zone_from_list(zone);
		munmap(zone, zone->size);
	}
	else
		start_defragmentation(zone);
}
