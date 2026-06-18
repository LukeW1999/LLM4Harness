/*  
Contract for aws_array_list_front  

Preconditions:  
- __CPROVER_assume(list != NULL);  
- __CPROVER_assume(val != NULL);  
- __CPROVER_assume(aws_array_list_is_valid(list));  
- __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list->item_size));  

Postconditions (validity):  
- The function returns AWS_OP_SUCCESS iff aws_array_list_length(list) > 0.  
- If the list is non‑empty, the bytes copied into *val are exactly the first element of the list (AWS_BYTES_EQ).  
- The function never modifies the list structure or its data buffer (length, current_size, item_size, alloc, data remain unchanged).  
- The list remains valid after the call (aws_array_list_is_valid(list) holds).  
- If the list is empty, the function returns AWS_ERROR_LIST_EMPTY and does not modify *val.  
*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_front_harness(void) {
    /* Allocate and initialize a dynamic array list */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size (must be > 0) */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 1024); /* bound for CBMC */

    /* nondet initial allocation count */
    size_t init_count;
    __CPROVER_assume(init_count <= 64);

    /* Initialize the list */
    int init_res = aws_array_list_init_dynamic(&list, alloc, init_count, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    /* nondet length (must be <= capacity) */
    size_t capacity = aws_array_list_capacity(&list);
    size_t length;
    __CPROVER_assume(length <= capacity);
    list.length = length; /* set length directly, respecting invariants */

    /* Ensure the list is still valid after manual length set */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate a buffer for the front element */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Make a copy of the first element (if any) for later comparison */
    uint8_t *first_elem_copy = NULL;
    if (length > 0) {
        first_elem_copy = malloc(item_size);
        __CPROVER_assume(first_elem_copy != NULL);
        memcpy(first_elem_copy, list.data, item_size);
    }

    /* Save a snapshot of the list structure and data for frame condition checks */
    struct aws_array_list list_snapshot = list;
    uint8_t *data_snapshot = NULL;
    if (list.current_size > 0) {
        data_snapshot = malloc(list.current_size);
        __CPROVER_assume(data_snapshot != NULL);
        memcpy(data_snapshot, list.data, list.current_size);
    }

    /* Call the function under test */
    int ret = aws_array_list_front(&list, val);

    /* Postcondition: return value */
    if (length > 0) {
        assert(ret == AWS_OP_SUCCESS);
    } else {
        assert(ret == aws_raise_error(AWS_ERROR_LIST_EMPTY) || ret == AWS_ERROR_LIST_EMPTY);
    }

    /* Postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition: frame (list structure unchanged) */
    assert(list.length == list_snapshot.length);
    assert(list.current_size == list_snapshot.current_size);
    assert(list.item_size == list_snapshot.item_size);
    assert(list.alloc == list_snapshot.alloc);
    assert(list.data == list_snapshot.data);

    /* Postcondition: data buffer unchanged */
    if (list.current_size > 0) {
        assert(memcmp(list.data, data_snapshot, list.current_size) == 0);
    }

    /* Postcondition: val contains first element when list non‑empty */
    if (length > 0) {
        assert(memcmp(val, first_elem_copy, item_size) == 0);
    }

    /* Clean up */
    aws_array_list_clean_up(&list);
    free(val);
    free(first_elem_copy);
    free(data_snapshot);

    return 0;
}
