/*  
 * Contract for aws_array_list_back  
 * Preconditions:  
 *   - list != NULL  
 *   - list->item_size > 0  
 *   - (list->current_size == 0) || (list->data != NULL)  
 *   - list->current_size == list->item_size * MAX_ITEM_ALLOCATION_or_0 (modeled as some non‑negative multiplier)  
 *   - list->length * list->item_size <= list->current_size  
 *   - val != NULL and points to writable memory of size list->item_size  
 * Postconditions (validity):  
 *   - aws_array_list_is_valid(list) holds after the call  
 * Postconditions (length & capacity):  
 *   - list->length, list->current_size, list->item_size, list->alloc, list->data remain unchanged  
 *   - If list->length > 0 then the function returns AWS_OP_SUCCESS (0) and copies the last element into *val  
 *   - If list->length == 0 then the function returns an error (not AWS_OP_SUCCESS)  
 * Postconditions (frame):  
 *   - No memory locations other than the bytes written to *val are modified  
 */

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* nondeterministic helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);
void *nondet_ptr(void);

void aws_array_list_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Allocate and initialize list fields nondeterministically respecting the validity predicate */
    list.alloc = alloc;

    /* item_size > 0 */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    /* Choose a nondeterministic multiplier for capacity (could be zero) */
    size_t capacity_multiplier = nondet_size_t();
    __CPROVER_assume(capacity_multiplier <= 10); /* bound to keep model small */

    list.current_size = list.item_size * capacity_multiplier;

    /* length must satisfy length * item_size <= current_size */
    if (list.current_size == 0) {
        list.length = 0;
        list.data = NULL;
    } else {
        size_t max_len = list.current_size / list.item_size;
        list.length = nondet_size_t();
        __CPROVER_assume(list.length <= max_len);
        /* Allocate backing storage */
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    }

    /* Allocate writable memory for val */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* Preserve original state for frame condition checks */
    struct aws_array_list list_before = list;
    void *data_before = list.data;
    size_t current_size_before = list.current_size;
    size_t length_before = list.length;
    size_t item_size_before = list.item_size;
    struct aws_allocator *alloc_before = list.alloc;

    int ret = aws_array_list_back(&list, val);

    /* Postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions */
    assert(list.alloc == alloc_before);
    assert(list.item_size == item_size_before);
    assert(list.current_size == current_size_before);
    assert(list.length == length_before);
    assert(list.data == data_before);

    /* Return value conditions */
    if (length_before > 0) {
        assert(ret == AWS_OP_SUCCESS);
    } else {
        assert(ret != AWS_OP_SUCCESS);
    }

    /* Clean up */
    if (list.data) {
        free(list.data);
    }
    free(val);

    return 0;
}

/* Definitions of nondeterministic helpers for CBMC */
size_t nondet_size_t(void) {
    size_t x;
    return x;
}
bool nondet_bool(void) {
    bool b;
    return b;
}
void *nondet_ptr(void) {
    void *p;
    return p;
}
