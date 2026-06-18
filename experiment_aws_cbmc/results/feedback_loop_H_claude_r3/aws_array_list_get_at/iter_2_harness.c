#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Allocate val with item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc unchanged */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    /* length unchanged */
    assert(list.length == old_list.length);
    /* current_size unchanged */
    assert(list.current_size == old_list.current_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success: index must be < length */
        assert(index < list.length);
        /* val should be readable */
        assert(AWS_MEM_IS_READABLE(val, list.item_size));
    } else {
        /* On failure: index >= length */
        assert(index >= list.length);
    }
}
