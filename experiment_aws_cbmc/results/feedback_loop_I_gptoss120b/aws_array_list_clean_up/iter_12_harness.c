#include <assert.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>

/* Dummy allocator functions that satisfy CBMC preconditions */
static void *dummy_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    return __CPROVER_allocate(size, 0);
}

static void dummy_mem_release(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    __CPROVER_free(ptr);
}

/* Minimal allocator instance for verification */
static struct aws_allocator dummy_allocator = {
    .mem_acquire = dummy_mem_acquire,
    .mem_release = dummy_mem_release,
    .mem_calloc = 0,
    .mem_realloc = 0
};

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = &dummy_allocator;

    /* Ensure the allocator's release function is non‑null (precondition) */
    __CPROVER_assume(alloc->mem_release != 0);

    /* Deterministic sizes for the list */
    size_t item_size = 8;
    size_t init_capacity = 4;

    /* Manually construct a valid list without calling init */
    void *data = dummy_mem_acquire(alloc, item_size * init_capacity);
    __CPROVER_assume(data != 0);               /* data must be a valid allocation */

    list.data = data;
    list.length = 0;
    list.current_size = init_capacity;
    list.item_size = item_size;
    list.alloc = alloc;

    /* Call the function under verification */
    aws_array_list_clean_up(&list);

    /* Post‑conditions */
    assert(list.data == NULL);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.alloc == NULL);
    assert(list.item_size == 0);
}
