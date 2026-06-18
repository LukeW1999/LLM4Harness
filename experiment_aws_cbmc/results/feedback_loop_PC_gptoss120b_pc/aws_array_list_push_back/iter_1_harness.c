#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 3. Allocate a readable source buffer for the value */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* make the buffer readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (old.length > 0 && old.data != NULL) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now contain the bytes from val */
        assert(list.data != NULL);
        assert(val != NULL);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* Length updates */
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* Capacity may grow but never shrink */
        assert(list.current_size >= old.current_size);

        /* All bytes except the overwritten element must stay the same */
        if (list.current_size > 0 && old.data != NULL && list.data != NULL) {
            size_t i;
            for (i = 0; i < list.current_size; ++i) {
                bool in_changed_region = (i >= index * list.item_size) &&
                                         (i < (index + 1) * list.item_size);
                if (!in_changed_region) {
                    assert(((uint8_t *)list.data)[i] == ((uint8_t *)old.data)[i]);
                }
            }
        }
    } else {
        /* Failure: the list must be unchanged */
        assert(result == AWS_OP_ERR);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        if (list.current_size > 0 && old.data != NULL && list.data != NULL) {
            size_t i;
            for (i = 0; i < list.current_size; ++i) {
                assert(((uint8_t *)list.data)[i] == ((uint8_t *)old.data)[i]);
            }
        }
    }

    /* 7. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
