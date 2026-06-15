#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now equal the source value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* Length updates only when index is beyond the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that must remain unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Capacity may grow but must never shrink */
        assert(list.current_size >= old.current_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* Verify that the unchanged byte from the original buffer is still the same */
        if (old.data) {
            assert_byte_from_buffer_matches(old.data, &old_byte);
        }
    }

    /* 7. The list must always satisfy its validity invariant */
    assert(aws_array_list_is_valid(&list));
}
