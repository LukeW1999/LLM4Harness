#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 3. Non-deterministic index */
    size_t index;

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (< length) */
        assert(index < old_length);
        /* length decreases by 1 */
        assert(list.length == old_length - 1);
        /* item_size unchanged */
        assert(list.item_size == old_item_size);
        /* alloc unchanged */
        assert(list.alloc == old_alloc);
        /* current_size unchanged (erase doesn't reallocate) */
        assert(list.current_size == old_current_size);
        /* data pointer unchanged */
        assert(list.data == old_data);
    } else {
        /* On failure: index >= length, list is unchanged */
        assert(index >= old_length);
        assert(list.length == old_length);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
