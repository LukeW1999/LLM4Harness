#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (list.data && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* 2. Prepare inputs */
    /* Allocate a readable buffer for val */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* make the buffer readable – nondet contents are fine */
    (void)memset(val, 0, list.item_size);

    /* Nondet index, bounded to keep state space reasonable */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= (MAX_INITIAL_ITEM_ALLOCATION * 2));

    /* 3. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now equal *val* */
        assert(memcmp((uint8_t *)list.data + (index * list.item_size),
                      val,
                      list.item_size) == 0);

        /* Length is the maximum of the old length and index+1 */
        size_t expected_len = (old.length > index) ? old.length : (index + 1);
        assert(list.length == expected_len);

        /* Capacity (current_size) cannot shrink */
        assert(list.current_size >= old.current_size);

        /* All other bytes that were not overwritten must stay the same */
        assert_array_list_equivalence(&list, &old, &old_byte);
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 5. Fields that never change regardless of outcome */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
