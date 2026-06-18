#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    list.item_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(list.item_size > 0);

    list.current_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(list.current_size <= SIZE_MAX / list.item_size);

    if (list.current_size > 0) {
        list.data = malloc(list.current_size * list.item_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    list.length = (size_t)nondet_uint64_t();
    __CPROVER_assume(list.length <= list.current_size);

    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;

    aws_array_list_swap(&list, a, b);

    assert(list.alloc == allocator);
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.length == old_length);
    assert(list.data == old_data);

    free(list.data);
}
