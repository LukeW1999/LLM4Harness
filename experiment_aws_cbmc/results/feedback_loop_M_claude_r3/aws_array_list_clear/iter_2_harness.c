#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_clear_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;

    /* Bound the list to prevent state space explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* 3. Call the function under test */
    aws_array_list_clear(&list);

    /* 4. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* length must be zero after clear */
    assert(list.length == 0);

    /* item_size never changes */
    assert(list.item_size == old_item_size);

    /* alloc never changes */
    assert(list.alloc == old_alloc);

    /* current_size never changes (clear does not free or reallocate) */
    assert(list.current_size == old_current_size);

    /* data pointer never changes */
    assert(list.data == old_data);
}
