#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_pop_front_n_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic n, bounded */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Save old state and a copy of the data buffer */
    struct aws_array_list old = list;
    uint8_t *old_data_copy = NULL;
    if (old.data != NULL && old.current_size > 0) {
        old_data_copy = malloc(old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old.data, old.current_size);
    }

    /* 4. Call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Post‑condition checks */
    if (n >= old.length) {
        /* n greater than or equal to current length → list cleared */
        assert(list.length == 0);
    } else if (n == 0) {
        /* No elements removed → list unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* Elements removed, length decreased */
        size_t expected_len = old.length - n;
        assert(list.length == expected_len);

        /* Unchanged fields when elements remain */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* Verify that the remaining elements have been shifted forward */
        size_t item_sz = list.item_size;
        __CPROVER_assume(item_sz == 0 || n <= SIZE_MAX / item_sz);
        __CPROVER_assume(expected_len == 0 || expected_len <= SIZE_MAX / item_sz);

        size_t remaining_bytes = expected_len * item_sz;
        uint8_t *new_data = (uint8_t *)list.data;
        size_t offset = n * item_sz;

        for (size_t i = 0; i < remaining_bytes; ++i) {
            assert(new_data[i] == old_data_copy[i + offset]);
        }
    }

    /* 6. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));

    /* 7. Clean up */
    free(old_data_copy);
}
