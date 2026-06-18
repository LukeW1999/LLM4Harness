#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the original list for later comparison */
    struct aws_array_list old = list;

    /* Save a byte from the original data buffer to detect unintended modifications */
    struct store_byte_from_buffer old_data_byte;
    if (list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data_byte);
    }

    /* 2. Prepare nondeterministic inputs, bounded appropriately */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Allocate a readable buffer for val, sized to the list's item size */
    size_t item_sz = list.item_size;
    __CPROVER_assume(item_sz > 0);
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 3. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length updates */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* The element at `index` now matches `val` */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* Fields that must stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        if (old.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data_byte);
        }
    }

    /* 5. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
