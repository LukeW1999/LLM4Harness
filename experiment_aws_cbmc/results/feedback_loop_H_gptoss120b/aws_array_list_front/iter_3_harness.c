#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_front_harness() {
    struct aws_array_list list;

    /* assume the list structure is bounded and has allocated data */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* choose a length that fits within the allocated buffer */
    size_t max_len = list.current_size / list.item_size;
    size_t len;
    __CPROVER_assume(len <= max_len);
    list.length = len;               /* may be zero or non‑zero */

    /* save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* call the function under test */
    void *result = aws_array_list_front(&list);

    /* post‑condition checks */
    if (old.length > 0) {
        assert(result == old.data);
    } else {
        assert(result == NULL);
    }

    /* the list must remain unchanged */
    assert_array_list_equivalence(&list, &old, &old_byte);
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
