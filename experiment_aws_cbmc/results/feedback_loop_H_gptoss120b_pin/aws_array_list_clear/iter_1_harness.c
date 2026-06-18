#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
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

    /* 3. Prepare a readable input value */
    size_t item_sz = list.item_size;
    uint8_t *val_buf = NULL;
    if (item_sz > 0) {
        val_buf = malloc(item_sz);
        __CPROVER_assume(val_buf != NULL);
        for (size_t i = 0; i < item_sz; ++i) {
            val_buf[i] = nondet_uint8_t();
        }
    }

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index now matches the input value */
        if (list.data && item_sz > 0) {
            assert_bytes_match((uint8_t *)list.data + index * item_sz,
                               val_buf,
                               item_sz);
        }

        /* Length is increased only when index was beyond the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        if (list.data && old.data) {
            size_t min_sz = old.current_size < list.current_size ?
                            old.current_size : list.current_size;
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               min_sz);
        }
    }

    /* 7. Fields that never change regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 8. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val_buf);
}
