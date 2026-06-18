#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 3. Call function under test */
    int result = aws_array_list_pop_back(&list);

    /* 4. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: list was non-empty, length decremented by 1 */
        assert(old_length > 0);
        assert(list.length == old_length - 1);
        /* current_size should not change on pop_back */
        assert(list.current_size == old_current_size);
    } else {
        /* On failure: list was empty */
        assert(old_length == 0);
        /* Nothing should have changed */
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
    }
}
