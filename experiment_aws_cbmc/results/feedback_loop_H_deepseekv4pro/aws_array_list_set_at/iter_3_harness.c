#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    size_t index;
    void *val;

    /* Initialize a valid array list with bounded sizes and default allocator */
    list.alloc = aws_default_allocator();
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);
    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION);
    /* Prevent overflow in current_size * item_size */
    __CPROVER_assume(list.current_size <= SIZE_MAX / list.item_size);
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= list.current_size);
    list.data = malloc(list.current_size * list.item_size);
    __CPROVER_assume(list.data != NULL);

    /* Allocate a buffer for the value to be written */
    val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Index must be within current capacity to avoid reallocation */
    index = nondet_size_t();
    __CPROVER_assume(index < list.current_size);

    /* Save the state before the call */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* Invoke the function under test */
    int rv = aws_array_list_set_at(&list, val, index);

    /* Universal invariants */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (rv == AWS_OP_SUCCESS) {
        /* On success the element must be properly stored */
        assert(index < list.current_size);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val, list.item_size);

        /* Length is updated only when writing beyond the old length */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }
    } else {
        /* On failure the length must not change */
        assert(list.length == old_length);
        /* list.data may have changed (if realloc succeeded before a later failure) */
    }
}
