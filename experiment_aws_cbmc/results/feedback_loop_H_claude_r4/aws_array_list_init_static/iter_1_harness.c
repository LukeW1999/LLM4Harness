#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness() {
    /* 1. Declare and set up the list */
    struct aws_array_list list;

    /* 2. Non-deterministic item_count and item_size with bounds */
    size_t item_count;
    size_t item_size;

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Ensure no overflow in item_count * item_size */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* 3. Allocate raw_array of appropriate size */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 4. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 5. Assert postconditions */

    /* Changed fields: */
    /* alloc should be NULL (static mode) */
    assert(list.alloc == NULL);

    /* current_size should be item_count * item_size */
    assert(list.current_size == current_size);

    /* item_size should be set */
    assert(list.item_size == item_size);

    /* length should be 0 */
    assert(list.length == 0);

    /* data should point to raw_array */
    assert(list.data == raw_array);

    /* 6. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* 7. Additional derived checks */
    /* capacity = current_size / item_size = item_count */
    assert(aws_array_list_capacity(&list) == item_count);

    /* length is 0 */
    assert(aws_array_list_length(&list) == 0);
}
