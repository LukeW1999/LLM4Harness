#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare data structures and nondeterministic inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc;
    size_t initial_item_allocation;
    size_t item_size;

    /* Bound the inputs */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 4. Postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that must be set on success */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == initial_item_allocation * item_size);
        /* data pointer must be non‑NULL iff current_size > 0 */
        if (list.current_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
        }
    } else {
        /* On failure the list is still initialized but empty */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* 5. Fields that must not change regardless of result */
    /* All fields are explicitly set by the function; no additional unchanged fields. */

    /* 6. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
