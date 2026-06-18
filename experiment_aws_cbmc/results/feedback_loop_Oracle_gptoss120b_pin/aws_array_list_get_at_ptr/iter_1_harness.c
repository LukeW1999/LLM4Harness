#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet initialization parameters within bounds */
    size_t item_size;
    size_t initial_allocation;

    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* initialize list in dynamic mode */
    aws_array_list_init_dynamic(&list, alloc, initial_allocation, item_size);

    /* nondet length respecting capacity */
    size_t capacity = aws_array_list_capacity(&list);
    size_t length;
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* nondet index */
    size_t index;

    /* output pointer */
    void *out = NULL;
    void **val = &out;

    /* pre‑condition assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(val);

    /* capture pre‑state for frame conditions */
    size_t pre_length        = list.length;
    size_t pre_current_size  = list.current_size;
    size_t pre_item_size     = list.item_size;
    void *pre_data           = list.data;
    void *pre_out            = out;

    uint8_t *pre_buffer = NULL;
    if (list.data && list.current_size > 0) {
        pre_buffer = malloc(list.current_size);
        __CPROVER_assume(pre_buffer);
        memcpy(pre_buffer, list.data, list.current_size);
    }

    /* call the function under verification */
    int ret = aws_array_list_get_at_ptr(&list, val, index);

    /* post‑condition: return value correctness */
    if (list.length > index) {
        __CPROVER_assert(ret == AWS_OP_SUCCESS,
                         "aws_array_list_get_at_ptr returns success when index is valid");
        __CPROVER_assert(*val == (void *)((uint8_t *)list.data + (list.item_size * index)),
                         "*val points to the correct element address");
    } else {
        __CPROVER_assert(ret != AWS_OP_SUCCESS,
                         "aws_array_list_get_at_ptr returns error when index is invalid");
        __CPROVER_assert(out == pre_out,
                         "output pointer unchanged on error");
    }

    /* post‑condition: list invariants unchanged */
    __CPROVER_assert(list.length == pre_length, "list.length unchanged");
    __CPROVER_assert(list.current_size == pre_current_size, "list.current_size unchanged");
    __CPROVER_assert(list.item_size == pre_item_size, "list.item_size unchanged");
    __CPROVER_assert(list.data == pre_data, "list.data pointer unchanged");

    /* post‑condition: memory frame (data buffer) unchanged */
    if (pre_buffer) {
        __CPROVER_assert(memcmp(pre_buffer, list.data, list.current_size) == 0,
                         "list data buffer unchanged");
    }

    return 0;
}
