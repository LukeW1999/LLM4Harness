#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 5
#define MAX_ITEM_SIZE 64

void aws_array_list_clear_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    /* nondeterministically choose a valid item size */
    list.item_size = nondet_uint();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);

    /* nondeterministically choose an initial capacity */
    size_t init_capacity = nondet_uint();
    __CPROVER_assume(init_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* initialize the list */
    aws_array_list_init(&list, allocator, init_capacity, list.item_size);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* optionally make the list non‑empty */
    if (nondet_bool()) {
        uint8_t dummy[MAX_ITEM_SIZE];
        aws_array_list_push_back(&list, dummy);
    }

    struct aws_array_list old = list;

    int result = aws_array_list_clear(&list);
    (void)result; /* suppress unused variable warning */

    /* post‑conditions */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.data == NULL);
}
