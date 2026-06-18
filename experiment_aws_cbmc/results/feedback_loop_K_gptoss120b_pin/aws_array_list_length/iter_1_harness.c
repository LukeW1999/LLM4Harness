/*  
 * Contract for aws_array_list_length  
 * Preconditions:  
 *   - list is either zero-initialized or satisfies aws_array_list_is_valid(list).  
 *   - list->item_size > 0.  
 *   - list->length <= list->current_size / list->item_size (i.e., length does not exceed capacity).  
 *   - If list->current_size == 0 then list->data == NULL; otherwise list->data points to a writable memory region of size list->current_size.  
 *   - list->alloc may be NULL (static list) or a valid allocator; it is not dereferenced by this function.  
 * Postconditions (validity):  
 *   - The returned value equals list->length.  
 *   - The list remains valid after the call (aws_array_list_is_valid(list) holds).  
 * Postconditions (frame):  
 *   - No fields of the list structure are modified (length, current_size, item_size, data, alloc).  
 *   - No memory outside the list structure is modified.  
 */

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    /* nondet initialization */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= SIZE_MAX / item_size); /* avoid overflow */

    size_t current_size = capacity * item_size;

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);

    void *data = NULL;
    if (current_size > 0) {
        data = malloc(current_size);
        __CPROVER_assume(data != NULL);
    }

    list.item_size = item_size;
    list.current_size = current_size;
    list.length = length;
    list.data = data;
    list.alloc = NULL; /* static list */

    /* Ensure the list satisfies the validity predicate before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the list for frame checking */
    struct aws_array_list old_list = list;

    size_t result = aws_array_list_length(&list);

    /* Postcondition: return value equals length */
    assert(result == old_list.length);

    /* Postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Frame condition: list fields unchanged */
    assert(list.item_size == old_list.item_size);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.data == old_list.data);
    assert(list.alloc == old_list.alloc);

    return 0;
}
