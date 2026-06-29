#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 4
#define MAX_ITEM_SIZE 4

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;
    
    /* Allocate and initialize the list with bounded parameters */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    
    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    
    /* Initialize the list dynamically */
    struct aws_allocator *allocator = aws_default_allocator();
    aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    
    /* Populate the list with some elements */
    size_t num_elements;
    __CPROVER_assume(num_elements <= initial_item_allocation);
    
    for (size_t i = 0; i < num_elements; i++) {
        uint8_t val[MAX_ITEM_SIZE];
        aws_array_list_push_back(&list, val);
    }
    
    /* Apply the ground-truth preconditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    /* Save pre-call state */
    size_t old_length = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;
    
    /* Non-deterministic n */
    size_t n;
    
    /* Call the function under test */
    aws_array_list_pop_front_n(&list, n);
    
    /* Postcondition 1: The list must still be valid after the operation */
    assert(aws_array_list_is_valid(&list));
    
    /* Postcondition 2: item_size must not change */
    assert(list.item_size == old_item_size);
    
    /* Postcondition 3: allocator must not change */
    assert(list.alloc == old_alloc);
    
    /* Postcondition 4: data pointer must not change (memmove in place) */
    assert(list.data == old_data);
    
    /* Postcondition 5: capacity must not decrease (no reallocation should shrink) */
    assert(aws_array_list_capacity(&list) == old_capacity);
    
    /* Postcondition 6: Length invariants based on n vs old_length */
    if (n >= old_length) {
        /* If n >= old_length, list should be cleared (length == 0) */
        assert(aws_array_list_length(&list) == 0);
    } else if (n == 0) {
        /* If n == 0, nothing changes */
        assert(aws_array_list_length(&list) == old_length);
    } else {
        /* Otherwise, length decreases by n */
        assert(aws_array_list_length(&list) == old_length - n);
    }
    
    /* Postcondition 7: Length must not exceed old length */
    assert(aws_array_list_length(&list) <= old_length);
    
    /* Postcondition 8: Length must not exceed capacity */
    assert(aws_array_list_length(&list) <= aws_array_list_capacity(&list));
    
    /* Postcondition 9: current_size must not change */
    assert(list.current_size == old_capacity * old_item_size || 
           (old_capacity == 0 && list.current_size == 0));
    
    /* Cleanup */
    aws_array_list_clean_up(&list);
}
