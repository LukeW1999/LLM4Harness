#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

unsigned char nondet_uchar(void);

void aws_array_list_init_static_harness(void) {
    /* Allocate the list structure */
    struct aws_array_list *list = malloc(sizeof *list);
    __CPROVER_assume(list != NULL);

    /* Nondeterministic but bounded inputs */
    size_t initial_item_allocation;
    size_t item_size;

    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &initial_item_allocation * item_size));

    /* Compute the total size of the raw array */
    size_t raw_len;
    __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &raw_len));

    /* Allocate and nondeterministically initialize the raw array */
    void *raw_array = malloc(raw_len);
    __CPROVER_assume(raw_array != NULL);
    unsigned char *raw_bytes = (unsigned char *)raw_array;
    for (size_t i = 0; i < raw_len; ++i) {
        raw_bytes[i] = nondet_uchar();
    }

    /* Keep a copy of the raw array to check the frame condition */
    unsigned char *raw_copy = malloc(raw_len);
    __CPROVER_assume(raw_copy != NULL);
    memcpy(raw_copy, raw_bytes, raw_len);

    /* Call the function under verification */
    aws_array_list_init_static(list, raw_array, initial_item_allocation, item_size);

    /* Post‑condition checks */

    /* 1. Validity predicate */
    assert(aws_array_list_is_valid(list));

    /* 2. Length / capacity invariants */
    assert(list->length == 0);
    assert(list->item_size == item_size);
    assert(list->current_size == raw_len);
    assert(list->data == raw_array);
    assert(list->alloc == NULL);
    assert(aws_array_list_capacity(list) == initial_item_allocation);

    /* 3. Frame condition: raw array contents unchanged */
    assert(memcmp(raw_array, raw_copy, raw_len) == 0);

    return 0;
}
