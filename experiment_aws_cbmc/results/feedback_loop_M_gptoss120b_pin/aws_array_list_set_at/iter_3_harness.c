#include <aws/common/array_list.h>
#include <aws/common/math.h>
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
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Nondeterministic index, allow both success and overflow paths */
    size_t index = nondet_size_t();
    size_t max_index = (SIZE_MAX / list.item_size) + 10U;
    __CPROVER_assume(index <= max_index);

    /* 3. Allocate a readable buffer for the value to set */
    uint8_t *val_buf = malloc(list.item_size);
    __CPROVER_assume(val_buf != NULL);
    /* contents of val_buf are nondeterministic */

    /* 4. Save old state before the call */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that must stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* Length may increase to index+1 if index was beyond the old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* The data at the target index must now equal the supplied value */
        assert(list.data != NULL);
        assert_bytes_match(
            (uint8_t *)list.data + (list.item_size * index),
            val_buf,
            list.item_size);
    } else {
        /* On failure the whole structure must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));
}
