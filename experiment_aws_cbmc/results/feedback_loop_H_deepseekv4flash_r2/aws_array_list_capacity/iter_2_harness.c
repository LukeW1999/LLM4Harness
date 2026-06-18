#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 10
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 100
#endif

void aws_array_list_capacity_harness() {
    /* Non-deterministic list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.current_size > 0 && list.data) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    /* Call function */
    size_t capacity = aws_array_list_capacity(&list);
    size_t expected_capacity = list.current_size / list.item_size;

    /* Postconditions */
    assert(capacity == expected_capacity);
    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    if (list.current_size > 0 && list.data) {
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
    }
}
