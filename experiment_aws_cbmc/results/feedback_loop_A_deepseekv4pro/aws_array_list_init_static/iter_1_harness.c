#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list list;
    size_t item_count;
    size_t item_size;
    void *raw_array;

    /* Bounded non-deterministic inputs */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    /* Ensure multiplication does not overflow */
    __CPROVER_assume(item_count <= SIZE_MAX / item_size);

    size_t total_size = item_count * item_size;
    raw_array = malloc(total_size);
    __CPROVER_assume(raw_array != NULL);

    /* Save a copy of the raw array to check immutability */
    uint8_t *old_data = malloc(total_size);
    __CPROVER_assume(old_data != NULL);
    memcpy(old_data, raw_array, total_size);

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Postcondition: the list is valid */
    assert(aws_array_list_is_valid(&list));

    /* Check that all fields are set as specified */
    assert(list.alloc == NULL);
    assert(list.current_size == total_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* The raw array content must not have been modified */
    assert_bytes_match(old_data, (const uint8_t *)raw_array, total_size);

    /* Clean up */
    free(raw_array);
    free(old_data);
}
