#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(item_size > 0);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.current_size == initial_item_allocation * item_size);
        assert(list.length == 0);
        assert(list.data != NULL);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
