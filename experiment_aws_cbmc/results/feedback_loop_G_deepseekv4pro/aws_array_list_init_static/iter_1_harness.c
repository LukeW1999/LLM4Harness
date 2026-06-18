#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

void aws_array_list_init_static_harness() {
    /* Non-deterministic input parameters */
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Bound inputs to keep state space small */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Ensure no overflow when computing total size */
    __CPROVER_assume(item_count <= SIZE_MAX / item_size);
    size_t total_size = item_count * item_size;

    /* Allocate raw array of total size */
    void *raw_array = malloc(total_size);
    /* Assume allocation succeeds (precondition of function) */
    __CPROVER_assume(raw_array != NULL);

    /* Save one byte of raw_array to check immutability */
    struct store_byte_from_buffer storage;
    save_byte_from_array(raw_array, total_size, &storage);

    /* Declare list */
    struct aws_array_list list;

    /* Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Assert postconditions */

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Specific field values */
    assert(list.alloc == NULL);
    assert(list.current_size == total_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* raw_array content should not be modified */
    assert_byte_from_buffer_matches(raw_array, &storage);

    /* Free the allocated raw_array (optional, not required for proof) */
    free(raw_array);
}
