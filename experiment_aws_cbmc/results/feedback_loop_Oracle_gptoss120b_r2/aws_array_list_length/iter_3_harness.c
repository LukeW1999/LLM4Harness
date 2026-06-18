#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_array_list_is_allocated(&list, allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;
    if (list.data) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    size_t ret_len = aws_array_list_length(&list);

    assert(ret_len == list.length);
    assert(memcmp(&list, &old_list, sizeof(struct aws_array_list)) == 0);
    if (list.data) {
        assert(memcmp(list.data, old_data, list.current_size) == 0);
    }

    assert(aws_array_list_is_valid(&list));

    free(old_data);
}
