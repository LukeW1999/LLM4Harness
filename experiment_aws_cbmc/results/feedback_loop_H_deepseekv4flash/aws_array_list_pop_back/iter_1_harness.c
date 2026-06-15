#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic index */
    size_t index;
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION); /* bound to avoid overflow in offset calculation */
    /* Ensure offset multiplication does not overflow */
    __CPROVER_assume(aws_mul_size_checked(index, list.item_size, &(size_t){0}) == AWS_OP_SUCCESS);

    /* Non-deterministic val pointer: must be readable for item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save old state BEFORE calling */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage;
    save_byte_from_array(val, list.item_size, &storage);

    /* Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* 1. Changed field: length increases if index >= old length */
        size_t expected_length;
        if (index >= old.length) {
            /* new length = index + 1 (no overflow assumed) */
            __CPROVER_assume(aws_add_size_checked(index, 1, &expected_length) == AWS_OP_SUCCESS);
        } else {
            expected_length = old.length;
        }
        assert(list.length == expected_length);

        /* 2. Changed field: element at index must match val */
        uint8_t *dst = (uint8_t *)list.data + (index * list.item_size);
        assert_byte_from_buffer_matches(dst, &storage);

        /* 3. Capacity must be sufficient */
        size_t required_size;
        int overflow = aws_mul_size_checked(list.length, list.item_size, &required_size);
        assert(overflow == AWS_OP_SUCCESS);
        assert(list.current_size >= required_size);

        /* 4. Data must be non-null if current_size > 0 */
        if (list.current_size > 0) {
            assert(list.data != NULL);
        }
    } else {
        /* On failure, list should be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* Fields that must NOT change regardless of result */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
