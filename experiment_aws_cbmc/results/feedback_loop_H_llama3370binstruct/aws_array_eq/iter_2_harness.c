#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_eq_harness() {
    struct aws_array_list list1;
    struct aws_array_list list2;
    __CPROVER_assume(aws_array_list_is_bounded(&list1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list2, MAX_BUFFER_SIZE));
    ensure_array_list_is_initialized(&list1);
    ensure_array_list_is_initialized(&list2);
    __CPROVER_assume(aws_array_list_is_valid(&list1));
    __CPROVER_assume(aws_array_list_is_valid(&list2));

    bool result = aws_array_eq(&list1, &list2, aws_default_allocator(), NULL, NULL);

    assert(result == (list1.length == list2.length));
    if (result) {
        for (size_t i = 0; i < list1.length; i++) {
            assert(*(uint8_t *)aws_array_list_get_at(&list1, i) == *(uint8_t *)aws_array_list_get_at(&list2, i));
        }
    }

    assert(aws_array_list_is_valid(&list1));
    assert(aws_array_list_is_valid(&list2));
}
