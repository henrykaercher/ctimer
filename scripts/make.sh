CC="gcc"
CFLAGS="-std=c11 -Wall -Wextra -Wpedantic"
DEVFLAGS="-g"
PRODFLAGS="-O2 -DNDEBUG"
LIBS="$(pkg-config --libs sqlite3)"
SRC="src/*.c"
OUTPUT="bin"

mkdir -p "$OUTPUT"

case "$1" in
    make)
        $CC $CFLAGS $DEVFLAGS -o "$OUTPUT/ctimer" $SRC $LIBS
        ;;
    make-prod)
        $CC $CFLAGS $PRODFLAGS -o "$OUTPUT/ctimer" $SRC $LIBS
        ;;
    clear)
        rm -rf "$OUTPUT"
        ;;
	*)
		echo "Error: Wrong option"
		echo
		echo "Usage:"
		echo "$0 make		Compile in dev mode"
		echo "$0 make-prod	Compile in production mode"
		echo "$0 clear		Delete bin/ directory"
		exit 1
		;;
esac
