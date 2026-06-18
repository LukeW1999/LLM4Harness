#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
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
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage;
    if (list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &storage);
    }

    /* 3. Allocate output buffer and save its initial content */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    uint8_t *old_val = malloc(list.item_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, list.item_size);

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: val must contain the last element */
        uint8_t *expected = (uint8_t *)list.data +
            list.item_size * (list.length - 1);
        assert_bytes_match(expected, val, list.item_size);
    } else {
        /* Failure: val must be unchanged */
        assert_bytes_match(old_val, val, list.item_size);
    }

    /* 6. Unchanged fields (both paths) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert_array_list_equivalence(&list, &old, &storage);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
    free(old_val);
}
