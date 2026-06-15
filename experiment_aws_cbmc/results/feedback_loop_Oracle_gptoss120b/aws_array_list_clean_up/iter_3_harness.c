#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(allocator->mem_release != NULL);

    size_t item_size = (nondet_uint() % MAX_ITEM_SIZE) + 1;
    size_t initial_capacity = (nondet_uint() % MAX_INITIAL_ITEM_ALLOCATION) + 1;

    if (aws_array_list_init(&list, allocator, initial_capacity, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    aws_array_list_clean_up(&list);

    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
}
