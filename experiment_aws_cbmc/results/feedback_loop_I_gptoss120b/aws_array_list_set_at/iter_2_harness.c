#include <aws/common/array_list.h>
#include <aws/common/math.h>
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
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable input buffer `val` */
    size_t val_size = list.item_size;
    if (val_size == 0) {
        /* AWS_PRECONDITION requires a non‑NULL pointer even for zero size */
        val_size = 1;
    }
    void *val = malloc(val_size);
    __CPROVER_assume(val != NULL);
    /* contents are nondeterministic */

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state before the call */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_storage;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array((uint8_t *)old.data, old.current_size, &old_storage);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at `index` must now equal the input value */
        if (list.item_size > 0) {
            assert_bytes_match(
                (uint8_t *)list.data + list.item_size * index,
                (uint8_t *)val,
                list.item_size);
        }

        /* Length updates only when writing past the previous end */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Capacity (current_size) never shrinks */
        assert(list.current_size >= old.current_size);

        /* Fields that never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)old.data, &old_storage);
        }
    }

    /* 7. The list must remain valid in all cases */
    assert(aws_array_list_is_valid(&list));
}
