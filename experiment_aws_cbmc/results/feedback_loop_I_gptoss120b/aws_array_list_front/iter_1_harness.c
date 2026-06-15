#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* bound item_size for the local value buffer */
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.item_size > 0);

    /* 2. Prepare nondeterministic input value */
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    /* 3. Prepare nondeterministic index */
    size_t index = nondet_size_t();
    /* keep index within a reasonable range to avoid overflow in the model */
    __CPROVER_assume(index < (MAX_INITIAL_ITEM_ALLOCATION * 2));

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_bytes);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* the value at the given index must match the input */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);

        /* length updates only when index is beyond the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* on failure the list must remain unchanged */
        assert_array_list_equivalence(&list, &old, &old_bytes);
    }

    /* 7. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));
}
