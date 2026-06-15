/*  
Contract for aws_array_list_pop_front  

Preconditions:  
- `list` points to a valid `struct aws_array_list` (aws_array_list_is_valid(list) == true).  
- The allocator used by the list is non‑NULL.  
- `list->item_size` > 0.  

Postconditions (validity):  
- After the call, `aws_array_list_is_valid(list)` must still hold.  

Postconditions (length):  
- If the list length before the call (`len_before`) > 0, the function returns `AWS_OP_SUCCESS` and the new length is `len_before - 1`.  
- If the list length before the call is 0, the function returns `AWS_ERROR_LIST_EMPTY` and the length remains 0.  

Postconditions (frame):  
- `list->alloc`, `list->item_size`, and the list capacity are unchanged.  
- No memory outside the list’s internal buffer is modified.  
*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet initial allocation and item size */
    size_t init_alloc = nondet_size_t();
    __CPROVER_assume(init_alloc <= 10);
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= 32);

    /* initialize list dynamically */
    int init_ret = aws_array_list_init_dynamic(&list, alloc, init_alloc, item_size);
    __CPROVER_assume(init_ret == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet initial length (cannot exceed allocated capacity) */
    size_t init_len = nondet_size_t();
    __CPROVER_assume(init_len <= init_alloc);

    /* fill the list with init_len elements */
    for (size_t i = 0; i < init_len; ++i) {
        uint8_t *elem = malloc(item_size);
        __CPROVER_assume(elem != NULL);
        for (size_t j = 0; j < item_size; ++j) {
            elem[j] = nondet_uint8_t();
        }
        int push_ret = aws_array_list_push_back(&list, elem);
        __CPROVER_assume(push_ret == AWS_OP_SUCCESS);
        free(elem);
    }

    /* capture state before the call */
    size_t len_before = aws_array_list_length(&list);
    size_t capacity_before = aws_array_list_capacity(&list);
    struct aws_allocator *alloc_before = list.alloc;
    size_t item_size_before = list.item_size;

    int ret = aws_array_list_pop_front(&list);

    /* postconditions on return value and length */
    if (len_before > 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == len_before - 1);
    } else {
        assert(ret == AWS_ERROR_LIST_EMPTY);
        assert(aws_array_list_length(&list) == 0);
    }

    /* frame conditions */
    assert(list.alloc == alloc_before);
    assert(list.item_size == item_size_before);
    assert(aws_array_list_capacity(&list) == capacity_before);
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    aws_array_list_clean_up(&list);
    return 0;
}
