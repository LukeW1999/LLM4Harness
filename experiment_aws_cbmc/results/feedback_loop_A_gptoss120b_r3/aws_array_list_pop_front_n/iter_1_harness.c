#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_pop_front_n_harness(void) {
    /* 1. Declare and bound data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* Save a byte from the old data buffer for later comparison */
    struct store_byte_from_buffer old_byte;
    if (list.length > 0 && list.item_size > 0) {
        size_t old_buf_len = list.length * list.item_size;
        size_t idx = nondet_size_t();
        __CPROVER_assume(idx < old_buf_len);
        save_byte_from_array((uint8_t *)list.data + idx, 1, &old_byte);
    }

    /* Choose a nondeterministic n, bounded to keep state space reasonable */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 4. Assert postconditions */

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* Length changes according to specification */
    if (n >= old.length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old.length - n);
    }

    /* Data shift correctness for a byte that was captured */
    if (list.length > 0 && list.item_size > 0) {
        size_t new_buf_len = list.length * list.item_size;
        /* The captured index must lie within the region that remains after the pop */
        if (old.length > n) {
            size_t max_valid_idx = (old.length - n) * old.item_size;
            /* Re‑use the same nondeterministic idx captured earlier */
            size_t idx = (size_t)old_byte.buffer_offset; /* helper stores offset */
            __CPROVER_assume(idx < max_valid_idx);
            assert_byte_from_buffer_matches((uint8_t *)list.data + idx, &old_byte);
        }
    }

    /* 5. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
