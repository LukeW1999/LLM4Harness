#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_clear_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_storage = {0};
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_storage);
    }

    /* 3. Call the function under test */
    int result = aws_array_list_clear(&list);

    /* 4. Post‑conditions */
    assert(result == AWS_OP_SUCCESS);
    assert(aws_array_list_is_valid(&list));

    /* Length must be zero after clear */
    assert(list.length == 0);

    /* Fields that must not change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* Data buffer must remain unchanged (clear does not free or modify contents) */
    if (list.data) {
        assert_byte_from_buffer_matches(list.data, &old_storage);
    }

    /* 5. Clean up */
    if (list.data) {
        aws_mem_release(list.alloc, list.data);
        list.data = NULL;
    }
}
