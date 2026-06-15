/*  
 * Contract for aws_array_list_init_static  
 * Preconditions:  
 *   - list != NULL  
 *   - raw_array != NULL  
 *   - item_count > 0  
 *   - item_size > 0  
 *   - multiplication item_count * item_size does not overflow (aws_mul_size_checked succeeds)  
 * Postconditions (validity):  
 *   - list->alloc == NULL  
 *   - list->item_size == item_size  
 *   - list->length == 0  
 *   - list->data == raw_array  
 *   - list->current_size == item_count * item_size (no overflow)  
 *   - aws_array_list_is_valid(list) returns true  
 * Postconditions (frame):  
 *   - No memory outside of *list and the raw_array buffer is modified.  
 */

#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Preconditions */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    size_t current_size = 0;
    int no_overflow = !aws_mul_size_checked(item_count, item_size, &current_size);
    __CPROVER_assume(no_overflow);
    __CPROVER_assume(current_size > 0);

    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Postconditions */
    assert(list.alloc == NULL);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);
    assert(list.current_size == current_size);
    assert(aws_array_list_is_valid(&list));
}
