#!/usr/bin/env bash
# Fetch the CBMC the paper pins (6.4.0, the version aws-c-common's CI proofs run)
# and print the CBMC640 export the *_640.py re-runs expect.
#
# The .deb is unpacked, not installed, so it never displaces a system CBMC.
#
#   ./get_cbmc640.sh [target-dir]        # default: ~/tools/cbmc-6.4.0
set -euo pipefail

DEST="${1:-$HOME/tools/cbmc-6.4.0}"
DEB="ubuntu-22.04-cbmc-6.4.0-Linux.deb"
URL="https://github.com/diffblue/cbmc/releases/download/cbmc-6.4.0/${DEB}"
SHA256="58f94eb3ecf87499389b9f0176656a4a89b303670dd06333fbaf5c8fa4bf38cd"

mkdir -p "$DEST"
cd "$DEST"

if [ ! -f "$DEB" ]; then
  echo "downloading $URL"
  curl -fL -o "$DEB" "$URL"
fi

echo "$SHA256  $DEB" | sha256sum -c -

rm -rf extracted
dpkg-deb -x "$DEB" extracted

CBMC="$DEST/extracted/usr/bin/cbmc"
"$CBMC" --version | grep -q '^6\.4\.0' || { echo "unexpected version: $($CBMC --version)"; exit 1; }

echo
echo "CBMC 6.4.0 ready. Export this before running any scripts/*_640.py:"
echo
echo "  export CBMC640=$CBMC"
