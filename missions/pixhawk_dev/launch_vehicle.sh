#!/bin/bash
TIME_WARP=1

SHORE_IP=10.42.0.1
SHORE_LISTEN="9200"

TRAIL_RANGE="3"
TRAIL_ANGLE="330"
HELP="no"
JUST_BUILD="no"
VTEAM=""
VNAME=""
VMODEL="kayak"

START_POS="0,0,180"
RETURN_POS="5,0"
LOITER_POS="x=100,y=-180"
GRAB_POS=""
UNTAG_POS=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        HELP="yes"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--jetyak" ] ; then
        M200_IP=10.42.0.47 # jetyak
        VNAME="jetyak"
	    VMODEL="kayak"
        VPORT="9005"
        SHARE_LISTEN="9305"
        echo "JETYAK vehicle selected."
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
        JUST_BUILD="yes"
        echo "Just building files; no vehicle launch."
    elif [ "${ARGI}" = "--sim" -o "${ARGI}" = "-s" ] ; then
        SIM="SIM"
        echo "Simulation mode ON."
    elif [ "${ARGI:0:10}" = "--start-x=" ] ; then
        START_POS_X="${ARGI#--start-x=*}"
    elif [ "${ARGI:0:10}" = "--start-y=" ] ; then
        START_POS_Y="${ARGI#--start-y=*}"
    elif [ "${ARGI:0:10}" = "--start-a=" ] ; then
        START_POS_A="${ARGI#--start-a=*}"
    else
        echo "Undefined argument:" $ARGI
        echo "Please use -h for help."
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Handle Ill-formed command-line arguments
#-------------------------------------------------------

if [ "${HELP}" = "yes" ]; then
    echo "$0 [SWITCHES]"
    echo "  --jetyak,         : Jetyak vehicle."
    echo "  --sim,        -s  : Simulation mode."
    echo "  --start-x=        : Start from x position (requires x y a)."
    echo "  --start-y=        : Start from y position (requires x y a)."
    echo "  --start-a=        : Start from angle (requires x y a)."
    echo "  --just_build, -j"
    echo "  --help, -h"
    exit 0;
fi

if [ -z $VNAME ]; then
    echo "No vehicle has been selected..."
    echo "Exiting."
    exit 2
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files.
#-------------------------------------------------------

if [[ -n $START_POS_X && (-n $START_POS_Y && -n $START_POS_A)]]; then
  START_POS="$START_POS_X,$START_POS_Y,$START_POS_A"
  echo "Starting from " $START_POS
elif [[ -z $START_POS_X && (-z $START_POS_Y && -z $START_POS_A) ]]; then
  echo "Starting from default position: " $START_POS
else [[ -z $START_POS_X || (-z $START_POS_Y || -z $START_POS_A) ]]
  echo "When specifying a starting coordinate, all 3 should be specified (x,y,a)."
  echo "See help (-h)."
  exit 1
fi

echo "Assembling MOOS file targ_${VNAME}.moos"


nsplug meta_vehicle.moos targ_${VNAME}.moos -f \
    VNAME=$VNAME                 \
    VPORT=$VPORT                 \
    WARP=$TIME_WARP              \
    SHARE_LISTEN=$SHARE_LISTEN   \
    SHORE_LISTEN=$SHORE_LISTEN   \
    FRONT_SEAT_IP=$FRONT_SEAT_IP \
    FRONT_SEAT_SHARE=$FRONT_SEAT_SHARE \
    SHORE_IP=$SHORE_IP           \
    M200_IP=$M200_IP             \
    HOSTIP_FORCE="localhost"     \
    LOITER_POS=$LOITER_POS       \
    VARIATION=$VARIATION         \
    VMODEL=$VMODEL                \
    VTYPE="kayak"                \
    VTEAM="blue"                 \
    START_POS=$START_POS         \
    $SIM                         

echo "Assembling BHV file targ_${VNAME}.bhv"
nsplug meta_vehicle.bhv targ_${VNAME}.bhv -f  \
        RETURN_POS=${RETURN_POS}    \
        TRAIL_RANGE=$TRAIL_RANGE    \
        TRAIL_ANGLE=$TRAIL_ANGLE    \
        VTEAM=$VTEAM                \
        VNAME=$VNAME                \
        GRAB_POS=$GRAB_POS          \
        UNTAG_POS=$UNTAG_POS


if [ ${JUST_BUILD} = "yes" ] ; then
    echo "Files assembled; vehicle not launched; exiting per request."
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

echo "Launching $VNAME MOOS Community "
pAntler targ_${VNAME}.moos >& /dev/null &
uMAC targ_${VNAME}.moos

echo "Killing all processes ..."
kill -- -$$
echo "Done killing processes."
