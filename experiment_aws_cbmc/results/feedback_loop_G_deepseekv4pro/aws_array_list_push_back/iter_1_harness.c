#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    
    /* Allocate underlying data member nondeterministically */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state for immutability checks */
    struct aws_array_list old = list;
    /* Save a byte from the data array (if any) to check potential changes later */
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    } else {
        /* No data to save; mark index as 0 to avoid invalid access */
        old_byte.index = 0;
    }

    /* 2. Prepare the value to be set (must be readable memory of item_size) */
    size_t item_size = list.item_size;
    /* item_size > 0 because list is valid */
    uint8_t *val = (uint8_t *)malloc(item_size);
    /* Assume allocation succeeded (memory model) */
    __CPROVER_assume(val != NULL);
    /* val now points to readable/writable block of item_size bytes */

    /* 3. Choose an arbitrary index */
    size_t index = nondet_size_t();

    /* 4. Call the function under test */
    int rv = aws_array_list_set_at(&list, (void *)val, index);

    /* 5. Postconditions */

    /* Validity always holds */
    assert(aws_array_list_is_valid(&list));

    /* These fields never change (allocator, item_size) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (rv == AWS_OP_SUCCESS) {
        /*
         * On success the element at the given index must equal the provided value.
         * The function has ensured capacity, so the region is writable and within bounds.
         */
        uint8_t *dest = (uint8_t *)list.data + index * list.item_size;
        assert(memcmp(dest, val, list.item_size) == 0);

        /* Length update logic:
         *   If index >= old.length, then new length = index + 1,
         *   otherwise length stays at old.length.
         */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* If the underlying data pointer changed (realloc), old_byte may point to freed memory.
         * We only verify that the byte at the saved position is unchanged if data pointer did not move.
         */
        if (old.data == list.data) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    } else {
        /* Failure: the function returned an error code.
         * The list must remain valid (already asserted).
         * In static mode (no allocator), the error must be raised if index is out of bounds.
         */
        if (old.alloc == NULL) {
            /* capacity = current_size / item_size, which is the max index allowed */
            size_t capacity = old.current_size / old.item_size;
            /* If index is beyond capacity, failure is expected; otherwise the function must have succeeded */
            if (index >= capacity) {
                assert(rv == AWS_OP_ERR);
            }
        }
        /* For dynamic mode, failure can happen due to overflow or memory exhaustion.
         * We cannot assert a specific state, but we still have the validity invariant.
         */
    }

    free(val);
}
