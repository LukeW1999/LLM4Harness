#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old_list = list;

    /* Save a byte from the backing data for immutability check */
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 3. Allocate output buffer of item_size bytes */
    /* item_size > 0 is guaranteed by validity predicate */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Assert postconditions */

    /* Return value: success iff list was non-empty */
    if (list.length > 0) {
        /* On success: result must be AWS_OP_SUCCESS */
        assert(result == AWS_OP_SUCCESS);

        /* The val buffer should contain the last element:
         * val == list.data + item_size * (length - 1) */
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match(val, (uint8_t *)list.data + last_item_offset, list.item_size);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == AWS_ERROR_LIST_EMPTY || result != AWS_OP_SUCCESS);
        /* More precisely, the function returns aws_raise_error(...) which returns AWS_OP_ERR */
        assert(result != AWS_OP_SUCCESS);
    }

    /* 6. Frame conditions: list fields must not change */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* 7. Backing data must not change */
    if (old_list.current_size > 0 && old_list.data != NULL) {
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    }

    /* 8. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));
}
