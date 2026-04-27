#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic input value */
    void *val = malloc(list.item_size);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* 1. Changed fields */
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }
        uint8_t *expected_data = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(expected_data, (uint8_t *)val, list.item_size);

        /* 2. Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        if (index < old.length) {
            assert(list.data != NULL);
            for (size_t i = 0; i < old.length; i++) {
                if (i != index) {
                    uint8_t *old_data = (uint8_t *)old.data + (old.item_size * i);
                    uint8_t *new_data = (uint8_t *)list.data + (list.item_size * i);
                    assert_bytes_match(new_data, old_data, list.item_size);
                }
            }
        }
    } else {
        /* 1. Changed fields */
        assert(list.length == old.length);

        /* 2. Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
