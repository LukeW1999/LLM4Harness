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
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before the call */
    struct aws_array_list old = list;

    /* Save a copy of the original data buffer for later comparison */
    uint8_t *old_data_copy = NULL;
    if (list.data != NULL && list.item_size > 0 && list.length > 0) {
        size_t old_bytes = list.item_size * list.length;
        old_data_copy = malloc(old_bytes);
        if (old_data_copy != NULL) {
            memcpy(old_data_copy, list.data, old_bytes);
        }
    }

    /* 3. Nondeterministic n */
    size_t n = nondet_size_t();

    /* 4. Call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Post‑condition assertions */

    /* Length and data movement */
    if (n >= old.length) {
        /* List should be cleared */
        assert(list.length == 0);
        /* Data pointer must stay the same */
        assert(list.data == old.data);
    } else {
        /* n < old.length */
        assert(list.length == old.length - n);
        assert(list.data == old.data);
        if (list.length > 0 && list.item_size > 0 && old_data_copy != NULL) {
            size_t moved_bytes = list.length * list.item_size;
            assert_bytes_match((uint8_t *)list.data,
                              old_data_copy + n * old.item_size,
                              moved_bytes);
        }
    }

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up the auxiliary copy */
    free(old_data_copy);
}
