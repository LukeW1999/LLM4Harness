/*  
 * Contract for aws_array_list_length  
 * Preconditions:  
 *   - The caller provides a pointer `list` that is either zero‑initialized (`AWS_IS_ZEROED(*list)`)  
 *     or satisfies `aws_array_list_is_valid(list)`.  
 *   - No additional assumptions about the contents of the list are required.  
 *   
 * Postconditions (validity):  
 *   - The function returns the current `list->length`.  
 *   - The returned value is a `size_t` equal to the length stored in the list.  
 *   - After the call, the list remains either zero‑initialized or valid (`aws_array_list_is_valid(list)`).  
 *   
 * Postconditions (length invariants):  
 *   - The length reported by the function equals the `length` field of the list before the call.  
 *   - The `length` field of the list is unchanged by the function.  
 *   
 * Postconditions (frame):  
 *   - No memory locations other than the return value are modified.  
 *   - All fields of the `aws_array_list` structure (`data`, `current_size`, `item_size`, `length`) remain unchanged.  
 */

#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    /* nondet initialization of the list structure */
    __CPROVER_assume(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* Preserve the original state of the list */
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    size_t old_length = list.length;

    size_t result = aws_array_list_length(&list);

    /* Postcondition: result equals the stored length */
    assert(result == old_length);

    /* Postcondition: list fields are unchanged */
    assert(list.data == old_data);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.length == old_length);

    /* Postcondition: list remains zeroed or valid */
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* Additional consistency: if the list is zeroed, length must be zero */
    if (AWS_IS_ZEROED(list)) {
        assert(result == 0);
    }

    return 0;
}
