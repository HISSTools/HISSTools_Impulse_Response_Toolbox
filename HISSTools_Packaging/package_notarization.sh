#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

PACKAGE_PATH="$DIR/HISSTools Impulse Response Toolbox (HIRT)"
EXTERNAL_PATH="$PACKAGE_PATH/externals/HIRT_externals_OSX"
REVERB_EXT_PATH="$PACKAGE_PATH/patchers/HIRT_reverb/HIRT_reverb_support"
NOTARIZE="$DIR/package_notarize_external.sh"

# Notarize main externals

"$NOTARIZE" "$EXTERNAL_PATH" bufconvolve~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" bufresample~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" bufreverse~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" iralign~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" iraverage~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irextract~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irinvert~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irmeasure~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irnonlin~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irphase~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irplapprox~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irreference~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irstats~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irsweeps~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irtransaural~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irtrimnorm~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" iruser~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" irvalue~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" morphfilter~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" multiconvolve~.mxo || exit 1
"$NOTARIZE" "$EXTERNAL_PATH" spectrumdraw~.mxo || exit 1

# Reverb Extras

"$NOTARIZE" "$REVERB_EXT_PATH" ircropfade~.mxo || exit 1
"$NOTARIZE" "$REVERB_EXT_PATH" irdisplay~.mxo || exit 1
"$NOTARIZE" "$REVERB_EXT_PATH" irmix~.mxo || exit 1
