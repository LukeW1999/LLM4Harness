#!/usr/bin/env bash
# Every check the paper has to pass, in the order a failure is cheapest to fix.
set -u
cd "$(dirname "$0")/.."
fail=0
run() { echo; echo "### $1"; shift; "$@" || fail=1; }
run "numbers still true"        python3 scripts/paper_numbers_640.py
run "no number unwatched"       python3 scripts/audit_paper_coverage.py
run "figures match the registry" python3 scripts/audit_figure_data.py
run "prose against CLAUDE.md"   python3 scripts/audit_paper_prose.py
echo
[ $fail -eq 0 ] && echo "ALL CHECKS PASSED" || echo "SOME CHECKS FAILED"
exit $fail
