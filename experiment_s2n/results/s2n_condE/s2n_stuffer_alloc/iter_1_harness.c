#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_alloc_harness()
{
    struct s2n_stuffer *stuffer = malloc(sizeof(*stuffer));
    uint32_t size = nondet_uint32_t();

    // Save old values for immutability checks
    struct s2n_stuffer old_stuffer;
    if (stuffer) {
        old_stuffer = *stuffer;
    }

    int result = s2n_stuffer_alloc(stuffer, size);

    if (result == S2N_SUCCESS) {
        // Assert that the stuffer is valid after allocation
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

        // Assert changed fields
        assert(stuffer->alloced == 1);
        assert(stuffer->read_cursor == 0);
        assert(stuffer->write_cursor == 0);
        assert(stuffer->high_water_mark == 0);
        assert(stuffer->growable == 0);
        assert(stuffer->tainted == 0);

        // Assert unchanged fields
        assert(stuffer->blob.size == size);
        assert(stuffer->blob.growable == old_stuffer.blob.growable);
    } else {
        // On failure, stuffer should remain unchanged
        if (stuffer) {
            assert(*stuffer == old_stuffer);
        }
    }
}
