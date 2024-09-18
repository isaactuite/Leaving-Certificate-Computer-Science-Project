import time
import serial

arduinoData = serial.Serial('COM3', 9600)     # initializes a serial connection to arduino in com3 at a BAUD rate of 9600
time.sleep(1)      #Doesn't work sometimes without this

def check_sensor_init():  #Checks initialisation status of the sensor before continuing with the rest of the code
    while True:
        data = arduinoData.readline().decode().strip()
        if data == "init success!":
            print("Sensor initialized successfully.")
            break
        elif data == "init fail!":
            print("Sensor initialization failed. Retrying...")
            time.sleep(1)
        else:
            print("Unexpected data received:", data)

check_sensor_init()

def validate_data(heartbeat, oxygen):  #Disreguards outlying data, or data pairs where only heartrate or only SP02 data is collected
    if heartbeat == 0 or oxygen == 0 or heartbeat > 200 or oxygen > 100:
        return False
    return True

def display_data(heartbeat, oxygen):
    print(f"Heartbeat: {heartbeat} bpm, Oxygen: {oxygen}%")  #the 'f' allows you to embed expressions inside string

def analyze_wellbeing(data_list):
    if not data_list:
        print("No data for analysis.")
        return

    heartbeatList, oxygenList = zip(*data_list)      #zip() function pairs values in the two lists, and '*' unpacks the elements of data_list
    avgHeartbeat = sum(heartbeatList) / len(heartbeatList)
    avgOxygen = sum(oxygenList) / len(oxygenList)

    print(f"Average Heartbeat: {avgHeartbeat} bpm")
    print(f"Average Oxygen Level: {avgOxygen}%")

    #Defined thresholds for analysis
    heartbeatThresh = 130
    oxygenThresh = 95

    # Analysis and suggestions
    if avgHeartbeat > heartbeatThresh and avgOxygen < oxygenThresh:
        print("Wellbeing analysis: Elevated heartbeat and low oxygen level. Consult a healthcare professional.")
    elif avgHeartbeat > heartbeatThresh:
        print("Wellbeing analysis: Elevated heartbeat. Monitor the situation.")
    elif avgOxygen < oxygenThresh:
        print("Wellbeing analysis: Low oxygen level. Monitor the situation.")
    else:
        print("Wellbeing analysis: No immediate concerns.")

data_list = []  # Empty list to store data

while True:
    data = arduinoData.readline().decode().strip()  #reads and decodes the data from bytes to a string, and removes leading and trailing 'whitespaces'
    if data:                                                            #checks that it is not an empty string
        if 'start measuring' in data:      
            continue                                                      # Skip line if no valid data

        try:                                                           #starts try-except block, to catch potential errors
            heartbeat, oxygen = map(int, data.split(','))              #converts data to integers and splits data in two separated by a comma

            if validate_data(heartbeat, oxygen): #checks data is valid
                display_data(heartbeat, oxygen) #Prints data to console
                data_list.append((heartbeat, oxygen)) # Appends data to 'data_list'

                if len(data_list) % 5 == 0:                                      # Perform analysis every N data points, (can be adjusted, but it is low for display purposes)
                    analyze_wellbeing(data_list)

        except ValueError as e:                                             #catches any value errors that might occur when converting to integers
            print("Error processing data:", e)
            print("Unexpected data received:", data)