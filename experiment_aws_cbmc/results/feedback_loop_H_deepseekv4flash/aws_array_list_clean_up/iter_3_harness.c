#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <stdlib.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;
    list.alloc = aws_default_allocator();
    list.current_size = 10;
    list.length = 5;
    list.item_size = 4;

    // Nondeterministically set data to NULL or allocated to cover both branches
    if (__CPROVER_bool) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    // Call function under test
    aws_array_list_clean_up(&list);

    // Assert postconditions: all fields are zeroed
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);
}
