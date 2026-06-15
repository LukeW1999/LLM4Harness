#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare and bound the list structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 2. Non‑NULL allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 3. Nondeterministic inputs respecting preconditions */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Call the function under test */
    int result = aws_array_list_init_dynamic(
        &list, alloc, initial_item_allocation, item_size);

    /* 5. Post‑conditions that must always hold */
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);
    assert(list.length == 0);

    /* current_size and data depend on allocation success */
    if (result == AWS_OP_SUCCESS) {
        /* on success, either no allocation was requested or it succeeded */
        if (initial_item_allocation == 0) {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            /* allocation may have succeeded */
            assert(list.current_size == initial_item_allocation * item_size);
            assert(list.data != NULL);
        }
    } else {
        /* on failure, current_size must be zero and data must be NULL */
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* 6. The list must satisfy its validity invariant */
    assert(aws_array_list_is_valid(&list));
}
