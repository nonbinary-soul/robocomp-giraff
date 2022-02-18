#!/usr/bin/python3
# -*- coding: utf-8 -*-
#
#    Copyright (C) 2022 by YOUR NAME HERE
#
#    This file is part of RoboComp
#
#    RoboComp is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    RoboComp is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
#

from PySide2.QtCore import QTimer
from PySide2.QtWidgets import QApplication
from rich.console import Console
from genericworker import *
import interfaces as ifaces
import numpy as np
import cv2, json
sys.path.append('/opt/robocomp/lib')
console = Console(highlight=False)
import matplotlib.pyplot as plt
from pydsr import *


# If RoboComp was compiled with Python bindings you can use InnerModel in Python
# import librobocomp_qmat
# import librobocomp_osgviewer
# import librobocomp_innermodel

MIN_VELOCITY = 0.05
TOLERANCE = 0.001

class SpecificWorker(GenericWorker):
    def __init__(self, proxy_map, startup_check=False):
        super(SpecificWorker, self).__init__(proxy_map)
        self.Period = 10

        QObject.connect(self.ui.horizontalSlider_pos, SIGNAL('valueChanged(int)'), self.slot_change_pos)
        QObject.connect(self.ui.horizontalSlider_max_speed, SIGNAL('valueChanged(int)'), self.slot_change_max_speed)
        QObject.connect(self.ui.pushButton_center, SIGNAL('clicked()'), self.slot_center)
        QObject.connect(self.ui.pushButton, SIGNAL('clicked()'), self.slot_track)
        self.motor = self.jointmotorsimple_proxy.getMotorState("")
        self.current_max_speed = 0.0
        self.ui.horizontalSlider_pos.setSliderPosition(self.motor.pos)

        self.faceList = ["0", "1", "2", "3", "4"]
        self.hipList = ["12", "13"]
        self.chestList = ["6", "7"]

        self.k1 = 0.8
        self.k2 = 1
        self.k3 = 0.9
        self.k4 = 1
        self.k5 = 0.7
        self.k6 = 2
        self.k7 = 5

        self.error_ant = 0
        self.rad_old = 0

        self.rotational_speed_coefficients=[0,0,0]
        self.rotational_speed_avg=0

        with open('human_pose.json', 'r') as f:
            self.human_pose = json.load(f)
        plt.ion()
        self.fig, self.ax = plt.subplots()
        self.ax.set_ylabel("y", fontsize=14)
        self.ax.set_xlabel("x", fontsize=14)
        self.ax.axis([-3, 3, -3, 3])

        # YOU MUST SET AN UNIQUE ID FOR THIS AGENT IN YOUR DEPLOYMENT. "_CHANGE_THIS_ID_" for a valid unique integer
        self.agent_id = 968
        self.g = DSRGraph(0, "eye_control", self.agent_id)


        try:
            signals.connect(self.g, signals.UPDATE_NODE_ATTR, self.update_node_att)
            signals.connect(self.g, signals.UPDATE_NODE, self.update_node)
            signals.connect(self.g, signals.DELETE_NODE, self.delete_node)
            signals.connect(self.g, signals.UPDATE_EDGE, self.update_edge)
            signals.connect(self.g, signals.UPDATE_EDGE_ATTR, self.update_edge_att)
            signals.connect(self.g, signals.DELETE_EDGE, self.delete_edge)
            console.print("signals connected")
        except RuntimeError as e:
            print(e)

        if startup_check:
            self.startup_check()
        else:
            self.timer.timeout.connect(self.compute)
            self.timer.start(self.Period)


    def __del__(self):
        """Destructor"""

    def setParams(self, params):
        # try:
        #	self.innermodel = InnerModel(params["InnerModelPath"])
        # except:
        #	traceback.print_exc()
        #	print("Error reading config params")
        return True


    @QtCore.Slot()
    def compute(self):
        #obtenemos datos
        color, people_data = self.obtencion_datos()

        image = np.frombuffer(color.image, np.uint8).reshape(color.width, color.height, color.depth)
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

        # # Draw body parts on image
        image = self.proceso_esqueleto(image, people_data)

        people_nodes = self.g.get_nodes_by_type('person')
        if len(people_nodes) == 0:
            pass

        for person in people_nodes:
            is_followed = person.attrs['followed'].value
            is_lost = person.attrs['lost'].value
            print(is_followed)
            if is_followed:
                if is_lost:
                    goal = ifaces.RoboCompJointMotorSimple.MotorGoalPosition()
                    goal.maxSpeed = 0
                    goal.position = 0
                    self.jointmotorsimple_proxy.setPosition("", goal)
                else:
                    self.tracker_camera(color, person.attrs['pixel_x'].value)
            elif abs(self.motor.pos > 0.05):
                goal = ifaces.RoboCompJointMotorSimple.MotorGoalPosition()
                goal.maxSpeed = 0
                goal.position = 0
                self.jointmotorsimple_proxy.setPosition("", goal)



        self.refesco_ventana(color, image)

    def tracker_camera(self, color, puntoMedioX):
        error = puntoMedioX - color.width / 2
        goal = ifaces.RoboCompJointMotorSimple.MotorGoalPosition()
        der_error = -(error - self.error_ant)
        error_rads = np.arctan2(error, color.focalx)
        der_error_rads = np.arctan2(der_error, color.focalx)

        # goal_rad = self.motor.pos - error_rads - self.rotational_speed_avg * (self.Period_camera/1000)
        # goal_rad = self.motor.pos - (
        #             self.k1 * error_rads + self.k2 * der_error_rads) + self.rotational_speed_avg * (
        #                        self.Period / 1000)

        goal_rad = self.motor.pos - (self.k1 * error_rads + self.k2 * der_error_rads)
        print("GOAL RADS: ", goal_rad)

        # Se mueve el sujeto?
        if self.rad_old > goal_rad + TOLERANCE or self.rad_old < goal_rad - TOLERANCE:
            rad_seg = self.k5 * error_rads + self.k2 * der_error_rads
            print(rad_seg)
            # filtramos velicidad, ya que 0 es maxima
            print("error rads: ", error_rads)
            if abs(error_rads) > 0.1:
                goal.maxSpeed = 0
            else:
                if abs(rad_seg) < MIN_VELOCITY:
                    goal.maxSpeed = MIN_VELOCITY
                else:
                    goal.maxSpeed = rad_seg
            print("max speed: ", goal.maxSpeed)
            goal.position = goal_rad
            # mandamos al motor el objetivo
            self.jointmotorsimple_proxy.setPosition("", goal)
            self.error_ant = error
            self.rad_old = goal_rad

    def obtencion_datos(self):
        try:
            self.motor = self.jointmotorsimple_proxy.getMotorState("")
        except:
            print("Ice error communicating with JointMotorSimple interface")

        try:
            color = self.camerargbdsimple_proxy.getImage("")
        except:
            print("Ice error communicating with CameraRGBDSimple interface")

        try:
            people_data = self.humancamerabody_proxy.newPeopleData()
            # print(list(people_data.peoplelist[0].joints.keys()))
        except:
            traceback.print_exc()
            print("Ice error communicating with HumaCameraBody interface")
        return color, people_data

    def proceso_esqueleto(self, image, people_data):

        # grey = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

        if len(people_data.peoplelist) > 0:
            for person in people_data.peoplelist:

                for name1, name2 in self.human_pose["skeleton"]:
                    try:
                        # Printing bones
                        # if str(name1) in list(person.joints.keys()) and str(name2) in list(person.joints.keys()):
                        #    print(name1, name2)
                        joint1 = person.joints[str(name1)]
                        joint2 = person.joints[str(name2)]
                        cv2.line(image, (joint1.i, joint1.j), (joint2.i, joint2.j), (0, 255, 0), 2)
                    except:
                        pass

                faceNameList = []
                hipNameList = []
                chestNameList = []

                point_list = []

                for key_point in list(person.joints.keys()):
                    point_list.append([person.joints[key_point].i, person.joints[key_point].j])

                    if key_point in self.faceList:
                        faceNameList.append(key_point)
                    if key_point in self.hipList:
                        hipNameList.append(key_point)
                    if key_point in self.chestList:
                        chestNameList.append(key_point)

                # Create dictionarý to append some skeleton data
                distances = {}
                distance_list = []
                puntoMedioX = None
                if len(hipNameList) == 2:
                    distances["hips"] = (self.get_pos_esqueleto(person, hipNameList))
                    cv2.circle(image, (int(distances["hips"][0] - 10), int(distances["hips"][1] - 10)), 10, (0, 255, 150), 2)
                    puntoMedioX = distances["hips"][0]

                if len(chestNameList) == 2:
                    distances["chest"] = (self.get_pos_esqueleto(person, chestNameList))
                    cv2.circle(image, (int(distances["chest"][0] - 10), int(distances["chest"][1] - 10)), 10, (0, 150, 255), 2)
                    puntoMedioX = distances["chest"][0]

                if len(faceNameList) == 5:
                    distances["face"] = (self.get_pos_esqueleto(person, faceNameList))
                    cv2.circle(image, (int(distances["face"][0] - 10), int(distances["face"][1] - 10)), 10, (255, 0, 0), 2)
                    puntoMedioX = distances["face"][0]

                if puntoMedioX:
                    font = cv2.FONT_HERSHEY_SIMPLEX
                    cv2.putText(image, str(person.id), (int(puntoMedioX), 450), font, 3, (0, 255, 255), 2, cv2.LINE_AA)

        return image

    def draw_error_data(self):
        self.d_camera_position_error.extend([self.error_dict["camera_position_error"][-1]])
        self.d_camera_audio_angle_error.extend([self.error_dict["camera_audio_angle_error"][-1]])
        self.dx.extend([self.x_data])
        self.camera_position_error.set_ydata(self.d_camera_position_error)
        self.camera_position_error.set_xdata(self.dx)
        self.camera_audio_angle_error.set_ydata(self.d_camera_audio_angle_error)
        self.camera_audio_angle_error.set_xdata(self.dx)

        # set axes
        self.ah1.set_ylim(-1, 1)
        self.ah1.set_xlim(self.x_data-self.visible, self.x_data)
        self.ah2.set_ylim(-math.pi, math.pi)
        self.ah2.set_xlim(self.x_data-self.visible, self.x_data)

        self.x_data += 1

        self.fig.canvas.draw()
        self.fig.canvas.flush_events()

    def get_pos_esqueleto(self, person, list):

        # Calculating middle points for eyes and hips

        puntoMedioX = (person.joints[list[0]].i + person.joints[list[1]].i) / 2.0
        puntoMedioY = (person.joints[list[0]].j + person.joints[list[1]].j) / 2.0

        z0 = person.joints[list[0]].z
        z1 = person.joints[list[1]].z

        if z0 != 0:
            if abs(z0 - z1) < 0.5:
                distance = (z0 + z1) / 2.0
                # print("puntos ", z0, z1, "dist", distance)
            else:
                # print("fallo poniendo punto ", z0)
                distance = z0
        else:
            distance = 0
        return puntoMedioX, puntoMedioY, distance

    def refesco_ventana(self, color, image):
        qt_image = QImage(image, color.height, color.width, QImage.Format_RGB888)
        pix = QPixmap.fromImage(qt_image).scaled(self.ui.label_image.width(), self.ui.label_image.height())
        self.ui.label_image.setPixmap(pix)
        # image = np.frombuffer(color.image, np.uint8).reshape(color.height, color.width, color.depth)

        self.ui.lcdNumber_pos.display(self.motor.pos)
        self.ui.lcdNumber_speed.display(self.motor.vel)
        self.ui.lcdNumber_temp.display(self.motor.temperature)
        self.ui.lcdNumber_max_speed.display(self.current_max_speed)
        if self.motor.isMoving:
            self.ui.radioButton_moving.setChecked(True)
        else:
            self.ui.radioButton_moving.setChecked(False)

    @QtCore.Slot()
    def slot_change_pos(self, pos):   # comes in degrees -150 .. 150. Sent in radians -2.62 .. 2.62
        goal = ifaces.RoboCompJointMotorSimple.MotorGoalPosition()
        goal.position = (2.62/150.0)*pos
        goal.maxSpeed = self.current_max_speed
        self.jointmotorsimple_proxy.setPosition("", goal)

    @QtCore.Slot()
    def slot_change_max_speed(self, max_speed):
        self.current_max_speed = max_speed*0.111/60*np.pi*2.0

    @QtCore.Slot()
    def slot_center(self):
        self.ui.horizontalSlider_pos.setSliderPosition(0)
        self.slot_change_pos(0)

    @QtCore.Slot()
    def slot_track(self):
        self.track = not self.track
        if not self.track:
            print("Guardando")
            with open("error_data.json", 'w+') as file:
                json.dump(self.error_dict, file, indent=4)
        print("state track", self.track)

    def startup_check(self):
        print(f"Testing RoboCompCameraRGBDSimple.TImage from ifaces.RoboCompCameraRGBDSimple")
        test = ifaces.RoboCompCameraRGBDSimple.TImage()
        print(f"Testing RoboCompCameraRGBDSimple.TDepth from ifaces.RoboCompCameraRGBDSimple")
        test = ifaces.RoboCompCameraRGBDSimple.TDepth()
        print(f"Testing RoboCompCameraRGBDSimple.TRGBD from ifaces.RoboCompCameraRGBDSimple")
        test = ifaces.RoboCompCameraRGBDSimple.TRGBD()
        print(f"Testing RoboCompDifferentialRobot.TMechParams from ifaces.RoboCompDifferentialRobot")
        test = ifaces.RoboCompDifferentialRobot.TMechParams()
        print(f"Testing RoboCompHumanCameraBody.TImage from ifaces.RoboCompHumanCameraBody")
        test = ifaces.RoboCompHumanCameraBody.TImage()
        print(f"Testing RoboCompHumanCameraBody.TGroundTruth from ifaces.RoboCompHumanCameraBody")
        test = ifaces.RoboCompHumanCameraBody.TGroundTruth()
        print(f"Testing RoboCompHumanCameraBody.KeyPoint from ifaces.RoboCompHumanCameraBody")
        test = ifaces.RoboCompHumanCameraBody.KeyPoint()
        print(f"Testing RoboCompHumanCameraBody.Person from ifaces.RoboCompHumanCameraBody")
        test = ifaces.RoboCompHumanCameraBody.Person()
        print(f"Testing RoboCompHumanCameraBody.PeopleData from ifaces.RoboCompHumanCameraBody")
        test = ifaces.RoboCompHumanCameraBody.PeopleData()
        print(f"Testing RoboCompJointMotorSimple.MotorState from ifaces.RoboCompJointMotorSimple")
        test = ifaces.RoboCompJointMotorSimple.MotorState()
        print(f"Testing RoboCompJointMotorSimple.MotorParams from ifaces.RoboCompJointMotorSimple")
        test = ifaces.RoboCompJointMotorSimple.MotorParams()
        print(f"Testing RoboCompJointMotorSimple.MotorGoalPosition from ifaces.RoboCompJointMotorSimple")
        test = ifaces.RoboCompJointMotorSimple.MotorGoalPosition()
        print(f"Testing RoboCompJointMotorSimple.MotorGoalVelocity from ifaces.RoboCompJointMotorSimple")
        test = ifaces.RoboCompJointMotorSimple.MotorGoalVelocity()
        print(f"Testing RoboCompMoveTowards.Command from ifaces.RoboCompMoveTowards")
        test = ifaces.RoboCompMoveTowards.Command()
        print(f"Testing RoboCompSoundRotation.Position from ifaces.RoboCompSoundRotation")
        test = ifaces.RoboCompSoundRotation.Position()
        print(f"Testing RoboCompSoundRotationPub.Position from ifaces.RoboCompSoundRotationPub")
        test = ifaces.RoboCompSoundRotationPub.Position()
        QTimer.singleShot(200, QApplication.instance().quit)


    # =============== Methods for Component SubscribesTo ================
    # ===================================================================

    #
    # SUBSCRIPTION to getAngle method from SoundRotationPub interface
    #
    def SoundRotationPub_getAngle(self, ang):
    
        #
        # write your CODE here
        #
        pass


    #
    # SUBSCRIPTION to getKeyWord method from SoundRotationPub interface
    #
    def SoundRotationPub_getKeyWord(self, word):
    
        #
        # write your CODE here
        #
        pass


    #
    # SUBSCRIPTION to getPositions method from SoundRotationPub interface
    #
    def SoundRotationPub_getPositions(self, pos):
    
        #
        # write your CODE here
        #
        pass


    #
    # SUBSCRIPTION to personFound method from SoundRotationPub interface
    #
    def SoundRotationPub_personFound(self, found):
    
        #
        # write your CODE here
        #
        pass


    # ===================================================================
    # ===================================================================


    # =============== Methods for Component Implements ==================
    # ===================================================================

    #
    # IMPLEMENTATION of setState method from Follower interface
    #
    def Follower_setState(self, state):
    
        #
        # write your CODE here
        #
        pass


    # ===================================================================
    # ===================================================================


    ######################
    # From the RoboCompCameraRGBDSimple you can call this methods:
    # self.camerargbdsimple_proxy.getAll(...)
    # self.camerargbdsimple_proxy.getDepth(...)
    # self.camerargbdsimple_proxy.getImage(...)

    ######################
    # From the RoboCompCameraRGBDSimple you can use this types:
    # RoboCompCameraRGBDSimple.TImage
    # RoboCompCameraRGBDSimple.TDepth
    # RoboCompCameraRGBDSimple.TRGBD

    ######################
    # From the RoboCompDifferentialRobot you can call this methods:
    # self.differentialrobot_proxy.correctOdometer(...)
    # self.differentialrobot_proxy.getBasePose(...)
    # self.differentialrobot_proxy.getBaseState(...)
    # self.differentialrobot_proxy.resetOdometer(...)
    # self.differentialrobot_proxy.setOdometer(...)
    # self.differentialrobot_proxy.setOdometerPose(...)
    # self.differentialrobot_proxy.setSpeedBase(...)
    # self.differentialrobot_proxy.stopBase(...)

    ######################
    # From the RoboCompDifferentialRobot you can use this types:
    # RoboCompDifferentialRobot.TMechParams

    ######################
    # From the RoboCompHumanCameraBody you can call this methods:
    # self.humancamerabody_proxy.newPeopleData(...)

    ######################
    # From the RoboCompHumanCameraBody you can use this types:
    # RoboCompHumanCameraBody.TImage
    # RoboCompHumanCameraBody.TGroundTruth
    # RoboCompHumanCameraBody.KeyPoint
    # RoboCompHumanCameraBody.Person
    # RoboCompHumanCameraBody.PeopleData

    ######################
    # From the RoboCompJointMotorSimple you can call this methods:
    # self.jointmotorsimple_proxy.getMotorParams(...)
    # self.jointmotorsimple_proxy.getMotorState(...)
    # self.jointmotorsimple_proxy.setPosition(...)
    # self.jointmotorsimple_proxy.setVelocity(...)
    # self.jointmotorsimple_proxy.setZeroPos(...)

    ######################
    # From the RoboCompJointMotorSimple you can use this types:
    # RoboCompJointMotorSimple.MotorState
    # RoboCompJointMotorSimple.MotorParams
    # RoboCompJointMotorSimple.MotorGoalPosition
    # RoboCompJointMotorSimple.MotorGoalVelocity

    ######################
    # From the RoboCompMoveTowards you can call this methods:
    # self.movetowards_proxy.move(...)

    ######################
    # From the RoboCompMoveTowards you can use this types:
    # RoboCompMoveTowards.Command

    ######################
    # From the RoboCompSoundRotation you can call this methods:
    # self.soundrotation_proxy.getAngle(...)
    # self.soundrotation_proxy.getKeyWord(...)
    # self.soundrotation_proxy.getPositions(...)
    # self.soundrotation_proxy.personFound(...)

    ######################
    # From the RoboCompSoundRotation you can use this types:
    # RoboCompSoundRotation.Position

    ######################
    # From the RoboCompSoundRotationPub you can use this types:
    # RoboCompSoundRotationPub.Position



    # =============== DSR SLOTS  ================
    # =============================================

    def update_node_att(self, id: int, attribute_names: [str]):
        console.print(f"UPDATE NODE ATT: {id} {attribute_names}", style='green')

    def update_node(self, id: int, type: str):
        console.print(f"UPDATE NODE: {id} {type}", style='green')

    def delete_node(self, id: int):
        console.print(f"DELETE NODE:: {id} ", style='green')

    def update_edge(self, fr: int, to: int, type: str):

        console.print(f"UPDATE EDGE: {fr} to {type}", type, style='green')

    def update_edge_att(self, fr: int, to: int, type: str, attribute_names: [str]):
        console.print(f"UPDATE EDGE ATT: {fr} to {type} {attribute_names}", style='green')

    def delete_edge(self, fr: int, to: int, type: str):
        console.print(f"DELETE EDGE: {fr} to {type} {type}", style='green')