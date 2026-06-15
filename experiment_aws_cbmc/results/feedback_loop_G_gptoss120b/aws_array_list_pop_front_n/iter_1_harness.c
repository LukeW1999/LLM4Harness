#include <assert.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

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
    size_t old_length = old.length;
    size_t old_item_size = old.item_size;
    void *old_data = old.data;

    /* 3. Nondeterministic n (number of elements to pop) */
    size_t n = nondet_size_t();

    /* 4. Call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Post‑condition: length updates correctly */
    if (n >= old_length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_length - n);
        /* 6. The remaining bytes are preserved and shifted to the front */
        size_t remaining_bytes = list.length * old_item_size;
        assert_bytes_match((uint8_t *)list.data,
                           (uint8_t *)old_data + n * old_item_size,
                           remaining_bytes);
    }

    /* 7. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 8. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));
}
