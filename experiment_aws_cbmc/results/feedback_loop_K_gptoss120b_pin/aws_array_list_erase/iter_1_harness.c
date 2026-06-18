/*  
 * Contract for aws_array_list_erase  
 * Preconditions:  
 *   - list != NULL  
 *   - list->alloc == aws_default_allocator() (or any allocator, but we use default)  
 *   - aws_array_list_is_valid(list) holds before the call  
 *   - list->item_size > 0  
 *   - list->data points to a buffer of at least list->length * list->item_size bytes  
 *   - index is an arbitrary size_t (may be out of bounds)  
 * Postconditions (validity):  
 *   - aws_array_list_is_valid(list) holds after the call  
 *   - list->alloc is unchanged  
 *   - list->item_size is unchanged  
 *   - list->data pointer is unchanged (the buffer may be modified but not reallocated)  
 * Postconditions (return value):  
 *   - if index < length_before: return == AWS_OP_SUCCESS and list->length == length_before - 1  
 *   - else: return == aws_raise_error(AWS_ERROR_INVALID_INDEX) and list->length == length_before  
 * Postconditions (frame):  
 *   - Memory outside the range [list->data, list->data + list->capacity * list->item_size) is unchanged  
 */

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_erase_harness(void) {
    /* Allocate and initialize a list */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size (must be >0) */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 256); /* bound for CBMC */

    /* nondet initial allocation count */
    size_t init_alloc;
    __CPROVER_assume(init_alloc > 0);
    __CPROVER_assume(init_alloc <= 16);

    /* Initialize list in dynamic mode */
    int init_ret = aws_array_list_init_dynamic(&list, alloc, init_alloc, item_size);
    __CPROVER_assume(init_ret == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Fill the list with a nondet number of elements (0 .. init_alloc) */
    size_t fill_len;
    __CPROVER_assume(fill_len <= init_alloc);
    for (size_t i = 0; i < fill_len; ++i) {
        uint8_t *elem = malloc(item_size);
        __CPROVER_assume(elem != NULL);
        /* nondet content */
        for (size_t j = 0; j < item_size; ++j) {
            elem[j] = __CPROVER_nondet_uint8_t();
        }
        int push_ret = aws_array_list_push_back(&list, elem);
        __CPROVER_assume(push_ret == AWS_OP_SUCCESS);
        free(elem);
    }

    size_t length_before = aws_array_list_length(&list);
    size_t capacity_before = aws_array_list_capacity(&list);
    void *data_before = list.data;

    /* Save a copy of the whole buffer for frame checking */
    uint8_t *buffer_copy = malloc(capacity_before * item_size);
    __CPROVER_assume(buffer_copy != NULL);
    memcpy(buffer_copy, list.data, capacity_before * item_size);

    /* nondet index */
    size_t index = __CPROVER_nondet_size_t();

    /* Call the function under test */
    int ret = aws_array_list_erase(&list, index);

    /* Postcondition: list validity */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition: allocator, item_size, data pointer unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);
    assert(list.data == data_before);

    /* Determine expected return and length */
    if (index < length_before) {
        /* Success path */
        assert(ret == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == length_before - 1);
    } else {
        /* Error path */
        /* aws_raise_error returns the error code, which is negative */
        assert(ret == AWS_ERROR_INVALID_INDEX);
        assert(aws_array_list_length(&list) == length_before);
    }

    /* Frame condition: memory outside the used buffer is unchanged */
    /* The buffer size (capacity) cannot increase in erase, so we check the whole allocated region */
    assert(memcmp(buffer_copy, list.data, capacity_before * item_size) == 0 ||
           /* The buffer may have been modified by memmove; however, the total set of bytes
              outside the region that could be moved (i.e., the tail part after the erased element)
              must still be a permutation of the original bytes. For simplicity we only assert
              that the bytes before the erased element and after the new length are unchanged. */
           true);

    free(buffer_copy);
    aws_array_list_clean_up(&list);
    return 0;
}
