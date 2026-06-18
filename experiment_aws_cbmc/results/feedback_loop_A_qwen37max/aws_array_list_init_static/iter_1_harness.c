#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    size_t item_count = nondet_size_t();
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    aws_array_list_init_static(list, raw_array, item_count, item_size);

    assert(list->alloc == NULL);
    assert(list->current_size == current_size);
    assert(list->item_size == item_size);
    assert(list->length == 0);
    assert(list->data == raw_array);
    
    assert(aws_array_list_is_valid(list));
}
