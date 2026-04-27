#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    __CPROVER_assume(max_initial_item_allocation > 0 && max_initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(max_item_size > 0 && max_item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_initial_item_allocation, max_item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic input value */
    void *val = malloc(list.item_size);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(index < list.alloc); // Ensure index is within bounds

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* 1. Changed fields */
        assert(list.length == old.length); // Length should not change on successful set_at
        uint8_t *expected_data = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(expected_data, (uint8_t *)val, list.item_size);

        /* 2. Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data); // Data pointer should not change

        // Check other elements remain unchanged
        for (size_t i = 0; i < old.length; i++) {
            if (i != index) {
                uint8_t *old_data = (uint8_t *)old.data + (old.item_size * i);
                uint8_t *new_data = (uint8_t *)list.data + (list.item_size * i);
                assert_bytes_match(new_data, old_data, list.item_size);
            }
        }
    } else {
        /* 1. Changed fields */
        assert(list.length == old.length); // Length should not change on failure

        /* 2. Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data); // Data pointer should not change
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data); // Data pointer should not change

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
