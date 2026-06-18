#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate a readable buffer for val */
    uint8_t *val_buf = NULL;
    if (list.item_size > 0) {
        val_buf = malloc(list.item_size);
        __CPROVER_assume(val_buf != NULL);
    }
    const void *val = val_buf;

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;
    /* Save a copy of the old data buffer for later comparison */
    uint8_t *old_data_copy = NULL;
    if (old.data != NULL && old.length > 0) {
        old_data_copy = malloc(old.length * old.item_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old.data, old.length * old.item_size);
    }

    /* 3. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Length increased by one */
        assert(list.length == old.length + 1);
        /* First element now equals val */
        assert(memcmp(list.data, val, list.item_size) == 0);
        /* Existing elements shifted right */
        if (old.length > 0) {
            assert(memcmp((uint8_t *)list.data + list.item_size,
                          old_data_copy,
                          old.length * list.item_size) == 0);
        }
    } else {
        /* On failure, the list must be unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val_buf);
    free(old_data_copy);
}
