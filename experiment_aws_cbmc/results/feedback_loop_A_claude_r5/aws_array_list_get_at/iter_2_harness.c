#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_harness(void) {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;

    /* Bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old_list = list;

    /* 3. Set up val: must be writable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Choose a non-deterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 5. Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_list.item_size);

    /* alloc never changes */
    assert(list.alloc == old_list.alloc);

    /* length never changes for get_at */
    assert(list.length == old_list.length);

    /* current_size never changes for get_at */
    assert(list.current_size == old_list.current_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been within bounds */
        assert(index < old_list.length);

        /* data must be non-null */
        assert(list.data != NULL);

        /* val must have been written with the element at index */
        uint8_t *src = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(val, src, list.item_size);

    } else {
        /* On failure: index was out of bounds */
        assert(result == AWS_OP_ERR);
        assert(index >= old_list.length);
    }
}
