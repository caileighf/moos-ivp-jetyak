#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------

VNAME="jetyak"           # The first vehicle Community
START_POS="100,-30"         
LOITER_POS="x=0,y=-75"
SHORE_LISTEN="9300"
SHORE_IP="172.20.10.10"

TIME_WARP=1
JUST_MAKE="no"
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI:0:11}" = "--shore-ip=" ] ; then
        SHORE_IP="${ARGI#--shore-ip=*}"
    elif [ "${ARGI:0:13}" = "--shore-port=" ] ; then
        SHORE_LISTEN=${ARGI#--shore-port=*}
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME          SHARE_LISTEN="9301"              \
    VPORT="9001"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS  SHORE_IP=$SHORE_IP 

nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME     \
    START_POS=$START_POS LOITER_POS=$LOITER_POS    

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &
printf "Done \n"

uMAC targ_jetyak.moos

printf "Killing all processes ... \n"
kill %1 %2 %3
printf "Done killing processes.   \n"
