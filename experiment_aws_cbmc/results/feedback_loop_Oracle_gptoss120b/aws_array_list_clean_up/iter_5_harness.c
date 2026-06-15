#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <string.h>

/* Stub release function that satisfies the allocator contract */
static void stub_mem_release(struct aws_allocator *alloc, void *ptr) {
    (void)alloc;
    (void)ptr;
}

void aws_array_list_clean_up_harness(void) {
    struct aws_allocator allocator;
    allocator.mem_release = stub_mem_release;
    allocator.mem_acquire = NULL;
    allocator.mem_calloc  = NULL;
    allocator.mem_realloc = NULL;

    struct aws_array_list list;
    memset(&list, 0, sizeof(list));
    /* Do NOT set list.alloc; keep the struct all‑zero to satisfy the precondition */

    aws_array_list_clean_up(&list);

    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
}
