#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stddef.h>

/* CBMC nondeterministic helpers */
extern void *nondet_voidp(void);
extern size_t nondet_size_t(void);

void aws_array_list_length_harness(void) {
    struct aws_array_list list;

    /* nondeterministic initialization of fields */
    list.data = nondet_voidp();
    list.current_size = nondet_size_t();
    list.item_size = nondet_size_t();
    list.length = nondet_size_t();

    /* Preconditions: list is either zeroed or valid */
    __CPROVER_assume(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* Preserve original state */
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    size_t old_length = list.length;

    size_t result = aws_array_list_length(&list);

    /* Postconditions */
    assert(result == old_length);
    assert(list.data == old_data);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.length == old_length);
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    if (AWS_IS_ZEROED(list)) {
        assert(result == 0);
    }
}
