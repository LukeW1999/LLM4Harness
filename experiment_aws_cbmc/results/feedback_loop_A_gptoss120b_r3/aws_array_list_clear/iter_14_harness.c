#include <stddef.h>
#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/error.h>
#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

/* CBMC nondeterministic size_t */
size_t nondet_size_t(void);

void aws_array_list_clear_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    list.alloc = allocator;

    /* nondeterministically initialize fields */
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();
    list.data = NULL;

    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int result = aws_array_list_clear(&list);

    assert(result == AWS_OP_SUCCESS);
    assert(list.length == 0);
    assert(list.current_size == old.current_size);
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));
}
