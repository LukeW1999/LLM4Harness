#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

extern void *nondet_voidp(void);
extern size_t nondet_size_t(void);

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    list.current_size = nondet_size_t();
    list.length = nondet_size_t();

    __CPROVER_assume(list.length * list.item_size <= list.current_size);

    if (list.current_size > 0) {
        list.data = nondet_voidp();
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    size_t old_length = list.length;
    struct aws_allocator *old_alloc = list.alloc;

    size_t result = aws_array_list_length(&list);

    assert(result == old_length);
    assert(list.data == old_data);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.length == old_length);
    assert(list.alloc == old_alloc);
    assert(aws_array_list_is_valid(&list));
}
