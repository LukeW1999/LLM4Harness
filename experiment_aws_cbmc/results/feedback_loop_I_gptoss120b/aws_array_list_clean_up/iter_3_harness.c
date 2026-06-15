#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t init_capacity;
    __CPROVER_assume(init_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialize the list with bounded parameters */
    __CPROVER_assume(aws_array_list_init(&list, alloc, init_capacity, item_size) == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* Post‑conditions */
    assert(list.data == NULL);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
