#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    list.alloc = alloc;

    /* structural validity assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot old state */
    size_t old_length        = list.length;
    size_t old_current_size  = list.current_size;
    size_t old_item_size     = list.item_size;
    void  *old_data          = list.data;

    /* nondeterministic n, bounded to avoid overflow */
    size_t n;
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);

    /* call the function under verification */
    aws_array_list_pop_front_n(&list, n);

    /* postconditions */
    if (n >= old_length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_length - n);
    }

    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(list.alloc == alloc);

    return 0;
}
