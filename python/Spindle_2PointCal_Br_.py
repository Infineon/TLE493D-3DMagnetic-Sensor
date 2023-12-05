# ----------------------------------------------------------------------------------------------------------------------
# SIGI'S SPECIAL
# Author: Schaefer Jan/Heinz Richard
# Date: 20.4.202 superSigi
# Date: 27.02.2023
# ----------------------------------------------------------------------------------------------------------------------

import pandas as pd
import datetime as datetime
import serial

import serial.tools.list_ports
import numpy as np
import time

# CONSTANTS

initialPointValue = 55.0 # mT(Br) value of initial point; changes from sample to sample. Needed for plausability check of initail point
intialPointDiviation = 20.0 # deviation from intitialPointValue in percent
sensitivity = 0.065  # mT/LSB12 -> short range mode
offset = 0 # offset in mm. defines where 0mm is. does not affect calibration.
maxCalibdistance = 3 #defines in which range the calibration is suposed to take place
screwLead = 1 #defines the pitch of the screw in mm (linear travel of the screw per rotation)

# max br values @ 355° and @ nearly 180°

def round_to_multiple(number,multiple):
  return multiple*round(number/multiple)


print('-----------------------------------------------------------------------------------------------------')
print('Welcome to Sigi\'s special spindle movement!')
print('A rotation 360° leads to 1mm forward movement (screw pitch = 1mm/360°).')
print('A possible range is 0mm to 3mm or 0° to 1080°. ')
print('By linear interpolation between 1mm and 2.5mm respectively 360° to 900° the best accuracy is achieved. ')
print('The position determination is based on the absolute Br algorythm.')
print('More accuracy can be achieved by ATAN method. ')
print('Note Br: Br is temperature dependent and needs to be corrected if not used at calibrated temperature only. ')
print('Note ATAN: ATAN ist not temperature dependent! ')

print('-----------------------------------------------------------------------------------------------------')

#Com Port Connection
print('Please input com port (e.g. "COM17")')
user_input = input()
print('Trying to connect to TLI493D-W2BW 2Go kit...')
ser = serial.Serial(
port=user_input, \
    baudrate=115200, \
    parity=serial.PARITY_NONE, \
    stopbits=serial.STOPBITS_ONE, \
     bytesize=serial.EIGHTBITS, \
    timeout=1)

# Setup data frame to store measurement results
df = pd.DataFrame(columns=['Bx[LSB12]', 'By[LSB12]', 'Bz[LSB12]',
                               'Bx[mT]', 'By[mT]', 'Bz[mT]',
                               'r[mT]1', 'phi[deg]', 'theta[deg]',
                               'Position', 'Magangle[deg]', 'distance[mm]', 'Label', 'r[mT]'])

# Programm Intro
print('Successfully connected to  ' + user_input)

#Com Port Connection End

print('Device configuration: short range (100mT) with 100x averaging')
print('-----------------------------------------------------------------------------------------------------')
# Plausability Check for zero position
while True:
    print('Please go to initial reference point = 0°. Long line scale unit is aligned with screw (top view)')
    print('Proceed further by enter.')
    user_input0 = input()
    ser.write(b'm')
    serial_data = str(ser.readline()).split(',')
    #if(serial_data[0] == "b''"):
    #    print("error: Device not connected correctly! Please restart program")
    #    exit()
    #print(serial_data)
    Bx = float(serial_data[1])/100
    By = float(serial_data[2])/100
    Bz = float(serial_data[3])/100

    # Calculate measurement in mT
    Bx_mT = sensitivity * Bx
    By_mT = sensitivity * By
    Bz_mT = sensitivity * Bz
    r_mT1 = np.sqrt(Bx_mT ** 2 + By_mT ** 2 + Bz_mT ** 2)
    print(f"Initial Br @ 0° = {r_mT1: .2f} mT") #print(r_mT1)

    if ((initialPointValue - initialPointValue*intialPointDiviation) < r_mT1 < (initialPointValue + initialPointValue*intialPointDiviation)): # falsch!?
        break # While loop is stopped?
    print('Not turned to correct point. Please make sure to turn to longer reference line')
    print('If you want to override enter "override" (not recommended). To repeat the process press enter')
    user_input = input()
    if(user_input == "override"):
        print("Value plausability check was overwritten! New Br is saved!")
        print("Proceeding with calibration")
        break
#Plausability Check end

# Start Calibration
print(
    ' - Now a two point calibration will be done.  \n'
    ' - Please move' + '\033[1m' + ' counterclockwise ' + '\033[0m' + 'while calibrating. \n'
    ' - Little mechanical tolerance can lead to error. Therefore please fix the kit. \n' 
    ' - Please let go of spindle while measurements are being taken. \n'                                                                  
    ' - The smallest scale on the spindle hardware has an angle increment of 15°.')
print('-----------------------------------------------------------------------------------------------------')

# User selection of number of calibration points
isnum = 0
numList = [2]
while(isnum != 1):
    Calibpoints = "2" #input()
    if(Calibpoints.isnumeric()):
        if(int(Calibpoints) in numList):
            break
    print('Input must be: 2')
n = int(Calibpoints)

# The user enters 0--> Default Algorithm is used
distance = 0
angle = 0
i = 0
last_r_mt = 0
angles = [0, 0, 405, 360, 0, 180, 0, 0, 90]
rotations = maxCalibdistance/screwLead
rotationDeg = rotations*360
label = None
anglePerCalibPoint = None

if(n != 0):
    anglePerCalibPoint = round_to_multiple(rotationDeg/n,45)
    #print(anglePerCalibPoint) 'information of what the angle change for one calibration points is
    label = 0
if(n != 0):
   # print(' - You have chosen', n, 'calibration points'
   #      ' - Please move', angles[n],'degrees for each calibration point'
   #     )
    for i in range(n):
        # Read console input !!
        if i == 0:
            label = 360
            print(' - Move' + '\033[1m' + ' counterclockwise ' + '\033[0m' + 'to calibration position', i +1 ,'( = ' , label , '°, ',', therefore turn counter clockwise', label, '°) and enter')
        elif i == 1:
            label = 900
            print(' - Move' + '\033[1m' + ' counterclockwise ' + '\033[0m' + 'to calibration position', i +1, '( = ', label, '°, ', '1.5 rotations, therefore turn counter clockwise 540°) and enter')
        while True: #\033[1m formating text in bold, 033\[0m --> reset bold
            user_input = input()
            ser.write(b'm')
            serial_data = str(ser.readline()).split(',')
            Bx = float(serial_data[1])/100
            By = float(serial_data[2])/100
            Bz = float(serial_data[3])/100

            # Calculate measurement in mT
            Bx_mT = sensitivity * Bx
            By_mT = sensitivity * By
            Bz_mT = sensitivity * Bz

            # Calculate spherical coordinates
            r_mT1 = np.sqrt(Bx_mT ** 2 + By_mT ** 2 + Bz_mT ** 2)
            theta = np.arctan2(np.sqrt(Bx_mT ** 2 + By_mT ** 2), Bz_mT) * 180 / np.pi
            phi = np.arctan2(By_mT, Bx_mT) * 180 / np.pi
            #print("Br = {1:r_mT .2f} mT")  print(r_mT1)
            print(f"Br = {r_mT1:.2f} mT")
            if i == 0:
                if(r_mT1 != 47.5):
                    last_r_mt = r_mT1
                    break
            else: #Plausibility check for next calibration position
                if(last_r_mt+0.25 < r_mT1 or last_r_mt-0.25 > r_mT1 ):
                    last_r_mt = r_mT1
                    break
            print(" - Position was not changed since last measurement, please move to position",i+1)
        d = {'Position': i+1,
             'Bx[LSB12]': Bx,
             'By[LSB12]': By,
             'Bz[LSB12]': Bz,
             'Bx[mT]': Bx_mT,
             'By[mT]': By_mT,
             'Bz[mT]': Bz_mT,
             'r[mT]': r_mT1,
             'phi[deg]': phi,
             'theta[deg]': theta,
             'Magangle[deg]': angle,
             'distance[mm]': distance,
             'Label': label}
        df2 = pd.DataFrame(data=d, index=[0])
        df = pd.concat([df, df2], ignore_index=True)
        #label = label + anglePerCalibPoint
        time.sleep(1)
        print(df)

else:
    print(
        '- You have chosen to proceed with the default algorithm ( 60 points based calibration) and not to perform a manual calibration'
        '- Press enter to start distance calculation')
    user_input = input()

    # if user_input == 'START':
    ser.write(b'm')
    serial_data = str(ser.readline()).split(',')
    Bx = float(serial_data[1])/100
    By = float(serial_data[2])/100
    Bz = float(serial_data[3])/100

    # Calculate measurement in mT
    Bx_mT = sensitivity * Bx
    By_mT = sensitivity * By
    Bz_mT = sensitivity * Bz

    # Calculate spherical coordinates
    r_mT = np.sqrt(Bx_mT ** 2 + By_mT ** 2 + Bz_mT ** 2)
    while True:
        ser.write(b'm')
        serial_data = str(ser.readline()).split(',')
        print(serial_data)
        if(len(serial_data) != 5):
            continue
        Bx = float(serial_data[1])/100
        By = float(serial_data[2])/100
        Bz = float(serial_data[3])/100

        # Calculate measurement in mT
        Bx_mT = sensitivity * Bx
        By_mT = sensitivity * By
        Bz_mT = sensitivity * Bz

        # Calculate spherical coordinates

        r_mT = np.sqrt(Bx_mT ** 2 + By_mT ** 2 + Bz_mT ** 2)
        theta = np.arctan2(np.sqrt(Bx_mT ** 2 + By_mT ** 2), Bz_mT) * 180 / np.pi
        phi = np.arctan2(By_mT, Bx_mT) * 180 / np.pi

        distance = (r_mT - 52.354) / (-19.369)
        angle = (r_mT - 52.354) / (-0.0537)

        d = {'Position': user_input,
             'Bx[LSB12]': Bx,
             'By[LSB12]': By,
             'Bz[LSB12]': Bz,
             'Bx[mT]': Bx_mT,
             'By[mT]': By_mT,
             'Bz[mT]': Bz_mT,
             'r[mT]': r_mT1,
             'phi[deg]': phi,
             'theta[deg]': theta,
             'Magangle[deg]': angle,
             'distance[mm]': distance,
             'Label': label}
        df2 = pd.DataFrame(data=d, index=[0])
        df = pd.concat([df, df2], ignore_index=True)
        print('distance = {:.2f} mm, angle = {:.2f}, r[mt] = {:.2f} '.format(distance + offset, angle, r_mT))
        time.sleep(1)  # Introducing a delay of 1 seconds between each two different value readouts
        # After Calibration calculation step
x = df['Label'].astype(str).astype(float)
y = df['r[mT]'].astype(str).astype(float)
print(x)
print(y)
fit = np.polyfit(x, y, 1)
# fit = np.polyfit(x, y, 2)
p=np.poly1d(fit)
print(p)
print('Calibration is successful. The linear interpolation function is: ', fit)
print('Press enter to start distance calculation')
user_input2 = input()

while True:

    ser.write(b'm')
    serial_data = str(ser.readline()).split(',')
    Bx = float(serial_data[1])/100
    By = float(serial_data[2])/100
    Bz = float(serial_data[3])/100

    # Calculate measurement in mT
    Bx_mT = sensitivity * Bx
    By_mT = sensitivity * By
    Bz_mT = sensitivity * Bz

    # Calculate spherical coordinates

    r_mT = np.sqrt(Bx_mT ** 2 + By_mT ** 2 + Bz_mT ** 2)
    theta = np.arctan2(np.sqrt(Bx_mT ** 2 + By_mT ** 2), Bz_mT) * 180 / np.pi
    phi = (np.arctan2(Bz_mT, By_mT) * 180 / np.pi) + 180 #180° Offset correction for hardware

    angle = (r_mT - fit[1]) / fit[0]
    distance = angle / 360
    turn = int( angle / 360)
    #phi = turn*360+phi
    d = {'Position': user_input,
         'Bx[LSB12]': Bx,
         'By[LSB12]': By,
         'Bz[LSB12]': Bz,
         'Bx[mT]': Bx_mT,
         'By[mT]': By_mT,
         'Bz[mT]': Bz_mT,
         'r[mT]': r_mT1,
         'phi[deg]': phi,
         'theta[deg]': theta,
         'Magangle[deg]': angle,
         'distance[mm]': distance,
         'Label': label}
    df2 = pd.DataFrame(data=d, index=[0])
    df = pd.concat([df, df2], ignore_index=True)

    print('distance = {:.2f} mm, angle = {:.2f}° phi precise= {:.2f}° Br = {:.2f}mT '.format(distance+offset, angle, phi, r_mT))
    #print('turn = {:.0f} mm, angle = {:.1f}° Br = {:.21}mT phi = {:.2f}°'.format(turn, angle, r_mT, phi))
    #print('Bx = {:.1f}mT, By = {:.1f}mT, Bz = {:.1f}mT '.format(Bx_mT, By_mT, Bz_mT))
    time.sleep(1.0)  # Introducing a delay of 1 seconds between each two different value readouts