#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Non-deterministic inputs */
    size_t index = nondet_size_t();
    uint8_t val[MAX_ITEM_SIZE];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* 1. Changed fields */
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        }
        uint8_t expected_val[MAX_ITEM_SIZE];
        memcpy(expected_val, val, list.item_size);
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), expected_val, list.item_size);

        /* 2. Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        if (index < old.length) {
            assert(list.length == old.length);
        }
    } else {
        /* 3. Failure: struct unchanged */
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
