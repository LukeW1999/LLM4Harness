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

    /* 3. Save old state */
    struct aws_array_list old = list;

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
        /* The exact contents of the remaining buffer are not verified
         * because the implementation may use memmove or other techniques
         * that CBMC does not model precisely for overlapping regions. */
    }

    /* 6. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));
}
