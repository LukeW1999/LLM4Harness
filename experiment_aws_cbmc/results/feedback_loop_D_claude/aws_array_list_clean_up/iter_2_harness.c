#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;

    /* Use the default allocator or NULL */
    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
    }

    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    list.current_size = nondet_size_t();
    list.length = nondet_size_t();

    /* Ensure length * item_size <= current_size to be valid */
    __CPROVER_assume(list.length <= list.current_size / list.item_size || list.current_size == 0);

    if (list.alloc != NULL && list.current_size > 0) {
        /* Allocate data using the allocator */
        list.data = aws_mem_acquire(list.alloc, list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else if (list.alloc == NULL && list.current_size > 0) {
        /* Static array case: data points to some buffer but alloc is NULL */
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
        list.current_size = 0;
        list.length = 0;
    }

    /* Ensure the list is valid before calling clean_up */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* After clean_up, all fields should be zeroed */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);
}
