#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and initialise the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    __CPROVER_assume(
        aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic n */
    size_t n = nondet_size_t();

    /* 3. Save old state */
    struct aws_array_list old = list;
    size_t old_len        = old.length;
    size_t old_item_size = old.item_size;
    size_t old_current_size = old.current_size;
    void *old_data = old.data;

    struct store_byte_from_buffer old_bytes;
    if (old_data != NULL && old_current_size > 0) {
        save_byte_from_array((uint8_t *)old_data, old_current_size, &old_bytes);
    }

    /* 4. Constrain n to a sensible range (avoid overflow and out‑of‑bounds) */
    __CPROVER_assume(n <= old_len);
    if (old_item_size != 0) {
        __CPROVER_assume(n * old_item_size <= old_current_size);
    }

    /* 5. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 6. Post‑condition checks */

    /* Unchanged fields (always) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);

    if (n >= old_len) {
        /* n greater than or equal to length → list cleared */
        assert(list.length == 0);
    } else if (n == 0) {
        /* No elements removed → list unchanged */
        assert(list.length == old_len);
        if (old_data != NULL && old_current_size > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old_data,
                               old_current_size);
        }
    } else {
        /* 0 < n < old_len → elements shifted left */
        size_t remaining_items = old_len - n;
        size_t remaining_bytes = remaining_items * old_item_size;
        assert(list.length == remaining_items);
        if (old_data != NULL && remaining_bytes > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old_data + n * old_item_size,
                               remaining_bytes);
        }
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
