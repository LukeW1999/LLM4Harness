#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list list;

    aws_linked_list_init(&list);

    assert(list.head == NULL);
    assert(list.tail == NULL);

    struct aws_linked_list expected = { .head = NULL, .tail = NULL };
    assert(memcmp(&list, &expected, sizeof(list)) == 0);
}
