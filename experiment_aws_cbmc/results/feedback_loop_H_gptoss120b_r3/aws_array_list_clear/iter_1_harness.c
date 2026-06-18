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

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    uint8_t *val = malloc(list.item_size ? list.item_size : 1);
    __CPROVER_assume(val != NULL);
    /* make the source buffer readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Unchanged fields (allocator and item size never change) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 7. Post‑condition handling for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* length may increase to index+1 if index was beyond the old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* capacity (current_size) never shrinks */
        assert(list.current_size >= old.current_size);

        /* the stored element must match the source value */
        assert(list.length > index);
        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                           val,
                           list.item_size);
    } else {
        /* On failure the list must remain unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 8. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
