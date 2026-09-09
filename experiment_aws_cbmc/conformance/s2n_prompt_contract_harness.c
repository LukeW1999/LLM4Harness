/* Does the s2n generation prompt describe an environment that actually builds?
 *
 * Every include, helper and macro below is one the prompt tells the model to use.
 * If this harness does not verify, the prompt is instructing the model to write a
 * harness that cannot compile, and every failure we would then attribute to the
 * model is ours. The expert harnesses do not cover this: they predate the prompt
 * and use a different subset of the API.
 *
 * Deliberately absent: any stub declaration. The prompt forbids them because the
 * proof already links them, and a redeclaration changes their signature.
 */
#include <s2n.h>
#include "stuffer/s2n_stuffer.h"
#include "utils/s2n_blob.h"
#include "utils/s2n_mem.h"
#include "utils/s2n_safety.h"
#include <cbmc_proof/cbmc_utils.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void s2n_stuffer_is_consumed_harness() {
    nondet_s2n_mem_init();

    struct s2n_stuffer stuffer;
    cbmc_populate_s2n_stuffer(&stuffer);
    __CPROVER_assume(s2n_stuffer_is_valid(&stuffer));

    struct s2n_stuffer old_stuffer = stuffer;

    bool result = s2n_stuffer_is_consumed(&stuffer);

    /* the documented status forms both parse */
    int status = result ? S2N_SUCCESS : S2N_FAILURE;
    assert(status == S2N_SUCCESS || status == S2N_FAILURE);

    /* every field the prompt names is readable */
    assert(stuffer.blob.size == old_stuffer.blob.size);
    assert(stuffer.read_cursor == old_stuffer.read_cursor);
    assert(stuffer.write_cursor == old_stuffer.write_cursor);
    assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
    assert(stuffer.alloced == old_stuffer.alloced);
    assert(stuffer.growable == old_stuffer.growable);
    assert(stuffer.tainted == old_stuffer.tainted);
    assert(s2n_stuffer_is_valid(&stuffer));

    /* the documented scalar and byte helpers link */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= stuffer.blob.size);
    if (stuffer.blob.data != NULL && n > 0) {
        assert_all_bytes_are(stuffer.blob.data, stuffer.blob.data[0], 0);
    }
    const char *cs = ensure_c_str_is_allocated(8);
    assert(cs == NULL || cs[0] == cs[0]);
}
