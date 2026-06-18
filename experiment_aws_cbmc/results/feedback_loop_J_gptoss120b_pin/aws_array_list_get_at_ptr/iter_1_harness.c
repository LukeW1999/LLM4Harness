#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet index */
    size_t index = nondet_size_t();

    /* output pointer */
    void *out = nondet_uint8_t() ? (void *)0x1 : (void *)0x0; /* nondet initial value */
    void **val = &out;

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;
    void *old_out = out;

    /* 3. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success, *val points to the element at the given index */
        void *expected = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(out == expected);
    } else {
        /* On failure, output pointer must be unchanged */
        assert(out == old_out);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
