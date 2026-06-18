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

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate and initialize the value to be pushed */
    size_t val_size = list.item_size ? list.item_size : 1;
    void *val = malloc(val_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        ((uint8_t *)val)[i] = nondet_uint8_t();
    }

    /* 3. Save old state before the call */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_byte = {0};
    if (old.length > 0) {
        save_byte_from_array(old.data,
                             old.item_size * old.length,
                             &old_data_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. General invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length must increase by one */
        assert(list.length == old.length + 1);

        /* Allocator and item size must stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* The new first element must equal the value we pushed */
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(((uint8_t *)list.data)[i] == ((uint8_t *)val)[i]);
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        if (old.length > 0) {
            /* The underlying data buffer must be unchanged */
            assert_byte_from_buffer_matches(old.data, &old_data_byte);
        }
    }

    /* 7. Clean up */
    free(val);
}
