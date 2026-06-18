#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Ensure the list may need to grow (to make failure reachable) */
    __CPROVER_assume(list.current_size >= list.length);

    /* 2. Allocate a nondeterministic input value */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
        /* Prevent aliasing between the input value and the list's buffer */
        __CPROVER_assume(list.data == NULL || val != list.data);
    }

    /* 3. Save old state before the call */
    struct aws_array_list old = list;

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length must increase by one */
        assert(list.length == old.length + 1);
        /* Allocator and item size must stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
