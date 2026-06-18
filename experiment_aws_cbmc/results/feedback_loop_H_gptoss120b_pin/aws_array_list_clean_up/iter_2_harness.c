#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_clean_up_harness() {
    /* 1. Declare the data structure */
    struct aws_array_list list;

    /* 2. Use the default allocator */
    list.alloc = aws_default_allocator();

    /* 3. Assume the list is bounded and valid */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 4. Save old state (not used but kept for completeness) */
    struct aws_array_list old = list;

    /* 5. Call function under test */
    aws_array_list_clean_up(&list);

    /* 6. Assert postconditions: all fields are zeroed */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
}
