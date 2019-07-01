#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
# ktm
# sleep 1
# ktm

TIME_WARP=1
JUST_MAKE="no"
START_POS3="40,-40"  
START_TX=$2
START_TY=$4

# for ARGI; do
#     if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
# 	printf "%s [SWITCHES] [time_warp]   \n" $0
# 	printf "  --just_make, -j    \n" 
# 	printf "  --help, -h         \n" 
# 	exit 0;
#     elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
#         TIME_WARP=$ARGI
#     elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
# 	JUST_MAKE="yes"
#     elif [ "${ARGI}" = "STX,STY" ]; then
START_POS3="x="
START_POS3=$START_POS3"$START_TX"
START_POS3=$START_POS3",y="
START_POS3=$START_POS3"$START_TY"
#     else 
# 	printf "Bad Argument: %s \n" $ARGI
# 	exit 0
#     fi
# done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="jetyak1"           # The first vehicle Community
VNAME2="jetyak2"           # The second vehicle Community
VNAME3="target"            # The target community 
VNAME4="jetyak3"
START_POS1="100,-30"
START_POS2="100,-200"         

START_POS4="-20,-31"      
LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=0,y=-80"
LOITER_POS3="x=50,y=-100"
LOITER_POS4="x=0,y=-200"
SHORE_LISTEN="9300"


nsplug jetyak/jetyak1.moos targ_$VNAME1.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME1          SHARE_LISTEN="9301"              \
    VPORT="9001"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS1  

nsplug jetyak/jetyak2.moos targ_$VNAME2.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME2          SHARE_LISTEN="9304"              \
    VPORT="9004"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS2 

#nsplug jetyak3.moos targ_$VNAME4.moos -f WARP=$TIME_WARP \
#    VNAME=$VNAME4          SHARE_LISTEN="9305"              \
#    VPORT="9005"           SHORE_LISTEN=$SHORE_LISTEN       \
#    START_POS=$START_POS4     

nsplug target.moos targ_$VNAME3.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME3          SHARE_LISTEN="9303"              \
    VPORT="9003"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS3 

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
    SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                 \
    SPORT="9000"       

nsplug jetyak/jetyak1.bhv targ_$VNAME1.bhv -f VNAME=$VNAME1     \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug jetyak/jetyak2.bhv targ_$VNAME2.bhv -f VNAME=$VNAME2     \
    START_POS=$START_POS2 LOITER_POS=$LOITER_POS2       

#nsplug jetyak3.bhv targ_$VNAME4.bhv -f VNAME=$VNAME4     \
#    START_POS=$START_POS4 LOITER_POS=$LOITER_POS4  

nsplug target.bhv targ_$VNAME3.bhv -f VNAME=$VNAME3     \
    START_POS=$START_POS3 LOITER_POS=$LOITER_POS3       

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler jetyak/targ_$VNAME1.moos >& /dev/null &
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler jetyak/targ_$VNAME2.moos >& /dev/null &
printf "Launching $VNAME3 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler jetyak/targ_$VNAME3.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
kill %1 %2 %3
printf "Done killing processes.   \n"
