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

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* 2. Prepare a readable input value */
    uint8_t *val_buf = malloc(list.item_size);
    __CPROVER_assume(val_buf != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val_buf, list.item_size));
    const void *val = val_buf;

    /* 3. Non‑deterministic index (avoid overflow in multiplication) */
    size_t index = nondet_size_t();
    __CPROVER_assume(list.item_size == 0 || index <= SIZE_MAX / list.item_size);

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The bytes at the target index must match the input value */
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           (const uint8_t *)val,
                           list.item_size);

        /* Length may increase by exactly one when index is beyond the old length */
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that must remain unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* current_size and data pointer may change in dynamic mode, so no assert */
    } else {
        /* On failure the list must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data) {
            assert_array_list_equivalence(&list, &old, &old_byte);
        }
    }

    /* 6. The list must always satisfy its validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val_buf);
}
