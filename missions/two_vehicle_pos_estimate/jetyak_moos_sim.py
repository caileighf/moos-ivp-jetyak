#!/usr/bin/python

import random
import os
import time
import math
# python script to auto-run Ryan Conway's 2-jetyak simulation

N=100 # number of simulations to run
T=1 # duration, in minutes, for each simulation
radius=1000 # radius of circle for sensing
n=100# number of points around circle to start from
pts=4 # number of points in target path
j1_pts=2
j2_pts=2
rundir=os.environ['HOME']+'/moos-ivp-conwayrl/missions/research/simulations/two_vehicle_pos_estimate'
mode='line' # mode options: line or curve

circle = list((math.cos(2*math.pi/n*x)*radius,math.sin(2*math.pi/n*x)*radius) for x in range(0,n+1))
# RYAN: make sure that 

for n in range(N):
    random.seed(n) # randomly seed so that you can replicate results with diff sim params
    # choose two points:
    t_pts = random.sample(circle,2)
    if mode=='line':
        center_pt_line = ((t_pts[0][0]+t_pts[1][0])/2,(t_pts[0][1]+t_pts[1][1])/2)
    # set up waypoint target string:
    tgt_up='"points = pts={' +str(center_pt_line[0])+','+str(center_pt_line[1])+':'+str(t_pts[1][0])+','+str(t_pts[1][1]) +'}"'
    launch_msg = 'start_tx= ' + str(t_pts[0][0])+' start_ty= ' + str(t_pts[0][1])
    print (tgt_up)
    print (launch_msg)
    # next, launch simulation:
    os.system('cd ' + rundir +';./launch.sh '+launch_msg +' & disown')# maybe add here to run with no vis
    # next, poke vars:
    time.sleep(2)
    # os.system("start cmd")
    os.system('cd ' + rundir +';uPokeDB targ_target.moos WPT_UPDATE=' + tgt_up)
    time.sleep(3)
    os.system('cd' + rundir + ';uPokeDB targ_shoreside.moos DEPLOY_ALL=true , MOOS_MANUAL_OVERRIDE_ALL=false , RETURN_ALL=false , STATION_KEEP_ALL=false')

    # next, wait T minutes:
    time.sleep(T*60)

    # then kill everything:
    os.system('cd ' + rundir +';ktm')
    
