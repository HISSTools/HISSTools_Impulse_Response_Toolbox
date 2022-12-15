#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Notarize main externals

"$DIR/package_notarization.sh" || exit 1

# N.B. Windows externals should already be in place

# Zip the package

PACKAGE_PATH="$SRCROOT/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)"
ZIP_PATH="$SRCROOT/build/HISSTools Impulse Response Toolbox (HIRT).zip"

/usr/bin/ditto -c -k --norsrc --keepParent "$PACKAGE_PATH" "$ZIP_PATH" || exit 1
