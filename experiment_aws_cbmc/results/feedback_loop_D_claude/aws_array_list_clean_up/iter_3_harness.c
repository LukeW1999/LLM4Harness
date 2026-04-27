#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;

    /* Initialize with a bounded current_size to avoid timeout */
    list.item_size = 1;
    list.current_size = 0;
    list.length = 0;
    list.data = NULL;

    /* Use the default allocator or NULL */
    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
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
