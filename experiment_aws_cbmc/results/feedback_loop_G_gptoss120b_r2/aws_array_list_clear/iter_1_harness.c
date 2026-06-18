#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
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

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare a readable input buffer */
    size_t item_sz = list.item_size;
    uint8_t *val_buf = NULL;
    if (item_sz > 0) {
        val_buf = malloc(item_sz);
        __CPROVER_assume(val_buf != NULL);
        for (size_t i = 0; i < item_sz; ++i) {
            val_buf[i] = nondet_uint8_t();
        }
    }
    const void *val = val_buf;

    /* Save a byte from the input buffer for later comparison */
    struct store_byte_from_buffer val_byte;
    if (item_sz > 0) {
        save_byte_from_array(val_buf, item_sz, &val_byte);
    }

    /* 4. Nondeterministic index (avoid overflow on index+1) */
    size_t index = nondet_size_t();
    __CPROVER_assume(index != SIZE_MAX);

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length may grow if we wrote past the previous end */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that must stay unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* Verify that the byte we saved from *val* was copied to the target slot */
        if (list.data && item_sz > 0) {
            uint8_t *target = (uint8_t *)list.data + (index * list.item_size);
            assert_byte_from_buffer_matches(target, &val_byte);
        }
    } else {
        /* On failure the whole structure must be unchanged */
        assert_array_list_equivalence(&list, &old, NULL);
    }

    /* 7. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val_buf);
}
