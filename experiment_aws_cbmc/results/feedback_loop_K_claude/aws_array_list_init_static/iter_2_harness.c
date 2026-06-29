#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_init_static_harness(void) {
    /* Symbolic inputs */
    struct aws_array_list list;
    size_t item_count;
    size_t item_size;

    /* Preconditions */
    __CPROVER_assume(item_count > 0 && item_count <= 1024);
    __CPROVER_assume(item_size > 0 && item_size <= 1024);

    /* Ensure item_count * item_size does not overflow */
    size_t current_size_check;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size_check));
    __CPROVER_assume(current_size_check > 0 && current_size_check <= 1024 * 1024);

    /* Allocate raw_array with sufficient size */
    uint8_t *raw_array = malloc(current_size_check);
    __CPROVER_assume(raw_array != NULL);

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Postconditions (validity) */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == NULL);
    assert(list.data == raw_array);

    /* Postconditions (length/capacity) */
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.current_size == current_size_check);
    assert(aws_array_list_capacity(&list) == item_count);
}
