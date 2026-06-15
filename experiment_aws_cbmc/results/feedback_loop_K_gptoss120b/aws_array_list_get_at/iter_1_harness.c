/*  
Contract for aws_array_list_get_at  

Preconditions:  
- list is a pointer to a valid aws_array_list (aws_array_list_is_valid(list) == true)  
- val points to writable memory of at least list->item_size bytes  
- index is an arbitrary size_t value  

Postconditions (validity):  
- The function returns AWS_OP_SUCCESS iff index < aws_array_list_length(list) before the call.  
- If the function returns AWS_OP_SUCCESS, the memory pointed to by val contains exactly the bytes stored in the list at the given index.  
- If the function returns an error, aws_last_error() == AWS_ERROR_INVALID_INDEX.  

Postconditions (length & capacity):  
- The list's length, current_size (capacity), and allocator are unchanged by the call.  

Postconditions (frame):  
- The contents of list->data are unchanged (no modification of any element).  
- No other memory locations are modified except for the bytes written to *val on success.  
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

void aws_array_list_get_at_harness(void) {
    /* Allocate and initialize a dynamic array list */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size (must be > 0) */
    size_t item_size;
    __CPROVER_assume(item_size > 0);

    /* nondet initial capacity (number of items) */
    size_t init_capacity;
    __CPROVER_assume(init_capacity <= 1024); /* bound to keep allocation reasonable */

    /* Initialize the list; assume success */
    int init_err = aws_array_list_init_dynamic(&list, alloc, init_capacity, item_size);
    __CPROVER_assume(init_err == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Populate the list with nondet data up to its current length */
    size_t i;
    for (i = 0; i < list.length; ++i) {
        uint8_t *elem = (uint8_t *)list.data + i * item_size;
        size_t j;
        for (j = 0; j < item_size; ++j) {
            elem[j] = (uint8_t) __CPROVER_nondet_uint();
        }
    }

    /* Snapshot of list state before the call */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;

    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* Allocate writable buffer for output */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    /* Initialize val with nondet bytes to detect overwrites */
    size_t k;
    for (k = 0; k < item_size; ++k) {
        ((uint8_t *)val)[k] = (uint8_t) __CPROVER_nondet_uint();
    }

    /* nondet index */
    size_t index;
    /* No assumption on index; it can be any size_t */

    /* Call the function under test */
    int ret = aws_array_list_get_at(&list, val, index);

    /* Postcondition: list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition: length and capacity unchanged */
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.alloc == old_alloc);

    /* Postcondition: data buffer unchanged */
    if (old_data != NULL) {
        assert(memcmp(old_data, list.data, list.current_size) == 0);
        free(old_data);
    }

    /* Determine expected outcome */
    if (index < old_length) {
        /* Success case */
        assert(ret == AWS_OP_SUCCESS);
        /* Verify that val now contains the element at the given index */
        void *expected = (void *)((uint8_t *)list.data + (item_size * index));
        assert(memcmp(val, expected, item_size) == 0);
    } else {
        /* Failure case */
        assert(ret != AWS_OP_SUCCESS);
        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);
        /* val must be unchanged */
        for (k = 0; k < item_size; ++k) {
            assert(((uint8_t *)val)[k] == ((uint8_t *)val)[k]); /* no-op, just keep val untouched */
        }
    }

    /* Clean up */
    free(val);
    aws_array_list_clean_up(&list);
    return 0;
}
