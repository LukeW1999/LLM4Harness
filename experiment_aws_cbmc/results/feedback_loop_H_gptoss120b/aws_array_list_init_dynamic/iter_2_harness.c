#include <assert.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>

/* Upper bounds for nondeterministic choices */
#ifndef MAX_INITIAL_ITEM_ALLOCATION
#   define MAX_INITIAL_ITEM_ALLOCATION 16
#endif
#ifndef MAX_ITEM_SIZE
#   define MAX_ITEM_SIZE 64
#endif

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Allocate and zero‑initialize the list structure */
    struct aws_array_list list = {0};

    /* 2. Use a concrete allocator with valid function pointers */
    struct aws_allocator *alloc = (struct aws_allocator *)aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* 3. Nondeterministic but bounded inputs */
    size_t initial_item_allocation;
    size_t item_size;

    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 4. Call the function under test */
    int result = aws_array_list_init_dynamic(
        &list,
        alloc,
        initial_item_allocation,
        item_size);

    /* 5. Restrict to the successful execution path for verification */
    __CPROVER_assume(result == AWS_OP_SUCCESS);

    /* 6. Post‑conditions that must hold on success */
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.current_size == initial_item_allocation * item_size);
    if (list.current_size == 0) {
        assert(list.data == NULL);
    } else {
        assert(list.data != NULL);
    }

    /* 7. The list must satisfy its validity invariant */
    assert(aws_array_list_is_valid(&list));
}
