#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    /* Allocate and initialize a nondeterministic array list */
    ensure_array_list_is_allocated(&list, aws_default_allocator());

    /* Assume the list satisfies the library's validity predicate */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Capture a snapshot of the list structure and its backing buffer */
    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;
    if (list.data) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* Call the function under test */
    size_t ret_len = aws_array_list_length(&list);

    /* 1. Return value / error code correctness */
    assert(ret_len == list.length);

    /* 3. Memory not modified beyond the function's contract (frame conditions) */
    assert(memcmp(&list, &old_list, sizeof(struct aws_array_list)) == 0);
    if (list.data) {
        assert(memcmp(list.data, old_data, list.current_size) == 0);
    }

    /* The list must remain valid after the call */
    assert(aws_array_list_is_valid(&list));

    free(old_data);
}
