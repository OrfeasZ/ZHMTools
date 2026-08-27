#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."

# Grab the MSVC CRT + Windows SDK for x64.
xwin --accept-license --arch x86_64 --cache-dir .xwin/cache \
    splat --include-debug-libs --output .xwin/splat
