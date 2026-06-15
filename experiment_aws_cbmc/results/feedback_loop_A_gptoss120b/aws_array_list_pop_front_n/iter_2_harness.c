#include <aws/common/array_list.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    size_t old_data_bytes = old.length * old.item_size;
    uint8_t *old_data = NULL;
    if (old_data_bytes > 0) {
        old_data = malloc(old_data_bytes);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, old.data, old_data_bytes);
    }

    /* 3. Nondeterministic n, bounded to avoid overflow */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= old.length);

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Postconditions */

    /* Length changes */
    if (n == old.length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old.length - n);
        if (list.length > 0) {
            /* Remaining data should be shifted correctly */
            assert_bytes_match((uint8_t *)list.data,
                               old_data + n * old.item_size,
                               list.length * old.item_size);
        }
    }

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
