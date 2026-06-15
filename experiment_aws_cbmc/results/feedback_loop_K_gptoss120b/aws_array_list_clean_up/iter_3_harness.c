/*  
Contract for aws_array_list_clean_up  

Preconditions:  
- __CPROVER_assume(list != NULL);  
- __CPROVER_assume(list->alloc == NULL || list->alloc == aws_default_allocator());  
- If list->alloc != NULL && list->data != NULL, then list->data points to a memory region allocated by list->alloc.  
- The fields of *list (current_size, length, item_size) may hold any size_t values.  

Postconditions (validity):  
- After the call, list->alloc == NULL.  
- list->data == NULL.  

Postconditions (length & capacity):  
- list->current_size == 0.  
- list->length == 0.  
- list->item_size == 0.  

Postconditions (frame):  
- No memory other than list->data (if any) is modified.  
- All other objects reachable from the program remain unchanged.  
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Helper to allocate nondeterministic data */
static void *nondet_alloc_data(struct aws_allocator *alloc, size_t size) {
    void *ptr = aws_mem_acquire(alloc, size);
    __CPROVER_assume(ptr != NULL);
    return ptr;
}

/* Harness */
void harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Non‑deterministically decide whether the list is in dynamic mode (has allocator) */
    bool has_alloc = __CPROVER_nondet_bool();
    if (has_alloc) {
        list.alloc = alloc;
    } else {
        list.alloc = NULL;
    }

    /* Non‑deterministically decide whether data is allocated */
    bool has_data = __CPROVER_nondet_bool();
    if (has_data && list.alloc != NULL) {
        size_t data_size = __CPROVER_nondet_size_t();
        __CPROVER_assume(data_size > 0);
        list.data = nondet_alloc_data(list.alloc, data_size);
    } else {
        list.data = NULL;
    }

    /* Other fields can be any value */
    list.current_size = __CPROVER_nondet_size_t();
    list.length = __CPROVER_nondet_size_t();
    list.item_size = __CPROVER_nondet_size_t();

    /* Frame object: an unrelated buffer that must remain unchanged */
    uint8_t other_buffer[16];
    for (size_t i = 0; i < sizeof(other_buffer); ++i) {
        other_buffer[i] = __CPROVER_nondet_uchar();
    }
    /* Save a copy for later comparison */
    uint8_t other_buffer_snapshot[16];
    for (size_t i = 0; i < sizeof(other_buffer); ++i) {
        other_buffer_snapshot[i] = other_buffer[i];
    }

    /* Assume the list satisfies the library's validity predicate */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Call the function under verification */
    aws_array_list_clean_up(&list);

    /* Post‑condition checks */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* Frame check: other_buffer must be unchanged */
    for (size_t i = 0; i < sizeof(other_buffer); ++i) {
        assert(other_buffer[i] == other_buffer_snapshot[i]);
    }
}
