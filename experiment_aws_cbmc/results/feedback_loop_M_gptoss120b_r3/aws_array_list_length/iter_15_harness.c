#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 5
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 32
#endif

void aws_array_list_length_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t init_cap = (size_t)nondet_uint64_t();
    __CPROVER_assume(init_cap <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_sz = (size_t)nondet_uint64_t();
    __CPROVER_assume(item_sz > 0);
    __CPROVER_assume(item_sz <= MAX_ITEM_SIZE);

    if (aws_array_list_init_dynamic(&list, allocator, init_cap, item_sz) != AWS_OP_SUCCESS) {
        return;
    }

    size_t old_len = list.length;

    size_t len = aws_array_list_length(&list);

    __CPROVER_assert(len == old_len, "aws_array_list_length returns correct length");
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid after length query");

    aws_array_list_clean_up(&list);
}
