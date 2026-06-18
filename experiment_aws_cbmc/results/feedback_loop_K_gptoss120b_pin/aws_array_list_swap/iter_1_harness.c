/*  
 * Preconditions:  
 *   - list is a valid, initialized aws_array_list (aws_array_list_is_valid(list) == true).  
 *   - list->length > 0.  
 *   - a < list->length && b < list->length.  
 *   - list->item_size > 0.  
 *   - The memory region pointed to by list->data is allocated and at least list->current_size bytes.  
 *   - The contents of list->data before the call are arbitrary.  
 *   - The allocator used for the list is the default allocator (aws_default_allocator()).  
 *   
 * Postconditions (validity):  
 *   - The list remains valid (aws_array_list_is_valid(list) == true).  
 *   - No allocation failures occur inside aws_array_list_swap (the function does not allocate).  
 *   
 * Postconditions (length & capacity):  
 *   - list->length is unchanged.  
 *   - list->current_size (and thus capacity) is unchanged.  
 *   
 * Postconditions (frame):  
 *   - All bytes of list->data except those belonging to the elements at indices a and b are unchanged.  
 *   - The element at index a after the call equals the original element at index b, and vice‑versa.  
 *   - If a == b, the entire data buffer is unchanged.  
 */  

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size, must be > 0 */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 256); /* reasonable bound */

    /* nondet initial capacity (number of items) */
    size_t init_capacity;
    __CPROVER_assume(init_capacity > 0);
    __CPROVER_assume(init_capacity <= 64); /* reasonable bound */

    struct aws_array_list list;
    int init_res = aws_array_list_init_dynamic(&list, alloc, init_capacity, item_size);
    __CPROVER_assume(init_res == 0);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet length between 0 and init_capacity */
    size_t len;
    __CPROVER_assume(len <= init_capacity);
    /* fill the list with len elements */
    for (size_t i = 0; i < len; ++i) {
        uint8_t *buf = malloc(item_size);
        __CPROVER_assume(buf != NULL);
        /* fill with nondet data */
        for (size_t j = 0; j < item_size; ++j) {
            buf[j] = (uint8_t) __CPROVER_nondet_uint();
        }
        int push_res = aws_array_list_push_back(&list, buf);
        __CPROVER_assume(push_res == 0);
        free(buf);
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length == len);
    __CPROVER_assume(list.current_size == len * item_size);

    /* if length is zero, swap cannot be called – abort harness */
    __CPROVER_assume(list.length > 0);

    /* nondet indices a and b within bounds */
    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* capture pre‑state of the data buffer */
    uint8_t *pre_data = malloc(list.current_size);
    __CPROVER_assume(pre_data != NULL);
    memcpy(pre_data, list.data, list.current_size);

    /* call the function under test */
    aws_array_list_swap(&list, a, b);

    /* ---- postconditions ---- */

    /* list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* length and capacity must be unchanged */
    assert(list.length == len);
    assert(list.current_size == len * item_size);

    /* verify that only the two elements have been swapped */
    uint8_t *data_bytes = (uint8_t *)list.data;
    for (size_t i = 0; i < list.length; ++i) {
        size_t offset = i * item_size;
        if (i == a) {
            /* element at a should now equal original element at b */
            assert(memcmp(data_bytes + offset,
                          pre_data + b * item_size,
                          item_size) == 0);
        } else if (i == b) {
            /* element at b should now equal original element at a */
            assert(memcmp(data_bytes + offset,
                          pre_data + a * item_size,
                          item_size) == 0);
        } else {
            /* all other elements unchanged */
            assert(memcmp(data_bytes + offset,
                          pre_data + offset,
                          item_size) == 0);
        }
    }

    /* clean up */
    free(pre_data);
    aws_array_list_clean_up(&list);
    return 0;
}
