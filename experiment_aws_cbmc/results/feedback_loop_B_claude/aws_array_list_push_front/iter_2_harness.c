#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdlib.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 8
#endif

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    
    /* Use nondet values for item_size and length to ensure both branches reachable */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    list.item_size = item_size;
    
    size_t initial_length;
    __CPROVER_assume(initial_length <= MAX_INITIAL_ITEM_ALLOCATION);
    list.length = initial_length;
    
    size_t current_size;
    __CPROVER_assume(current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);
    /* current_size must be a multiple of item_size or zero */
    __CPROVER_assume(current_size == 0 || (current_size % item_size == 0));
    list.current_size = current_size;
    
    /* length * item_size <= current_size */
    __CPROVER_assume(list.length * list.item_size <= list.current_size);
    
    /* Allocate data buffer */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }
    
    /* Set allocator */
    list.alloc = aws_default_allocator();
    
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create a valid val pointer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions - these must always hold */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(aws_array_list_is_valid(&list));

    /* 6. Branch-specific postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length + 1);
        assert(list.current_size >= old_current_size);
        assert(list.data != NULL);
        assert(memcmp(list.data, val, list.item_size) == 0);
    } else {
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
    }
}
