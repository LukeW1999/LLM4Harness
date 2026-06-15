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
    struct store_byte_from_buffer old_data_byte;
    if (list.data != NULL && list.item_size > 0 && list.length > 0) {
        save_byte_from_array((uint8_t *)list.data,
                             list.length * list.item_size,
                             &old_data_byte);
    }

    /* 3. Nondeterministic n, bounded */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 6. Length changes */
    if (n >= old.length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old.length - n);
    }

    /* 7. Weakened data movement check */
    if (n < old.length && old.data != NULL && old.item_size > 0) {
        size_t remaining_items = old.length - n;
        /* At least one item remains, check the first remaining item */
        if (remaining_items > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data + n * old.item_size,
                               old.item_size);
        }
    }

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
