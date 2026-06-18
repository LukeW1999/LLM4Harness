#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
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

    /* item_size must be non‑zero for the precondition on val */
    __CPROVER_assume(list.item_size > 0);

    /* 2. Allocate a readable source buffer for val */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* The allocated memory is readable/writable by construction */

    /* 3. Nondeterministic index, bounded to avoid overflow in multiplication */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= SIZE_MAX / list.item_size);

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage;
    if (index < old.length) {
        /* Save a byte from the element that may be overwritten */
        save_byte_from_array(
            (const uint8_t *)old.data + index * old.item_size,
            old.item_size,
            &storage);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length is updated if we wrote past the previous end */
        size_t expected_length = (index >= old.length) ? index + 1 : old.length;
        assert(list.length == expected_length);

        /* current_size (capacity in bytes) never shrinks */
        assert(list.current_size >= old.current_size);

        /* The data at the target index now matches the source buffer */
        assert_bytes_match(
            (const uint8_t *)list.data + index * list.item_size,
            val,
            list.item_size);

        /* Fields that must stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* data pointer may change in dynamic mode – no assertion */

    } else {
        /* On failure the whole structure must be unchanged */
        assert_array_list_equivalence(&list, &old, &storage);
    }

    /* 7. Invariant that the list remains valid */
    assert(aws_array_list_is_valid(&list));
}
