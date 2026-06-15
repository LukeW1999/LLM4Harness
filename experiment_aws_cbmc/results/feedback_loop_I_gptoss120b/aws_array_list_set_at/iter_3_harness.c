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

    /* Ensure non‑zero item size and non‑empty list so that both success and
       failure paths are reachable. */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length > 0);

    /* 2. Allocate a readable input buffer `val` */
    size_t val_size = list.item_size;
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
    /* result must be either success or an error code */
    assert(result == AWS_OP_SUCCESS || result == AWS_ERROR_INVALID_INDEX);

    if (result == AWS_OP_SUCCESS) {
        /* The element at `index` must now equal the input value */
        assert(index < old.length);
        assert_bytes_match(
            (uint8_t *)list.data + list.item_size * index,
            (uint8_t *)val,
            list.item_size);
    } else {
        /* On failure the index must be out of bounds */
        assert(index >= old.length);
    }

    /* Length never changes */
    assert(list.length == old.length);

    /* Capacity (current_size) never changes */
    assert(list.current_size == old.current_size);

    /* Fields that never change */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /* Frame condition: unchanged memory outside the overwritten element */
    if (old.data != NULL && old.current_size > 0) {
        assert_byte_from_buffer_matches((uint8_t *)old.data, &old_storage);
    }

    /* 7. The list must remain valid in all cases */
    assert(aws_array_list_is_valid(&list));
}
