#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Allocate and initialize a nondeterministic value buffer */
    size_t item_size = list.item_size;
    __CPROVER_assume(item_size > 0);
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 4. Nondeterministic index, bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length is increased to index+1 if index was beyond the old length */
        size_t expected_length = (index >= old.length) ? (index + 1) : old.length;
        assert(list.length == expected_length);

        /* item_size and allocator must stay the same */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* the element at the given index must now match the supplied value */
        assert_bytes_match(
            (uint8_t *)list.data + (list.item_size * index),
            val,
            list.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }

    /* 7. Invariant that the list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* 8. Clean up */
    free(val);
}
