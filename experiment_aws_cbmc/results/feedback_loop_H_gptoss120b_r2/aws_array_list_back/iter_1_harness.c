#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate writable output buffer */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Save old contents of val (for failure case) */
    uint8_t *old_val_bytes = NULL;
    if (list.item_size > 0) {
        old_val_bytes = malloc(list.item_size);
        __CPROVER_assume(old_val_bytes != NULL);
        memcpy(old_val_bytes, val, list.item_size);
    }

    /* 5. Save expected last element (if any) */
    uint8_t *expected_last = NULL;
    if (list.length > 0 && list.item_size > 0) {
        expected_last = malloc(list.item_size);
        __CPROVER_assume(expected_last != NULL);
        size_t last_offset = list.item_size * (list.length - 1);
        memcpy(expected_last,
               (uint8_t *)list.data + last_offset,
               list.item_size);
    }

    /* 6. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* val must contain the copied last element */
        if (list.length > 0 && list.item_size > 0) {
            assert_bytes_match((uint8_t *)val, expected_last, list.item_size);
        }
        /* list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* on failure, list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        /* val must be unchanged */
        if (list.item_size > 0) {
            assert_bytes_match((uint8_t *)val, old_val_bytes, list.item_size);
        }
    }

    /* 8. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
    free(old_val_bytes);
    free(expected_last);
}
