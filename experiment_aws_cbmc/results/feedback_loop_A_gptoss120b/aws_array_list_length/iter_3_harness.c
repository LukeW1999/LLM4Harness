#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 8U
#define MAX_ITEM_SIZE               32U
#define MAX_INDEX                   16U

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Constrain item_size */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* 3. Constrain length to stay within a reasonable bound */
    __CPROVER_assume(list.length <= MAX_INDEX);

    /* 4. Prepare a readable input value */
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = (const void *)val_buf;
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 5. Non‑deterministic index within a reasonable bound */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INDEX);

    /* 6. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.data && list.current_size > 0) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_data);
    }

    /* 7. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 8. Basic result sanity */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* 9. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length may increase if index is beyond the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* the element at the given index must now equal the input value */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        assert_bytes_match(
            (const uint8_t *)list.data + (index * list.item_size),
            (const uint8_t *)val,
            list.item_size);

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* current_size may grow but never shrink */
        assert(list.current_size >= old.current_size);
    } else {
        /* on failure the whole structure must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert_array_list_equivalence(&list, &old, &old_data);
    }

    /* 10. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
