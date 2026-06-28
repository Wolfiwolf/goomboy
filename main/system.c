#include <esp_heap_caps.h>
#include <multi_heap.h>
#include "system.h"

size_t sys_get_free_mem(void)
{
	multi_heap_info_t meminfo;

	heap_caps_get_info(&meminfo, MALLOC_CAP_DEFAULT);

	return meminfo.total_free_bytes;
}
