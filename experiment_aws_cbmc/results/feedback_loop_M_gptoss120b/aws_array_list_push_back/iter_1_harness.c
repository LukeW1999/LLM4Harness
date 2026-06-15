#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Non‑deterministic index, bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Allocate a readable buffer for val (size = item_size) */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* make the buffer readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Post‑condition checks */

    /* In all cases the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Length must be max(old.length, index+1) */
        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);

        /* The item at `index` must now equal the input `val` */
        if (list.data && list.item_size > 0 && index < list.length) {
            uint8_t *item_ptr = (uint8_t *)list.data + (index * list.item_size);
            assert_bytes_match(item_ptr, val, list.item_size);
        }

        /* Fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* `current_size` may change due to reallocation, so we do not assert it unchanged */
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }

    /* Clean up */
    free(val);
    return 0;
}
