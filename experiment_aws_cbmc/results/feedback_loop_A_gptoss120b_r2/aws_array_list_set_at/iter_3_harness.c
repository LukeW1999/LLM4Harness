#include <aws/common/array_list.h>
#include <aws/common/common.h>
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

    /* 2. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 3. Allocate a readable source buffer for the value to set */
    size_t item_sz = list.item_size;
    __CPROVER_assume(item_sz > 0);
    uint8_t *val = (uint8_t *)malloc(item_sz);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (index < old.length) {
        save_byte_from_array(
            (uint8_t *)old.data + old.item_size * index,
            old.item_size,
            &old_bytes);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* success: index must be within current length */
        assert(index < old.length);
        /* length does not change */
        assert(list.length == old.length);
        /* the stored value matches */
        assert_bytes_match(
            (uint8_t *)list.data + list.item_size * index,
            val,
            list.item_size);
        /* allocator and item size stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* failure: index is out of bounds */
        assert(index >= old.length);
        /* list must remain unchanged */
        assert_array_list_equivalence(&list, &old, &old_bytes);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    }

    /* 7. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
