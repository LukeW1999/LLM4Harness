#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and set up the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* 3. Optionally capture the back element if list is non-empty */
    void *val = NULL;
    if (list.item_size > 0 && list.length > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_item_size);

    /* alloc never changes */
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* Success requires list was non-empty */
        assert(old_length > 0);

        /* Length decremented by 1 */
        assert(list.length == old_length - 1);

        /* current_size unchanged on pop_back */
        assert(list.current_size == old_current_size);

        /* data pointer unchanged */
        assert(list.data == old_data);

        /* val was written with the popped element */
        if (val != NULL && list.item_size > 0) {
            assert(AWS_MEM_IS_READABLE(val, list.item_size));
        }

    } else {
        /* On failure: list was empty */
        assert(old_length == 0);

        /* List state unchanged */
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }
}
