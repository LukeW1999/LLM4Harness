#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;
    size_t initial_item_allocation;
    size_t item_size;
    size_t len;
    void *raw_array;

    /* nondet inputs */
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &len));

    /* allocate raw array */
    raw_array = malloc(len);
    __CPROVER_assume(raw_array != NULL);

    /* keep a copy for frame condition */
    uint8_t *raw_array_copy = malloc(len);
    __CPROVER_assume(raw_array_copy != NULL);
    memcpy(raw_array_copy, raw_array, len);

    /* call the function under test */
    aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);

    /* postconditions */
    assert(aws_array_list_is_valid(&list));
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.current_size == len);
    assert(list.alloc == NULL);
    assert(list.data == raw_array);
    assert(aws_array_list_capacity(&list) == initial_item_allocation);
    assert(memcmp(raw_array, raw_array_copy, len) == 0);

    /* clean up */
    free(raw_array);
    free(raw_array_copy);
    return 0;
}
