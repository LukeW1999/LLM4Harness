#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_array_list_clear_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic choice: zeroed list or a valid static list */
    bool is_zeroed = nondet_bool();
    if (is_zeroed) {
        /* make the list zeroed */
        memset(&list, 0, sizeof(list));
    } else {
        /* create a valid static list */
        size_t item_size = nondet_size_t();
        __CPROVER_assume(item_size > 0);

        size_t item_count = nondet_size_t();
        __CPROVER_assume(item_count > 0);
        __CPROVER_assume(item_count <= 10); /* bound to keep allocation reasonable */

        /* prevent overflow in allocation size */
        __CPROVER_assume(item_size <= SIZE_MAX / item_count);

        uint8_t *raw_array = (uint8_t *)malloc(item_size * item_count);
        __CPROVER_assume(raw_array != NULL);

        aws_array_list_init_static(&list, raw_array, item_count, item_size);

        /* nondeterministically set the current length (must be <= item_count) */
        size_t length = nondet_size_t();
        __CPROVER_assume(length <= item_count);
        list.length = length;
    }

    /* enforce the precondition of aws_array_list_clear */
    __CPROVER_assume(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* PRE-CALL SNAPSHOT */
    size_t old_length = list.length;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    uint8_t *old_contents = NULL;
    if (list.data && old_current_size > 0) {
        old_contents = (uint8_t *)malloc(old_current_size);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, list.data, old_current_size);
    }

    /* CALL */
    aws_array_list_clear(&list);

    /* POSTCONDITIONS */
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));
    assert(list.length == 0);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);
    /* allocator may be NULL for static lists */
    assert(list.alloc == NULL || list.alloc == alloc);
    if (old_data && old_current_size > 0) {
        assert(memcmp(list.data, old_contents, old_current_size) == 0);
    }
}
