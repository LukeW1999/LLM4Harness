#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Declare output pointer and nondet index */
    void *out = NULL;
    void **val = &out;
    size_t index = nondet_size_t();

    /* 3. Save old state BEFORE calling */
    struct aws_array_list old = list;
    void *old_out = out;

    /* 4. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index is within bounds and out points to the element */
        assert(list.length > index);
        void *expected = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(out == expected);
    } else {
        /* Failure: out must remain unchanged */
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
