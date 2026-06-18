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
    /* data must be non‑NULL when length > 0 */
    __CPROVER_assume(list.length == 0 || list.data != NULL);

    /* 2. Non‑deterministic n */
    size_t n = nondet_size_t();

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_storage;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data_storage);
    }

    /* 4. Constrain n */
    if (old.item_size != 0) {
        __CPROVER_assume(n <= SIZE_MAX / old.item_size);
    }
    __CPROVER_assume(n <= old.length);

    /* Additional assumptions to help CBMC reason about pointer arithmetic */
    __CPROVER_assume(old.current_size >= old.length * old.item_size);
    __CPROVER_assume(old.current_size >= n * old.item_size);
    __CPROVER_assume(old.current_size >= (old.length - n) * old.item_size);

    /* 5. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 6. Unchanged fields (frame) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 7. Post‑conditions on length and data */
    if (n >= old.length) {
        /* List cleared */
        assert(list.length == 0);
        if (list.data != NULL && old.length * old.item_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data_storage);
        }
    } else {
        if (n == 0) {
            /* No change in length */
            assert(list.length == old.length);
        } else {
            size_t new_len = old.length - n;
            assert(list.length == new_len);
            if (list.data != NULL && new_len * old.item_size > 0) {
                assert_bytes_match((uint8_t *)list.data,
                                   (uint8_t *)old.data + n * old.item_size,
                                   new_len * old.item_size);
            }
        }
    }

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
