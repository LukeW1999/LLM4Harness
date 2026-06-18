#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* 3. Prepare output buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL || list.item_size == 0);

    /* 4. Non-deterministic index */
    size_t index;
    index = nondet_size_t();

    /* 5. Call function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_item_size);

    /* alloc never changes */
    assert(list.alloc == old_alloc);

    /* length never changes (get_at is read-only) */
    assert(list.length == old_length);

    /* current_size never changes (get_at is read-only) */
    assert(list.current_size == old_current_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been within bounds */
        assert(index < old_length);

        /* val must be non-null since we read something */
        assert(val != NULL);

        /* The value at index matches what was in the list */
        if (list.item_size > 0 && list.data != NULL) {
            assert_bytes_match((uint8_t *)val,
                               (uint8_t *)list.data + (list.item_size * index),
                               list.item_size);
        }
    } else {
        /* On failure: index was out of bounds */
        assert(index >= old_length);

        /* list is still valid (already asserted above) */
        assert(aws_array_list_is_valid(&list));
    }
}
