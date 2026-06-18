#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;

    /* Bound the list to prevent state space explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up val — must be writable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state */
    struct aws_array_list old_list = list;

    /* 4. Call the function under test */
    int result = aws_array_list_front(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_list.item_size);

    /* alloc never changes */
    assert(list.alloc == old_list.alloc);

    /* length never changes */
    assert(list.length == old_list.length);

    /* current_size never changes */
    assert(list.current_size == old_list.current_size);

    /* data pointer never changes */
    assert(list.data == old_list.data);

    if (result == AWS_OP_SUCCESS) {
        /* On success: list must have had at least one element */
        assert(old_list.length > 0);

        /* data pointer must be non-null */
        assert(list.data != NULL);

        /* The value copied out must match the front of the list */
        assert_bytes_match(
            val,
            (uint8_t *)list.data,
            list.item_size);
    } else {
        /* On failure: list was empty */
        assert(old_list.length == 0);
    }
}
