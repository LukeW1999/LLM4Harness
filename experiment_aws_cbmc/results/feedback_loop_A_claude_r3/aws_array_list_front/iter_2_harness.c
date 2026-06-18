#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare output buffer of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Call function under test */
    int result = aws_array_list_front(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old.item_size);

    /* alloc never changes */
    assert(list.alloc == old.alloc);

    /* length never changes */
    assert(list.length == old.length);

    /* current_size never changes */
    assert(list.current_size == old.current_size);

    /* data pointer never changes */
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        /* On success: list must have had at least one element */
        assert(old.length > 0);

        /* val must have been written with the front element */
        assert(val != NULL);
        assert_bytes_match(val, (uint8_t *)list.data, list.item_size);
    } else {
        /* On failure: list was empty */
        assert(old.length == 0);
    }
}
