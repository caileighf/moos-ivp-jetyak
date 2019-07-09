# after all launch scripts have been run AND the pixhawk has a GPS fix
# - you can see if the pixhawk has a GPS fix in mission planner

uPokeDB ROV_MODE=GUIDED targ_jetyak.moos 
sleep(1)
uPokeDB ROV_STATE=DISARM targ_jetyak.moos
sleep(1)

# check in mission planner that the mode is "guided" and that the vehicle is "disarmed"
# otherwise you'll need to change the mode and state in mission planner
# now hit deploy in pMarineViewer