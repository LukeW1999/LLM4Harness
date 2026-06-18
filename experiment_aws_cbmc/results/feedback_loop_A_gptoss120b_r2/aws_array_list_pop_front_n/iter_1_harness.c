#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_pop_front_n_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* Save a copy of the old data buffer for later comparison */
    uint8_t *old_data = NULL;
    if (old.length > 0 && old.data != NULL) {
        size_t old_bytes = old.length * old.item_size;
        old_data = malloc(old_bytes);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, old.data, old_bytes);
    }

    /* 3. Nondeterministic n (bounded) */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 6. Post‑conditions on length and data */
    if (n >= old.length) {
        /* List cleared */
        assert(list.length == 0);
    } else if (n == 0) {
        /* No change */
        assert(list.length == old.length);
        if (old.length > 0 && old.data != NULL) {
            size_t total_bytes = old.length * old.item_size;
            assert_bytes_match((uint8_t *)list.data, old_data, total_bytes);
        }
    } else {
        /* 0 < n < old.length */
        size_t expected_len = old.length - n;
        assert(list.length == expected_len);

        size_t popping_bytes = old.item_size * n;
        size_t remaining_bytes = expected_len * old.item_size;

        if (remaining_bytes > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               old_data + popping_bytes,
                               remaining_bytes);
        }
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_data);
}
