#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a nondet buffer for the value to push */
    uint8_t *val_buf = NULL;
    if (list.item_size > 0) {
        val_buf = malloc(list.item_size);
        __CPROVER_assume(val_buf != NULL);
    }
    const void *val = (const void *)val_buf;

    /* 3. Save old state before the call */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* Length must increase by one */
        assert(list.length == old.length + 1);
        /* The first element must now contain the pushed value */
        if (list.item_size > 0) {
            assert_bytes_match((uint8_t *)list.data, (const uint8_t *)val, list.item_size);
        }
    } else {
        /* On failure the list must be unchanged */
        assert_array_list_equivalence(&list, &old, &old_bytes);
    }

    /* 6. Fields that must never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 7. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val_buf);
}
