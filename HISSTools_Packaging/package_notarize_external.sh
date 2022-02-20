#!/bin/bash

ZIP_PATH="$1/$2.zip"

# Sign

codesign --force --timestamp --sign HIRT "$1/$2"  || exit 1

# Zip

/usr/bin/ditto -c -k --keepParent "$1/$2" "$ZIP_PATH" || exit 1

# Notarise

if ! xcrun notarytool submit "$ZIP_PATH" --keychain-profile "AJH-NOTARISE" --wait
then
  rm "$ZIP_PATH"
  exit 1
fi

rm "$ZIP_PATH"

# Staple

xcrun stapler staple "$1/$2" || exit 1

