#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list list;
    struct aws_linked_list old = list;

    aws_linked_list_init(&list);

    /* The list pointer must remain non‑NULL */
    assert(&list != NULL);

    /* After initialization both head and tail must be NULL */
    assert(list.head == NULL);
    assert(list.tail == NULL);

    /* The whole struct must now equal a freshly‑initialized list. */
    struct aws_linked_list expected = { .head = NULL, .tail = NULL };
    assert(memcmp(&list, &expected, sizeof(list)) == 0);
}
