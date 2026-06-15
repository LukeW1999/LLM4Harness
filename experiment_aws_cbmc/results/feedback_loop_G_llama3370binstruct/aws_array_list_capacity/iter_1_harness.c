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
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    size_t index = nondet_size_t();
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length >= index); /* length is at least index */
        assert(list.length <= list.current_size / list.item_size); /* length does not exceed capacity */
        assert(list.data != NULL); /* data is not null */
        assert(list.item_size == old.item_size); /* item_size is unchanged */
        assert(list.alloc == old.alloc); /* allocator is unchanged */
    } else {
        assert(list.length == old.length); /* length is unchanged on failure */
        assert(list.data == old.data); /* data is unchanged on failure */
        assert(list.item_size == old.item_size); /* item_size is unchanged on failure */
        assert(list.alloc == old.alloc); /* allocator is unchanged on failure */
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
