#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and initialize the array list */
    struct aws_array_list list;

    /* Use bounded assumptions to keep CBMC tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Non-deterministic index - allow any index including out-of-bounds */
    size_t index;

    /* 3. Allocate a val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must never change */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success: data pointer must be non-null */
        assert(list.data != NULL);

        /* On success: length must be > index */
        assert(list.length > index);

        /* On success: current_size must accommodate at least index+1 items */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* current_size must be >= old current_size (can only grow) */
        assert(list.current_size >= old_current_size);

        /* If index was >= old length, length should be index+1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within existing bounds */
            assert(list.length == old_length);
        }

        /* The data at index must match val */
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

    } else {
        /* On failure: the list state should be consistent */
        assert(aws_array_list_is_valid(&list));
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
    }
}
