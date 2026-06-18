/*  
Preconditions:  
- `list` points to a valid `struct aws_array_list` (aws_array_list_is_valid(list) == true).  
- `list->alloc` is a non‑NULL allocator (we use aws_default_allocator()).  
- `list->item_size` is non‑zero and bounded (e.g., 1 ≤ item_size ≤ 1024).  
- `list->length` (old_length) is any value that does not exceed the list's capacity.  

Postconditions (validity):  
- The function returns `AWS_OP_SUCCESS` iff the list was non‑empty before the call.  
- If the list was empty, the function returns an error code (`AWS_ERROR_LIST_EMPTY`).  

Postconditions (length & size):  
- If old_length > 0 then `aws_array_list_length(list) == old_length - 1`.  
- If old_length == 0 then `aws_array_list_length(list) == 0`.  
- `list->current_size` (allocated byte size) is unchanged.  
- `aws_array_list_capacity(list)` is unchanged.  

Postconditions (frame):  
- `list->alloc`, `list->item_size`, and the allocator pointer are unchanged.  
- No memory outside the list’s internal buffer is modified.  
*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size, bounded */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 1024);

    /* nondet initial allocation count, bounded */
    size_t initial_alloc = nondet_size_t();
    __CPROVER_assume(initial_alloc <= 10);

    /* initialize list in dynamic mode */
    int init_rc = aws_array_list_init_dynamic(&list, alloc, initial_alloc, item_size);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet initial length, must not exceed capacity */
    size_t old_length = nondet_size_t();
    __CPROVER_assume(old_length <= aws_array_list_capacity(&list));

    /* fill the list to the desired length */
    for (size_t i = 0; i < old_length; ++i) {
        void *val = malloc(item_size);
        __CPROVER_assume(val != NULL);
        /* optionally nondet fill the value */
        aws_array_list_push_back(&list, val);
        free(val);
    }

    __CPROVER_assume(aws_array_list_length(&list) == old_length);

    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_current_size = list.current_size;

    int ret = aws_array_list_pop_front(&list);

    if (old_length > 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(ret != AWS_OP_SUCCESS);
        assert(ret == aws_raise_error(AWS_ERROR_LIST_EMPTY));
        assert(aws_array_list_length(&list) == 0);
    }

    /* size‑related invariants */
    assert(list.current_size == old_current_size);
    assert(aws_array_list_capacity(&list) == old_capacity);

    /* unchanged fields */
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);

    /* clean up */
    aws_array_list_clean_up(&list);
    return 0;
}
