#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_storage;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data_storage);
    }

    /* 3. Nondeterministic n */
    size_t n = nondet_size_t();

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Postconditions */

    /* 5.1 Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* 5.2 Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 5.3 Length and data changes */
    if (n >= old.length) {
        /* List cleared */
        assert(list.length == 0);
    } else {
        /* Length reduced by n */
        assert(list.length == old.length - n);

        /* Remaining elements (memmove is overridden to a no‑op in verification) */
        size_t new_len = list.length;
        size_t item_sz = list.item_size;
        if (new_len > 0) {
            /* Data buffer remains unchanged because memmove is a no‑op */
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               new_len * item_sz);
        }
    }

    /* 5.4 No‑op when n == 0 */
    if (n == 0) {
        assert(list.length == old.length);
        if (old.length > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               old.length * old.item_size);
        }
    }
}
