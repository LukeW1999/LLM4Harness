#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic input parameters */
    void *val = malloc(list.item_size);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    size_t index = nondet_size_t();

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == (index >= old.length ? index + 1 : old.length));
        assert(AWS_MEM_IS_EQUAL(((uint8_t *)list.data) + (index * list.item_size), val, list.item_size));
    } else {
        assert(list.length == old.length);
        assert(AWS_MEM_IS_EQUAL(list.data, old.data, old.current_size));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
