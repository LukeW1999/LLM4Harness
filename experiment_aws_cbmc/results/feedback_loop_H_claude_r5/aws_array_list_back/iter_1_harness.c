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

    /* 2. Save old state before calling */
    struct aws_array_list old = list;

    /* 3. Allocate val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 4. Save a byte from the last element (if list is non-empty) for postcondition check */
    struct store_byte_from_buffer last_elem_byte;
    bool list_nonempty = (list.length > 0);
    if (list_nonempty) {
        size_t last_item_offset = list.item_size * (list.length - 1);
        save_byte_from_array((uint8_t *)list.data + last_item_offset, list.item_size, &last_elem_byte);
    }

    /* 5. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 6. Assert postconditions */

    /* On success: list was non-empty, val now contains the last element */
    if (result == AWS_OP_SUCCESS) {
        assert(list_nonempty);
        /* val should match the last element that was in the list */
        assert_byte_from_buffer_matches((uint8_t *)val, &last_elem_byte);
    } else {
        /* On failure: list was empty */
        assert(!list_nonempty);
        assert(list.length == 0);
    }

    /* 7. Assert unchanged fields — aws_array_list_back is a read-only operation */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
