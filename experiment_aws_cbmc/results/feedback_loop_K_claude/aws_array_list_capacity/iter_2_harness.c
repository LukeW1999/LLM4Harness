#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_capacity_harness(void) {
    /* Stack-allocate the list */
    struct aws_array_list list;

    /* Nondeterministically initialize fields */
    size_t item_size;
    size_t current_size;
    size_t length;
    void *data;

    __CPROVER_assume(item_size > 0);

    /* Ensure required_size = length * item_size does not overflow */
    __CPROVER_assume(!__CPROVER_overflow_mult(length, item_size));
    size_t required_size = length * item_size;

    /* current_size must be >= required_size */
    __CPROVER_assume(current_size >= required_size);

    /* data validity: current_size == 0 => data == NULL, current_size != 0 => data is valid */
    if (current_size == 0) {
        data = NULL;
    } else {
        data = malloc(current_size);
        __CPROVER_assume(data != NULL);
    }

    list.item_size    = item_size;
    list.current_size = current_size;
    list.length       = length;
    list.data         = data;
    list.alloc        = aws_default_allocator();

    /* Verify precondition: aws_array_list_is_valid holds */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save pre-call state for frame conditions */
    size_t old_current_size = list.current_size;
    size_t old_item_size    = list.item_size;
    size_t old_length       = list.length;
    void  *old_data         = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under verification */
    size_t capacity = aws_array_list_capacity(&list);

    /* Postcondition: return value is current_size / item_size */
    assert(capacity == old_current_size / old_item_size);

    /* Postcondition: aws_array_list_is_valid still holds */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition: capacity * item_size <= current_size */
    assert(capacity * list.item_size <= list.current_size);

    /* Postcondition: length <= capacity */
    assert(list.length <= capacity);

    /* Postcondition: frame conditions - nothing was modified */
    assert(list.current_size == old_current_size);
    assert(list.item_size    == old_item_size);
    assert(list.length       == old_length);
    assert(list.data         == old_data);
    assert(list.alloc        == old_alloc);
}
