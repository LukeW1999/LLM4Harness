#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* s2n struct definitions needed for the harness */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

/* Forward declarations for s2n functions */
int s2n_stuffer_free(struct s2n_stuffer *stuffer);
int s2n_stuffer_validate(const struct s2n_stuffer *stuffer);

/*
 * Analysis:
 * 1. On success: stuffer is zeroed out (*stuffer = {0})
 *    - blob.data = NULL, blob.size = 0, blob.allocated = 0, blob.growable = 0
 *    - read_cursor = 0, write_cursor = 0, high_water_mark = 0
 *    - alloced = 0, growable = 0, tainted = 0
 * 2. On failure: stuffer may be partially modified (precondition failed)
 * 3. The function frees blob memory if alloced is set
 * 4. Returns S2N_SUCCESS (0) on success
 */

void s2n_stuffer_free_harness(void) {
    /* Allocate a stuffer non-deterministically */
    struct s2n_stuffer *stuffer = malloc(sizeof(struct s2n_stuffer));
    if (stuffer == NULL) return;

    /* Initialize fields non-deterministically */
    stuffer->read_cursor = nondet_uint32_t();
    stuffer->write_cursor = nondet_uint32_t();
    stuffer->high_water_mark = nondet_uint32_t();
    stuffer->alloced = nondet_bool() ? 1 : 0;
    stuffer->growable = nondet_bool() ? 1 : 0;
    stuffer->tainted = nondet_bool() ? 1 : 0;

    /* Initialize blob */
    stuffer->blob.size = nondet_uint32_t();
    stuffer->blob.allocated = nondet_uint32_t();
    stuffer->blob.growable = nondet_bool() ? 1 : 0;

    /* Allocate blob data if needed */
    if (stuffer->blob.allocated > 0) {
        stuffer->blob.data = malloc(stuffer->blob.allocated);
        /* Assume data is allocated if allocated > 0 */
        __CPROVER_assume(stuffer->blob.data != NULL);
        /* size must be <= allocated */
        __CPROVER_assume(stuffer->blob.size <= stuffer->blob.allocated);
    } else {
        /* Non-deterministically set data pointer for non-allocated blobs */
        stuffer->blob.data = nondet_bool() ? NULL : malloc(stuffer->blob.size);
        if (stuffer->blob.size > 0) {
            __CPROVER_assume(stuffer->blob.data != NULL);
        }
    }

    /* Assume valid stuffer state for precondition */
    /* read_cursor <= write_cursor <= high_water_mark <= blob.size */
    __CPROVER_assume(stuffer->read_cursor <= stuffer->write_cursor);
    __CPROVER_assume(stuffer->write_cursor <= stuffer->high_water_mark);
    __CPROVER_assume(stuffer->high_water_mark <= stuffer->blob.size);

    /* If alloced, blob must be growable and have allocated memory */
    if (stuffer->alloced) {
        __CPROVER_assume(stuffer->blob.growable == 1);
        __CPROVER_assume(stuffer->blob.allocated >= stuffer->blob.size);
    }

    /* Call the function under test */
    int result = s2n_stuffer_free(stuffer);

    /* Postconditions */
    if (result == 0) { /* S2N_SUCCESS */
        /* On success, stuffer is zeroed out */
        assert(stuffer->blob.data == NULL);
        assert(stuffer->blob.size == 0);
        assert(stuffer->blob.allocated == 0);
        assert(stuffer->blob.growable == 0);
        assert(stuffer->read_cursor == 0);
        assert(stuffer->write_cursor == 0);
        assert(stuffer->high_water_mark == 0);
        assert(stuffer->alloced == 0);
        assert(stuffer->growable == 0);
        assert(stuffer->tainted == 0);
    } else {
        /* On failure (precondition failed), result should be -1 */
        assert(result == -1);
    }
}

void s2n_stuffer_free_harness(void) {
    s2n_stuffer_free_harness();
    return 0;
}
