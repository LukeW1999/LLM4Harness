#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    __CPROVER_assume(max_initial_item_allocation > 0);
    __CPROVER_assume(max_item_size > 0);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_initial_item_allocation, max_item_size));
    ensure_array_list_has_allocated_data_member(&list, allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Non-deterministically choose input parameters */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size <= max_item_size);
    void *val = can_fail_malloc(item_size);
    __CPROVER_assume(val && AWS_MEM_IS_READABLE(val, item_size));

    /* 4. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(memcmp(list.data, val, item_size) == 0);
        if (old.length > 0) {
            assert(memcmp((uint8_t *)list.data + item_size, old.data, old.length * old.item_size) == 0);
        }
    } else {
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
