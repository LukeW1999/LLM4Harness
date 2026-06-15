#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    void *val = (void *)nondet_uint8_t();
    __CPROVER_assume(val && AWS_MEM_IS_READABLE(val, list.item_size));
    size_t index = nondet_size_t();
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields (from Doxygen) */
        if (index >= aws_array_list_length(&old)) {
            assert(list.length == index + 1);
        }
        assert(list.data != NULL);
        assert(list.current_size >= list.length * list.item_size);

        /* Unchanged fields (implied — Doxygen rarely lists these) */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* Failure: struct unchanged */
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
