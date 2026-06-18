#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_pop_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before the call */
    struct aws_array_list old = list;

    /* Save a byte from the data buffer for immutability checks on failure */
    struct store_byte_from_buffer old_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);
    }

    /* 3. Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Assert postconditions */

    /* Fields that must NEVER change regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        /* Success path: list was non-empty, front element was removed */
        /* Length decreases by 1 */
        assert(list.length == old.length - 1);
        /* The list was non-empty before the call */
        assert(old.length > 0);
    } else {
        /* Failure path: list was empty, nothing changes */
        assert(result == AWS_OP_ERR);
        assert(old.length == 0);
        /* Length unchanged */
        assert(list.length == old.length);
        /* Data contents unchanged */
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_byte);
        }
    }

    /* 5. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
