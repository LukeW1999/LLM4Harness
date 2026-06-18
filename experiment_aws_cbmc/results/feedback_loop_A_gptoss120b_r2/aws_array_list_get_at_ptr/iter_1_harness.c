#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare output pointer and nondet index */
    void *out = NULL;
    void *old_out = out;
    void **val = &out;
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 5. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index must be within bounds and out points to the correct element */
        assert(index < old.length);
        void *expected = (void *)((uint8_t *)old.data + (old.item_size * index));
        assert(out == expected);
    } else {
        /* Failure: index out of bounds and out unchanged */
        assert(index >= old.length);
        assert(out == old_out);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
