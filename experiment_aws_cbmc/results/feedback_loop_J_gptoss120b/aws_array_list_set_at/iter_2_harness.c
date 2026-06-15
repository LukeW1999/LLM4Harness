#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Allocate a nondeterministic value buffer */
    uint8_t *val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        ((uint8_t *)val)[i] = nondet_uint8_t();
    }

    /* 3. Nondeterministic index, bounded to keep state space reasonable */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < (MAX_INITIAL_ITEM_ALLOCATION * 2));

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Length may increase if index was beyond the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* The element at the given index must now match the supplied value */
        assert(list.data != NULL);
        uint8_t *dest = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(dest, val, list.item_size);

        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
