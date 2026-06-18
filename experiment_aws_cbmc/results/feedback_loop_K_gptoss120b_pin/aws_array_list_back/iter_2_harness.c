#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* nondeterministic helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);
void *nondet_ptr(void);

void aws_array_list_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize list fields nondeterministically respecting the validity predicate */
    list.alloc = allocator;

    /* item_size > 0 */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    /* Choose a nondeterministic multiplier for capacity (could be zero) */
    size_t capacity_multiplier = nondet_size_t();
    __CPROVER_assume(capacity_multiplier <= 10); /* bound to keep model small */

    list.current_size = list.item_size * capacity_multiplier;

    if (list.current_size == 0) {
        list.length = 0;
        list.data = NULL;
    } else {
        size_t max_len = list.current_size / list.item_size;
        list.length = nondet_size_t();
        __CPROVER_assume(list.length <= max_len);
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    }

    /* Allocate writable memory for val */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* Preserve original state for frame condition checks */
    struct aws_allocator *alloc_before = list.alloc;
    size_t item_size_before = list.item_size;
    size_t current_size_before = list.current_size;
    size_t length_before = list.length;
    void *data_before = list.data;

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
