#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Nondeterministic index (bounded to keep state space reasonable) */
    size_t idx = nondet_size_t();
    __CPROVER_assume(idx < (MAX_INITIAL_ITEM_ALLOCATION * 2));

    /* 4. Allocate a readable source buffer for the value to set */
    uint8_t *val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* make the buffer readable – CBMC treats malloced memory as readable */
    /* optionally nondet-initialize the buffer */
    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, idx);

    /* 6. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* length must be max(old.length, idx + 1) */
        size_t expected_len = (old.length > idx) ? old.length : (idx + 1);
        assert(list.length == expected_len);

        /* the element at idx must now equal the supplied value */
        assert_bytes_match((uint8_t *)list.data + (idx * list.item_size),
                           val,
                           list.item_size);
    } else {
        /* on failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Fields that never change regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 8. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
