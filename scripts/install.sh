#!/bin/sh

set -e

PREFIX="$HOME/.local"
BIN="$PREFIX/bin"

echo "Building ctimer..."

mkdir -p bin

cc \
	-std=c11 \
	-Wall \
	-Wextra \
	-Wpedantic \
	src/*.c \
	-lsqlite3 \
	-o bin/ctimer

echo "Installing..."

mkdir -p "$BIN"

cp bin/ctimer "$BIN/ctimer"
chmod +x "$BIN/ctimer"

echo ""
echo "ctimer installed successfully."
echo ""

if ! echo "$PATH" | grep -q "$BIN"; then
	echo "Add this to your shell configuration or check if it's already added:"
	echo ""
	echo "export PATH=\"\$HOME/.local/bin:\$PATH\""
fi
