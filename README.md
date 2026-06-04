**OVERVIEW**

A simple first stage Rootic Arm with 6 Degrees Of Freedom (6DOF). The pakcage include a Gazebo simulation and an RViz vizualization of the Robot

**LAUNCH PROCESS**
Gazebo Simulation launch and Rviz visualization are all brought up with a single command. 
The comand is in the ```.sh``` file in the path ```src/robotic_arm/robotic_arm_bringup/scripts/robotic_arm_280_gazebo.sh```

Make the ```.sh``` file executbale first by runnig the following commmand on ```bash```
```
bash
chmod +x <path/to/the/root/workspace>/src/robotic_arm/robotic_arm_bringup/scripts/robotic_arm_280_gazebo.sh
```

**Running Gazebo World and Rviz**
After making sure the ```.sh``` file is executable, now its time to run the Gazebo simulation and Rviz visialization.
First make sure you are in your worksapce root directory and Source the Robot's workspace by runnnid the following command
```
bash
source <path/to/workspace/root>/install/local_setup.bash
```

Then, run the ```.sh``` script that brings-up both gazebo and rviz.
```
bash
bash <path/to/workspace/root>/src/robotic_arm/robotic_arm_bringup/scripts/robotic_arm_280_gazebo.sh
```
After bringing up gazebo and run rviz, you can now run a test script that will move the Robotic arm through a pre-determined loop.
The test function will move an arm to and from two points while also clossing and opening the gripper. 

Open a new terminal and source your workspace.
Run the following command to run the test on the arm movements.
```
bash
ros2 run robotic_arm_sys_test arm_gripper_loop_controller
```


https://github.com/user-attachments/assets/aa2642b9-fc6b-42b6-ae11-be3b6b9dd031


