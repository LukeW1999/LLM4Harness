#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

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

    /* 3. Prepare a readable input value */
    /* item_size is guaranteed > 0 by validity predicate */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Choose a nondeterministic index and ensure no overflow on multiplication */
    size_t index = nondet_size_t();
    size_t offset;
    __CPROVER_assume(aws_mul_size_checked(index, list.item_size, &offset) == AWS_OP_SUCCESS);

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length is updated to max(old.length, index+1) */
        size_t expected_length = (index + 1 > old.length) ? (index + 1) : old.length;
        assert(list.length == expected_length);

        /* the stored element matches the input value */
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data + offset, val, list.item_size);

        /* fields that must stay the same */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* current_size may grow but never shrink */
        assert(list.current_size >= old.current_size);
    } else {
        /* on failure the list must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Invariant: the list remains valid */
    assert(aws_array_list_is_valid(&list));
}
