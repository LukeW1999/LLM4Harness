#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic index, bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION * 2);

    /* 3. Allocate a readable source value */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* make the memory readable – fill with nondet bytes */
    for (size_t i = 0; i < list.item_size; ++i) {
        ((uint8_t *)val)[i] = nondet_uint8_t();
    }

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (list.data != NULL && list.item_size > 0) {
        size_t offset = nondet_size_t();
        __CPROVER_assume(offset < list.current_size * list.item_size);
        save_byte_from_array(
            (const uint8_t *)list.data + offset,
            list.item_size,
            &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length may increase by one if we wrote past the previous end */
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }

        /* the element at the given index must now equal the supplied value */
        assert_bytes_match(
            (const uint8_t *)list.data + (index * list.item_size),
            (const uint8_t *)val,
            list.item_size);
    } else {
        /* on failure the whole list must be unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 7. Fields that must never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
