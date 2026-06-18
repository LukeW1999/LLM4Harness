#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source value of the appropriate item size */
    size_t item_sz = list.item_size;
    __CPROVER_assume(item_sz > 0);
    __CPROVER_assume(item_sz <= MAX_ITEM_SIZE);
    uint8_t val_buf[MAX_ITEM_SIZE];
    uint8_t *val = val_buf;
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));

    /* 3. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* item size and allocator must stay the same */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* the value at the given index must now equal the input value */
        if (list.length > index) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               val,
                               list.item_size);
        }

        /* length updates only when writing past the previous end */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure the whole structure (including its contents) must be unchanged */
        assert_array_list_equivalence(&list, &old, &old_bytes);
    }

    /* 7. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
