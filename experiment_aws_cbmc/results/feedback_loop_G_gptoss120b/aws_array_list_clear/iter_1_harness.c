#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Declare a nondeterministic value buffer */
    uint8_t val_buf[MAX_ITEM_SIZE];
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    /* make the buffer readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val_buf, list.item_size));

    /* capture the bytes of the input value for later comparison */
    struct store_byte_from_buffer val_storage;
    save_byte_from_array(val_buf, list.item_size, &val_storage);

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length may grow to index+1 */
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);

        /* the element at the given index must now equal the input value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_storage.bytes,
                           list.item_size);
    } else {
        /* on failure the list must be unchanged */
        assert_array_list_equivalence(&list, &old, &val_storage);
    }

    /* 7. Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    /* data pointer may change only on success; if it changed on failure the
       equivalence check above already guarantees the whole structure is equal */

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
