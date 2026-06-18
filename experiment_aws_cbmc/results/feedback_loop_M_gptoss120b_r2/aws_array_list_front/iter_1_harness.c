#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    uint8_t val_buf[MAX_ITEM_SIZE];
    /* Ensure the list's item size fits into our static buffer */
    __CPROVER_assume(list.item_size <= sizeof(val_buf));
    /* Make the source buffer nondeterministic */
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    /* 3. Nondeterministic index, bounded to avoid unbounded state space */
    size_t index = nondet_size_t();
    /* Upper bound: a reasonable multiple of the maximum initial allocation */
    __CPROVER_assume(index <= (MAX_INITIAL_ITEM_ALLOCATION * 2));
    /* Prevent overflow when computing byte offset */
    __CPROVER_assume(list.item_size == 0 ||
                     index <= (SIZE_MAX / list.item_size));

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (old.data && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now equal the source value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);

        /* Length handling */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* current_size may grow, data pointer may change – no assertion here */
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* If data was present, its contents must be unchanged */
        if (old.data && old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    /* 7. The list must always satisfy its validity invariant */
    assert(aws_array_list_is_valid(&list));
}
