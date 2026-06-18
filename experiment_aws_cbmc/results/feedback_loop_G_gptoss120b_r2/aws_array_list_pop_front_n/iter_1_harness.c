#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 3. Nondeterministic n */
    size_t n = nondet_size_t();

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Post‑conditions */

    /* Length changes */
    if (n >= old.length) {
        /* When n is greater than or equal to the current length the list is cleared */
        assert(list.length == 0);
    } else {
        /* Otherwise the length is reduced by n */
        assert(list.length == old.length - n);
        if (n > 0) {
            /* The remaining items are shifted to the front */
            size_t remaining_items = old.length - n;
            size_t remaining_bytes = remaining_items * list.item_size;
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data + n * list.item_size,
                               remaining_bytes);
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
