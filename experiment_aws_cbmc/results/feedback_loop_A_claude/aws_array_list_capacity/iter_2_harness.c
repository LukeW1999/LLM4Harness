#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 4
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

void aws_array_list_capacity_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;
    size_t old_length = list.length;

    /* 3. Call function under test */
    size_t capacity = aws_array_list_capacity(&list);

    /* 4. Assert postconditions */

    /* The list must still be valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* The function must not modify any list fields */
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.alloc == old_alloc);
    assert(list.data == old_data);
    assert(list.length == old_length);

    /* Capacity is defined as current_size / item_size when item_size > 0 */
    if (list.item_size > 0) {
        assert(capacity == list.current_size / list.item_size);
    }

    /* Capacity must be >= length (can't have more elements than capacity) */
    assert(capacity >= list.length);
}
