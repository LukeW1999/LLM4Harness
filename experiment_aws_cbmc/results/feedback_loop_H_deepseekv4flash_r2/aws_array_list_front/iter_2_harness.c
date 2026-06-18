#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;

    /* Bounding to limit state space */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* We need val to point to readable memory of list.item_size bytes */
    size_t val_size = list.item_size;
    void *val = malloc(val_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, val_size));

    /* Save old length and item_size for later assertions */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    if (result == AWS_OP_SUCCESS) {
        /* On success, length may increase if index >= old length */
        if (index >= old_length) {
            size_t expected_new_length;
            if (aws_add_size_checked(index, 1, &expected_new_length) == AWS_OP_SUCCESS) {
                assert(list.length == expected_new_length);
            }
        } else {
            assert(list.length == old_length);
        }
    } else {
        /* On failure, length and data pointer should be unchanged */
        assert(list.length == old_length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* Free allocated val memory (to avoid leak, but not strictly necessary in CBMC) */
    free(val);
}
