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
#include <string.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare the array list and set its allocator */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    /* 2. Bound the array list and ensure it has allocated data */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 3. Allocate a readable buffer for the value to push */
    uint8_t *val_buf = NULL;
    if (list.item_size > 0) {
        val_buf = malloc(list.item_size);
        __CPROVER_assume(val_buf != NULL);
        uint8_t pattern = nondet_uint8();
        for (size_t i = 0; i < list.item_size; ++i) {
            val_buf[i] = pattern;
        }
    }

    /* 4. Save old state and a copy of the old data */
    struct aws_array_list old = list;
    uint8_t *old_data_copy = NULL;
    if (old.length > 0) {
        size_t old_bytes = old.length * old.item_size;
        old_data_copy = malloc(old_bytes);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old.data, old_bytes);
    }

    /* 5. Call function under test */
    int result = aws_array_list_push_front(&list, val_buf);

    /* 6. If the call succeeded, check post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (list.item_size > 0) {
            assert_bytes_match((uint8_t *)list.data, val_buf, list.item_size);
        }

        if (old.length > 0) {
            assert_bytes_match((uint8_t *)list.data + list.item_size,
                               old_data_copy,
                               old.length * old.item_size);
        }

        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure the list must remain unchanged */
        assert(result == AWS_OP_ERR);
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(aws_array_list_is_valid(&list));
    }

    /* 7. Clean up */
    free(val_buf);
    free(old_data_copy);
}
