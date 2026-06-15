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

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = &dummy_allocator;

    /* Use small, deterministic sizes to keep the verification tractable */
    size_t item_size = 8;
    size_t init_capacity = 4;

    /* Assume allocator function pointers are non‑null (precondition of clean_up) */
    __CPROVER_assume(alloc->mem_release != 0);

    if (aws_array_list_init(&list, alloc, init_capacity, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    aws_array_list_clean_up(&list);

    assert(list.data == NULL);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.alloc == NULL);
    assert(list.item_size == 0);
}
