## Write up for Mavlink/PX4 and their viability for our kinds of projects

### Terminology
Term / Acronym              | Definition
----------------------------|-------------------------
Pixhawk                     | Pixhawk Hex Cube Black (Front seat)
Flight Controller Unit (FCU)| Hardware running autopilot software (Pixhawk in our case)
Ground Control Station (GCS)| Software that provided a GUI client to command/interact with the Pixhawk/FCU (I'm using QGroundControl)
Remote Controller (RC)      | RC to drive the Pixhawk which drives the vehicle
Companion Computer          | "Back seat" Computer running MOOS and commanding the Pixhawk in *offboard mode*. In this case we are using a RaspberryPi (RPi)
Airframe (Rover) | The airframe defines the kind of vehicle/propulsion system

*Pixhawk and FCU are being used interchangeably in this document*

### Minimum Viable System
From what I've found through my research it looks like the basic system for a vehicle using the Pixhawk ecosystem includes:

- Vehicle with a Pixhawk as the front seat
- Companion computer running software to control the Pixhawk when in *offboard mode*
- GPS Fix (a MUST for the Rover airframe type despite configuration params that say otherwise -- more on this later)
- Active RF link to a remote controller
- Active RF link to a GCS
- WiFi LAN if using topside/vehicle community style setup with the companion machine and topside machine communicating via WiFi 

### Vehicle
Communication type | Endpoints | Use
------------------ | --------- | ----
RF Telemetry | Pixhawk, GCS | Connects the FCU to the GCS -- For adding way points, debugging, calibration, etc...
RF Telemetry | Pixhawk, RC  | Connects the RC and the FCU 
WiFi LAN     | RPi vehicle community, Topside MOOS community | For a typical MOOS-IvP topside/shoreside and vehicle setup
Serial       | RPi, Pixhawk | Hard line serial connection between the Pixhawk and RPi for sending Mavlink commands *when in offboard mode*

### Topside Machine
Communication type | Endpoints | Use
------------------ | --------- | ----
RF Telemetry | GCS, Pixhawk | 
WiFi LAN     | Topside MOOS community, RPi vehicle community | 


### Problems I am having with Mavlink/PX4
As far as I can tell from my research, the open source community using these tools do not have a clear path to *offboard mode* where you can command the Pixhawk using a companion computer. This problem is manifesting in a few different ways:
1. There are multiple sources for documentation that have conflicting information. PX4 and Ardupilot seem to share a lot of the same code base and a lot of times you'll be directed to the [other] flight stacks documentation because it works the "same"
2. The message set available to you depends on the autopilot stack you're using (in our case PX4) and your Airframe type (in our case Rover).
3. The Rover airframe has minimal support for *offboard mode* (thankfully the messages available cover our current needs but it's something to keep in mind)
4. Aerial drones have much more support for *offboard mode* but using the copter style propulsion system on a JetYak or RC car would be complicated (if it's even possible)
5. The MAVSDK documentation is comprehensive but overly idealistic and inaccurate. They have documented features that are not actually supported in the implementation connected to that documentation (e.g., There is an explicit option to allow arming of the vehicle even if there is no GPS fix: however, I wasn't able to arm the vehicle. I found a discord thread where I found out the Rover stack does not support that and you MUST have a 3D GPS fix to arm -- it ignores that parameter all together...)
6. Getting visibility into the problems I'm having is difficult. The GCS is the way most people interact with the Pixhawk and most error messages are not informative (e.g., The GPS fix pre-req I previously mentioned was erroring with "Critical: REJECT OFFBOARD MODE"). The autopilot stack (in our case PX4) is running in a NuttX real-time OS. Mavlink has utilities you can access in the limited shell. However, there is no way to see what Mavlink messages are getting passed from the companion computer to the Pixhawk. Which brings me to the next issue...
7. To put the Pixhawk in *offboard mode* you have to satisfy a nebulous set of states. Depending on your autopilot stack, version of Mavlink, GCS and, airframe type, you can try to gather those pre-reqs. So far these are the pre-reqs for *offboard mode* with PX4 Version 3.0 Rover (current stable version for Rover):
        1. You must have a 3D GPS fix
        2. You must have an active connection to an RC controller (despite the documentation saying otherwise -- maybe after it's setup?) it was blocking me from arming the Pixhawk (another pre-req for offboard mode)
        3. You must have an active connection to a GCS like QGroundControl. 
        4. The Pixhawk must be armed and in either "Stabilized Mode" or "Manual Mode"
        5. The companion machine must be:
            1. Connected to the Pixhawk telemetry/serial port configured for the companion machine
            2. Already sending set_point messages to that serial port (using MAVSDK)
            3. Sending those set_point messages at >2Hz (But I have read conflicting reports -- some say >10Hz and some say you must send 100 set_point messages before going into offboard mode)


### Speculation
Drone autopilot software has A LOT of of functionality "out of the box". Clearly people have been able to use Mavlink effectively 