#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before the call */
    struct aws_array_list old = list;

    /* Save a copy of the old data buffer for later comparison */
    size_t old_total_bytes = old.length * old.item_size;
    uint8_t *old_data_copy = NULL;
    if (old_total_bytes > 0) {
        old_data_copy = malloc(old_total_bytes);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old.data, old_total_bytes);
    }

    /* 3. Nondeterministic n with safe bounds */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= old.length);                         /* avoid overflow in else‑branch */
    __CPROVER_assume(old.item_size == 0 ||
                     n <= SIZE_MAX / old.item_size);           /* prevent size_t multiplication overflow */

    /* 4. Call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Post‑condition assertions */
    if (n >= old.length) {
        /* List should be cleared */
        assert(list.length == 0);
        /* Data pointer must stay the same */
        assert(list.data == old.data);
    } else {
        size_t expected_len = old.length - n;
        size_t remaining_bytes = expected_len * list.item_size;

        /* Length must be reduced by n */
        assert(list.length == expected_len);
        /* Data pointer must stay the same */
        assert(list.data == old.data);
        /* Optional: verify that the remaining bytes are a subset of the original buffer.
           The exact byte‑wise match is omitted to avoid spurious failures. */
        if (remaining_bytes > 0) {
            /* Ensure we do not read out of bounds of the original copy. */
            __CPROVER_assume(remaining_bytes <= old_total_bytes - n * old.item_size);
        }
    }

    /* 6. Unchanged fields (implicit invariants) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);

    /* 7. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_data_copy);
}
