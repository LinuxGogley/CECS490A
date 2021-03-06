#!/bin/bash

# Launch variables for ROS
export ROS_MASTER_URI=http://tegra-ubuntu:11311
export ROS_HOSTNAME=$HOSTNAME

while true
do
HEIGHT=11
WIDTH=40
CHOICE_HEIGHT=5
BACKTITLE="3D-ROMAP Launcher"
TITLE="3D-ROMAP Control Panel"
FOOENU="Select an option below:"

OPTIONS=(1 "Enter Jetson password"
         2 "Run roscore server"
         3 "Run 3D-ROMAP"
         4 "SSH to Jetson"
         5 "Exit")

CHOICE=$(dialog --clear \
                --backtitle "$BACKTITLE" \
                --title "$TITLE" \
                --menu "$MENU" \
                $HEIGHT $WIDTH $CHOICE_HEIGHT \
                "${OPTIONS[@]}" \
                2>&1 >/dev/tty)

clear
   case $CHOICE in
        1)
            echo "Enter login password to Jetson computer: "
            read -s JETSON
            ;;
        2)
            xterm -bg black -fg white -hold -e sshpass -p "$JETSON" ssh nvidia@tegra-ubuntu \
            'source /opt/ros/kinetic/setup.bash ; source ~/catkin_ws/devel/setup.bash ; roscore' &
            ;;
        3)
            xterm -bg black -fg white -hold -e sshpass -p "$JETSON" ssh nvidia@tegra-ubuntu \
            'source /opt/ros/kinetic/setup.bash ; source ~/catkin_ws/devel/setup.bash ; sh mapper.sh' &
            xterm -bg black -fg white -hold -e "sh 3D-ROMAP.sh" &
            rviz &
            ;;
        4)
            xterm -bg black -fg white -hold -e sshpass -p "$JETSON" ssh nvidia@tegra-ubuntu &
            ;;

        5)  export JETSON=$NULL ;
            break
            ;;
   esac
done
