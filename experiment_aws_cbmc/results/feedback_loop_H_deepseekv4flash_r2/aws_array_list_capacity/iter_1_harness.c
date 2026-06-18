#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    /* Bounded non-deterministic list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.current_size > 0 && list.data) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    /* Non-deterministic val: readable memory of item_size bytes */
    size_t item_size = list.item_size;
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Non-deterministic index */
    size_t index = nondet_size_t();

    /* Call function */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Data at index matches val (only if index < new length) */
        if (list.length > index) {
            assert_bytes_match((uint8_t *)list.data + (index * item_size), val, item_size);
        }
        /* Length becomes max(old.length, index + 1) */
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);
        /* Fields that must not change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* Data pointer may change due to reallocation, but current_size and data are valid */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure, list is unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.current_size > 0 && list.data) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
        }
        assert(aws_array_list_is_valid(&list));
    }

    /* Validity invariant holds on both paths */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
