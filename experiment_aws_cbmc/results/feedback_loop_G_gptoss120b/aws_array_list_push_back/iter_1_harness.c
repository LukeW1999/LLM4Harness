#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* bound item_size for later use */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* 2. Prepare a readable input value */
    uint8_t val_buf[MAX_ITEM_SIZE];
    /* make the contents nondeterministic */
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }
    const void *val = (const void *)val_buf;
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();
    /* keep the index within a reasonable range */
    __CPROVER_assume(index <= (MAX_INITIAL_ITEM_ALLOCATION * 2));

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length is increased to at least index+1 */
        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);

        /* the bytes at the target index now equal the input value */
        if (list.data != NULL && list.item_size > 0) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               val_buf,
                               list.item_size);
        }
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Fields that never change (regardless of success/failure) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 8. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
