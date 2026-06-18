#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate val with item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state */
    struct aws_array_list old_list = list;

    /* 4. Call function under test */
    int result = aws_array_list_push_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must not change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - length increases by 1
         * - current_size must be >= length * item_size
         * - data pointer must be valid
         */
        assert(list.length == old_list.length + 1);
        assert(list.current_size >= list.length * list.item_size);
        assert(list.data != NULL);

        /* Verify the value was actually written at the end */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * old_list.length), val, list.item_size);

    } else {
        /* On failure:
         * - The list should remain unchanged
         */
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
    }
}
