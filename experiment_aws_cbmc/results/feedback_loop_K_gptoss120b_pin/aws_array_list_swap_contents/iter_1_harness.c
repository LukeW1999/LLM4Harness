/*  
Preconditions:  
- __CPROVER_assume(list_a != &list_a); // distinct objects (handled by separate variables)  
- __CPROVER_assume(list_b != &list_b); // distinct objects (handled by separate variables)  
- __CPROVER_assume(list_a != list_b);  
- __CPROVER_assume(list_a.alloc != NULL);  
- __CPROVER_assume(list_a.alloc == list_b.alloc);  
- __CPROVER_assume(list_a.item_size == list_b.item_size);  
- __CPROVER_assume(list_a.item_size > 0);  
- __CPROVER_assume(list_a.length <= list_a.current_size / list_a.item_size); // valid list_a  
- __CPROVER_assume(list_b.length <= list_b.current_size / list_b.item_size); // valid list_b  

Postconditions (validity):  
- After the call both list_a and list_b remain valid according to aws_array_list_is_valid.  

Postconditions (swap semantics):  
- All fields of list_a after the call are equal to the corresponding fields of list_b before the call, and vice‑versa.  
- The underlying memory buffers pointed to by list_a.data and list_b.data are unchanged; only the pointers are swapped.  

Postconditions (frame):  
- No memory outside the two list structures and their data buffers is modified.  
- The allocator object (list_a.alloc / list_b.alloc) is unchanged.  
*/

#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_swap_contents_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size, bounded to avoid overflow */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 64);

    /* nondet initial capacities for the two lists */
    size_t init_cap_a;
    size_t init_cap_b;
    __CPROVER_assume(init_cap_a <= 8);
    __CPROVER_assume(init_cap_b <= 8);

    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* initialize both lists in dynamic mode with the same allocator and item size */
    assert(aws_array_list_init_dynamic(&list_a, alloc, init_cap_a, item_size) == 0);
    assert(aws_array_list_init_dynamic(&list_b, alloc, init_cap_b, item_size) == 0);

    /* nondet lengths that respect the capacities */
    __CPROVER_assume(list_a.length <= init_cap_a);
    __CPROVER_assume(list_b.length <= init_cap_b);

    /* fill the allocated buffers with nondet data */
    for (size_t i = 0; i < list_a.current_size; ++i) {
        ((uint8_t *)list_a.data)[i] = (uint8_t)__CPROVER_nondet_uint();
    }
    for (size_t i = 0; i < list_b.current_size; ++i) {
        ((uint8_t *)list_b.data)[i] = (uint8_t)__CPROVER_nondet_uint();
    }

    /* keep copies of the original structures and their buffer contents */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    uint8_t *buf_a = NULL;
    uint8_t *buf_b = NULL;
    if (old_a.data != NULL && old_a.length > 0) {
        buf_a = malloc(old_a.length * old_a.item_size);
        assert(buf_a != NULL);
        memcpy(buf_a, old_a.data, old_a.length * old_a.item_size);
    }
    if (old_b.data != NULL && old_b.length > 0) {
        buf_b = malloc(old_b.length * old_b.item_size);
        assert(buf_b != NULL);
        memcpy(buf_b, old_b.data, old_b.length * old_b.item_size);
    }

    /* Preconditions for the function (already satisfied by construction) */
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(list_a != list_b);

    /* Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* Postcondition: both lists are still valid */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    /* Postcondition: fields have been swapped */
    assert(list_a.alloc == old_b.alloc);
    assert(list_b.alloc == old_a.alloc);
    assert(list_a.item_size == old_b.item_size);
    assert(list_b.item_size == old_a.item_size);
    assert(list_a.length == old_b.length);
    assert(list_b.length == old_a.length);
    assert(list_a.current_size == old_b.current_size);
    assert(list_b.current_size == old_a.current_size);
    assert(list_a.data == old_b.data);
    assert(list_b.data == old_a.data);

    /* Postcondition: underlying buffers unchanged */
    if (buf_a != NULL) {
        assert(memcmp(buf_b, list_a.data, old_b.length * old_b.item_size) == 0);
    }
    if (buf_b != NULL) {
        assert(memcmp(buf_a, list_b.data, old_a.length * old_a.item_size) == 0);
    }

    /* Frame condition: allocator object unchanged */
    assert(list_a.alloc == alloc);
    assert(list_b.alloc == alloc);

    /* Clean up */
    if (buf_a) free(buf_a);
    if (buf_b) free(buf_b);
    aws_array_list_clean_up(&list_a);
    aws_array_list_clean_up(&list_b);

    return 0;
}
