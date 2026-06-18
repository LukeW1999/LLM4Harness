#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable input buffer */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* Assume the memory is readable for the required size */
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The data pointer must be non‑NULL after a successful set */
        assert(list.data != NULL);

        /* Prevent overflow when computing the byte offset */
        __CPROVER_assume(list.item_size == 0 || index <= SIZE_MAX / list.item_size);

        /* The value at the specified index must match the input */
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           (const uint8_t *)val,
                           list.item_size);

        /* Length updates */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Fields that never change regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 8. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
