#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

size_t nondet_size_t(void);
void *nondet_void_ptr(void);

void aws_array_list_get_at_harness(void) {
    struct aws_array_list list;
    /* allocator */
    list.alloc = aws_default_allocator();

    /* nondet item size, must be > 0 */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    /* nondet current size */
    list.current_size = nondet_size_t();

    /* nondet length */
    list.length = nondet_size_t();

    /* ensure length * item_size does not overflow and fits in current_size */
    size_t required_bytes;
    __CPROVER_assume(!aws_mul_size_checked(list.length, list.item_size, &required_bytes));
    __CPROVER_assume(required_bytes <= list.current_size);

    /* allocate data buffer if needed */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* nondet index */
    size_t index = nondet_size_t();

    /* allocate output buffer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* preconditions */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(val != NULL);

    /* snapshot of input state */
    size_t old_length = list.length;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    uint8_t *old_contents = NULL;
    if (list.current_size > 0) {
        old_contents = malloc(list.current_size);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, list.data, list.current_size);
    }

    /* call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* ASSERT_POSTCONDITIONS_HERE */

    /* cleanup */
    free(old_contents);
    free(val);
    free(list.data);
}
