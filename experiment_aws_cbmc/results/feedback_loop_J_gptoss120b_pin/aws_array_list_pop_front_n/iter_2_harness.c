#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
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
    size_t old_bytes = old.length * old.item_size;
    uint8_t *old_data = NULL;
    if (old_bytes > 0) {
        old_data = malloc(old_bytes);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, old.data, old_bytes);
    }

    /* 3. Nondeterministic n, constrained to avoid overflow */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= old.length);
    if (old.item_size != 0) {
        __CPROVER_assume(n <= old_bytes / old.item_size);
    }

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 6. Length and data content changes */
    if (n >= old.length) {
        /* List should be cleared */
        assert(list.length == 0);
    } else {
        /* Length reduced by n */
        assert(list.length == old.length - n);

        /* Verify that the remaining elements are shifted correctly */
        size_t new_bytes = list.length * list.item_size;
        if (new_bytes > 0) {
            assert_bytes_match(
                (uint8_t *)list.data,
                old_data + n * old.item_size,
                new_bytes);
        }
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_data);
}
