#include <assert.h>
#include "aws/common/array_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                              MAX_INITIAL_ITEM_ALLOCATION,
                                              MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(list.data, list.current_size, &old_bytes);

    /* 3. Nondeterministic n */
    size_t n = nondet_size_t();

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Post‑condition: length */
    if (n >= old.length) {
        /* list cleared */
        assert(list.length == 0);
    } else {
        /* length reduced by n */
        assert(list.length == old.length - n);
        if (n > 0) {
            /* remaining bytes are shifted forward unchanged */
            size_t remaining_items = old.length - n;
            size_t remaining_bytes = remaining_items * old.item_size;
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data + n * old.item_size,
                               remaining_bytes);
        } else {
            /* n == 0 → length unchanged, data unchanged */
            assert(list.length == old.length);
        }
    }

    /* 6. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
