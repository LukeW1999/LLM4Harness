#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_array_list old = list;

    /* 3. Allocate val buffer of item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Assert postconditions */

    /* On success: list had at least one element, val was written with last element */
    if (result == AWS_OP_SUCCESS) {
        /* list must have had length > 0 */
        assert(old.length > 0);
        /* val now contains the last element — we can check memory is readable */
        assert(AWS_MEM_IS_READABLE(val, list.item_size));
    } else {
        /* On failure: list was empty */
        assert(old.length == 0);
    }

    /* 6. Assert unchanged fields — aws_array_list_back is read-only on the list */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
