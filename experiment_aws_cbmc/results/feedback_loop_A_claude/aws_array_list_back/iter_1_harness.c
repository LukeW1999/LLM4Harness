#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/*
 * Analysis before writing:
 * 1. What fields change on success?
 *    - val is written with the last element's data (memcpy)
 *    - The list itself is NOT modified (const parameter)
 * 2. What fields are unchanged?
 *    - list->alloc, list->current_size, list->length, list->item_size, list->data
 *      (all unchanged — list is const)
 * 3. What happens on failure?
 *    - Returns AWS_ERROR_LIST_EMPTY (via aws_raise_error) when length == 0
 *    - val is not written
 *    - list is unchanged
 * 4. What validity invariants must hold after the call?
 *    - aws_array_list_is_valid(list) must hold (postcondition in implementation)
 */

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;

    /* 3. Allocate val — must be writable for item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Assert postconditions */

    /* The list must remain valid regardless of outcome */
    assert(aws_array_list_is_valid(&list));

    /* The list fields must be unchanged (list is const) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        /* Success: list was non-empty, val was written with last element */
        assert(list.length > 0);
        /* val should contain the last element — verify by checking memory matches */
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match(
            (const uint8_t *)val,
            (const uint8_t *)list.data + last_item_offset,
            list.item_size
        );
    } else {
        /* Failure: list was empty */
        assert(list.length == 0);
        assert(result == AWS_OP_ERR);
    }
}
