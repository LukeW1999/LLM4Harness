#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;
    size_t item_count;
    size_t item_size;

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= 100);
    __CPROVER_assume(item_size <= 100);

    size_t tmp_current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &tmp_current_size));

    void *raw_array = malloc(tmp_current_size);
    __CPROVER_assume(raw_array != NULL);

    unsigned char *raw_before = malloc(tmp_current_size);
    __CPROVER_assume(raw_before != NULL);
    memcpy(raw_before, raw_array, tmp_current_size);

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    assert(list.alloc == NULL);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);
    assert(list.current_size == tmp_current_size);
    assert(aws_array_list_is_valid(&list));

    assert(memcmp(raw_before, raw_array, tmp_current_size) == 0);

    free(raw_before);
    free(raw_array);
}
