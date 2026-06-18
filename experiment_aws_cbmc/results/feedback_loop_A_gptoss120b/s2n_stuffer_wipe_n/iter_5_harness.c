#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

/*--- s2n definitions needed for the harness ---------------------------------*/

#define S2N_SUCCESS 0
#define S2N_WIPE_PATTERN ((uint8_t)0x00)
#define S2N_MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX_BUFFER_SIZE 1024

/* macros used in the implementation – make them no‑ops for verification */
#define POSIX_PRECONDITION(x)   ((void)0)
#define POSIX_POSTCONDITION(x)  ((void)0)
#define POSIX_CHECKED_MEMSET(p, c, n)  memset((p), (c), (n))

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

/*--- external functions ------------------------------------------------------*/

extern int s2n_stuffer_wipe_n(struct s2n_stuffer *stuffer,
                              const uint32_t size);
extern bool s2n_stuffer_validate(const struct s2n_stuffer *stuffer);

/* nondet helpers */
uint32_t nondet_uint32_t(void);
_Bool nondet_bool(void);

/*--- harness -----------------------------------------------------------------*/

void s2n_stuffer_wipe_n_harness(void) {
    struct s2n_stuffer stuffer;
    static uint8_t buffer[MAX_BUFFER_SIZE];

    /* nondet allocation size for the underlying blob, bounded */
    uint32_t max_blob_size = nondet_uint32_t();
    __CPROVER_assume(max_blob_size <= MAX_BUFFER_SIZE);

    /* set the blob's data buffer */
    stuffer.blob.data = (max_blob_size == 0) ? NULL : buffer;
    __CPROVER_assume(stuffer.blob.data != NULL || max_blob_size == 0);

    /* nondet initialise blob fields */
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= max_blob_size);
    stuffer.blob.allocated = max_blob_size;
    stuffer.blob.growable = nondet_bool();

    /* nondet initialise stuffer cursors – must be within the blob */
    stuffer.write_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    stuffer.read_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* nondet flags */
    stuffer.alloced   = nondet_bool();
    stuffer.growable  = nondet_bool();
    stuffer.tainted   = nondet_bool();

    /* precondition: the stuffer must be valid */
    __CPROVER_assume(s2n_stuffer_validate(&stuffer));

    /* save old state for later comparison */
    struct s2n_stuffer old = stuffer;

    /* nondet argument to the function */
    uint32_t n = nondet_uint32_t();

    /* call the function under test */
    int result = s2n_stuffer_wipe_n(&stuffer, n);

    /* the implementation always returns S2N_SUCCESS */
    assert(result == S2N_SUCCESS);

    /* ----- post‑conditions on the fields that may change ----- */
    uint32_t expected_wipe = S2N_MIN(n, old.write_cursor);
    assert(stuffer.write_cursor == old.write_cursor - expected_wipe);

    uint32_t expected_read = S2N_MIN(old.read_cursor, stuffer.write_cursor);
    assert(stuffer.read_cursor == expected_read);

    /* the wiped region must contain the wipe pattern */
    for (uint32_t i = 0; i < expected_wipe; ++i) {
        assert(stuffer.blob.data[stuffer.write_cursor + i] == S2N_WIPE_PATTERN);
    }

    /* ----- fields that must remain unchanged ----- */
    assert(stuffer.blob.size      == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable  == old.blob.growable);
    assert(stuffer.blob.data      == old.blob.data);

    assert(stuffer.high_water_mark == old.high_water_mark);
    assert(stuffer.alloced         == old.alloced);
    assert(stuffer.growable        == old.growable);
    assert(stuffer.tainted         == old.tainted);

    /* ----- validity invariant must still hold ----- */
    assert(s2n_stuffer_validate(&stuffer));
}
