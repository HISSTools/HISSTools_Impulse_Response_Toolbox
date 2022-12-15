#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

PACKAGE_PATH="$DIR/HISSTools Impulse Response Toolbox (HIRT)"
EXTERNAL_PATH="$PACKAGE_PATH/externals/HIRT_externals_OSX"
REVERB_EXT_PATH="$PACKAGE_PATH/patchers/HIRT_reverb/HIRT_reverb_support"

RESULT=0
COMPLETED=0
pids=""

notarize()
{
    "$DIR/package_notarize_external.sh" "$2" "$1" &
    pids+="$! "
}

notarize_main()
{
    notarize "$1" "$EXTERNAL_PATH"
}

notarize_extra()
{
    notarize "$1" "$REVERB_EXT_PATH"
}

# Notarize main externals

notarize_main bufconvolve~.mxo
notarize_main bufresample~.mxo
notarize_main bufreverse~.mxo
notarize_main iralign~.mxo
notarize_main iraverage~.mxo
notarize_main irextract~.mxo
notarize_main irinvert~.mxo
notarize_main irmeasure~.mxo
notarize_main irnonlin~.mxo
notarize_main irphase~.mxo
notarize_main irplapprox~.mxo
notarize_main irreference~.mxo
notarize_main irstats~.mxo
notarize_main irsweeps~.mxo
notarize_main irtransaural~.mxo
notarize_main irtrimnorm~.mxo
notarize_main iruser~.mxo
notarize_main irvalue~.mxo
notarize_main morphfilter~.mxo
notarize_main multiconvolve~.mxo
notarize_main spectrumdraw~.mxo

# Reverb Extras

notarize_extra ircropfade~.mxo
notarize_extra irdisplay~.mxo
notarize_extra irmix~.mxo

# Wait to complete

for pid in $pids; do
    wait $pid
    if [ $? -eq 0 ]; then
        let COMPLETED+=1
    else
        RESULT=1
        echo "FAILED - Job $pid exited with a status of $?"
    fi
done

if [ $RESULT -eq 0 ]; then
    echo "SUCCESS - All Externals Notarized! $COMPLETED Completed"
else
    echo "FAILED - Some Externals Failed to Notarize! $COMPLETED Completed"
fi

exit $RESULT


