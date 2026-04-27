#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic n */
    size_t n;

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Assert postconditions */

    /* Unchanged fields */
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);

    /* Changed fields based on n vs old_length */
    if (n >= old_length) {
        /* List is cleared */
        assert(list.length == 0);
    } else {
        /* n < old_length */
        assert(list.length == old_length - n);
    }

    /* General: length must be <= old_length */
    assert(list.length <= old_length);

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
