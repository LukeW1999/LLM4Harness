#include <aws/common/array_list.h>
#include <aws/common/math.h>
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

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Allocate output buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;

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

    /* current_size never changes */
    assert(list.current_size == old_current_size);

    /* data pointer never changes */
    assert(list.data == old_data);

    if (result == AWS_OP_SUCCESS) {
        /* On success: index must be within bounds */
        assert(index < old_length);

        /* val must have been written (data is non-null) */
        assert(list.data != NULL);

        /* The value copied out matches what's in the list at index */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), (uint8_t *)val, list.item_size);
    } else {
        /* On failure: index must be out of bounds */
        assert(index >= old_length);
    }
}
