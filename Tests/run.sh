#!/usr/bin/env bash
set -u

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cc="${CC:-clang}"
build_dir="$(mktemp -d "${TMPDIR:-/tmp}/c_core_tests.XXXXXX")"
trap 'rm -rf "$build_dir"' EXIT

if [ "$#" -gt 0 ]; then
	sources=("$@")
else
	sources=("$script_dir"/test_*.c)
fi

failures=0
for source in "${sources[@]}"; do
	if [ ! -f "$source" ] && [ -f "$script_dir/$source" ]; then
		source="$script_dir/$source"
	fi

	name="$(basename "${source%.c}")"
	binary="$build_dir/$name"
	printf '==> %s\n' "$name"
	if ! "$cc" -std=c11 -Wall -Wextra -Wpedantic "$source" -lm -o "$binary"; then
		printf 'COMPILE FAILED: %s\n\n' "$name" >&2
		failures=$((failures + 1))
		continue
	fi
	if ! "$binary"; then
		printf 'TEST FAILED: %s\n\n' "$name" >&2
		failures=$((failures + 1))
		continue
	fi
	printf '\n'
done

if [ "$failures" -ne 0 ]; then
	printf '%d test file(s) failed\n' "$failures" >&2
	exit 1
fi

printf 'All test files passed\n'
