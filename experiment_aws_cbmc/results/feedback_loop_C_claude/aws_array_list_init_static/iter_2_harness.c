#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_init_static_harness() {
    /* Stack-allocated list struct */
    struct aws_array_list list;

    /* Non-deterministic item_count and item_size, bounded to avoid overflow */
    size_t item_count;
    size_t item_size;
    __CPROVER_assume(item_count > 0 && item_count <= 4);
    __CPROVER_assume(item_size > 0 && item_size <= 4);

    /* Compute total size */
    size_t total_size = item_count * item_size;

    /* Allocate raw_array with the correct size */
    void *raw_array = malloc(total_size);
    __CPROVER_assume(raw_array != NULL);

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Verify all fields are set correctly */
    assert(list.alloc == NULL);
    assert(list.current_size == total_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
