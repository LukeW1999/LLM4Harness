#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length);  // length does not change
        assert(list.current_size == old.current_size);  // current_size does not change
        assert(list.item_size == old.item_size);  // item_size does not change
        assert(list.alloc == old.alloc);  // allocator does not change
        assert(list.data == old.data);  // data pointer does not change
    } else {
        assert(list.length == old.length);  // length does not change on failure
        assert(list.current_size == old.current_size);  // current_size does not change on failure
        assert(list.item_size == old.item_size);  // item_size does not change on failure
        assert(list.alloc == old.alloc);  // allocator does not change on failure
        assert(list.data == old.data);  // data pointer does not change on failure
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
