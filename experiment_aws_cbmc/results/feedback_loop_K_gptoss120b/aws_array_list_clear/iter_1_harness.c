/*  
 * Contract for aws_array_list_clear  
 * Preconditions:  
 *   - list != NULL  
 *   - Either the list structure is completely zeroed (AWS_IS_ZEROED) or  
 *     aws_array_list_is_valid(list) holds.  
 *   - If list->data != NULL then list->current_size bytes are writable.  
 *   - list->item_size > 0 when list is valid.  
 *   - list->length * list->item_size <= list->current_size.  
 *   - If list->alloc != NULL then it points to a valid allocator.  
 *   
 * Postconditions (validity):  
 *   - After the call, either the list is still zeroed or aws_array_list_is_valid(list) holds.  
 *   
 * Postconditions (length):  
 *   - list->length == 0.  
 *   
 * Postconditions (frame):  
 *   - list->current_size, list->item_size, list->alloc, and the pointer value of list->data are unchanged.  
 *   - No memory outside list->data (if any) is modified.  
 */

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void harness(void) {
    struct aws_array_list list;
    /* nondet initialization of the structure */
    __CPROVER_assume(&list != NULL);
    /* Zero the structure first */
    memset(&list, 0, sizeof(list));

    /* nondet choice: either keep it zeroed or make it a valid list */
    bool make_valid = __CPROVER_nondet_bool();

    size_t old_current_size = 0;
    size_t old_item_size = 0;
    size_t old_length = 0;
    struct aws_allocator *old_alloc = NULL;
    void *old_data = NULL;

    if (make_valid) {
        /* item size must be > 0 */
        list.item_size = __CPROVER_nondet_uint();
        __CPROVER_assume(list.item_size > 0);

        /* length can be any value */
        list.length = __CPROVER_nondet_uint();

        /* current_size must be enough to hold length items */
        __CPROVER_assume(!aws_mul_size_checked(list.length, list.item_size, &old_current_size));
        list.current_size = old_current_size;

        /* allocate data if current_size > 0 */
        if (list.current_size > 0) {
            list.data = malloc(list.current_size);
            __CPROVER_assume(list.data != NULL);
            /* make the allocated memory writable */
            __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
        } else {
            list.data = NULL;
        }

        /* allocator may be NULL (static list) or a valid allocator */
        if (__CPROVER_nondet_bool()) {
            list.alloc = aws_default_allocator();
        } else {
            list.alloc = NULL;
        }

        /* ensure the list satisfies its own validity predicate */
        __CPROVER_assume(aws_array_list_is_valid(&list));

        /* remember old values for frame conditions */
        old_current_size = list.current_size;
        old_item_size = list.item_size;
        old_length = list.length;
        old_alloc = list.alloc;
        old_data = list.data;
    } else {
        /* keep the list zeroed – all fields already zero */
        old_current_size = 0;
        old_item_size = 0;
        old_length = 0;
        old_alloc = NULL;
        old_data = NULL;
    }

    /* Call the function under verification */
    aws_array_list_clear(&list);

    /* Postcondition: length must be zero */
    assert(list.length == 0);

    /* Postcondition: structural fields unchanged (frame) */
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.data == old_data);

    /* Postcondition: validity or zeroed */
    bool is_zeroed = (list.length == 0) && (list.current_size == 0) &&
                     (list.item_size == 0) && (list.alloc == NULL) && (list.data == NULL);
    assert(is_zeroed || aws_array_list_is_valid(&list));
}
