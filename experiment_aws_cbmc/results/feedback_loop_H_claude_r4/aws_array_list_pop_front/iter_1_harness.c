#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list was non-empty, front element was removed */
        /* length decreases by 1 */
        assert(list.length == old.length - 1);
        /* item_size unchanged */
        assert(list.item_size == old.item_size);
        /* current_size unchanged */
        assert(list.current_size == old.current_size);
        /* allocator unchanged */
        assert(list.alloc == old.alloc);
        /* data pointer unchanged */
        assert(list.data == old.data);
    } else {
        /* Failure: list was empty, AWS_ERROR_LIST_EMPTY raised */
        /* length was 0 and remains 0 */
        assert(old.length == 0);
        assert(list.length == 0);
        /* all fields unchanged */
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    }

    /* 5. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
