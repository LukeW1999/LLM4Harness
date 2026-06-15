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

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate writable output buffer */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 2a. Save a copy of the output buffer for the failure case */
    uint8_t *old_val = malloc(list.item_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, list.item_size);

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        /* The output buffer must contain the last element */
        size_t last_offset = list.item_size * (list.length - 1);
        assert_bytes_match(val,
                           (const uint8_t *)list.data + last_offset,
                           list.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        /* The output buffer must be unchanged */
        assert_bytes_match(val, old_val, list.item_size);
    }

    /* 6. The list must always satisfy its validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Cleanup (not required for CBMC but keeps the harness tidy) */
    free(val);
    free(old_val);
}
