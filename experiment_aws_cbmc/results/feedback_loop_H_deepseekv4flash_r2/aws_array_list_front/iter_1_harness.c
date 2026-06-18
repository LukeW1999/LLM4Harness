#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;
    size_t item_size = nondet_size_t();

    /* Bounding to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* We need val to point to readable memory of list.item_size bytes */
    size_t val_size = list.item_size;
    void *val = malloc(val_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, val_size));

    /* Save old structure state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* Call the function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    assert(aws_array_list_is_valid(&list)); /* Assert validity always */
    if (result == AWS_OP_SUCCESS) {
        /* On success, list remains valid. We cannot guarantee data unchanged because ensure_capacity may reallocate. */
        /* Length may increase if index >= old length */
        if (index >= old.length) {
            size_t expected_new_length;
            if (aws_add_size_checked(index, 1, &expected_new_length) == AWS_OP_SUCCESS) {
                assert(list.length == expected_new_length);
            }
        } else {
            assert(list.length == old.length);
        }
        /* item_size and alloc must not change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        /* On failure, list should be unchanged (but still valid) */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        /* Data pointer should be unchanged on failure (ensure_capacity would not have modified it) */
        assert(list.data == old.data);
        /* Also bytes in data buffer unchanged (if data is writable) */
        if (list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* Free allocated val memory (to avoid leak, but not strictly necessary in CBMC) */
    free(val);
}
