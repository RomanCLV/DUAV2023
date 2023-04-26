from dronekit import connect, VehicleMode
import time
import os


# Connection string
connection_string = "/dev/ttyAMA0"  # À remplacer par le port série utilisé sur la Raspberry Pi


def connect_vehicle():
    print("Connecting to vehicle on: %s" % connection_string)
    vehicle = connect(connection_string, wait_ready=True, baud=57600)  # Utilisez le baudrate approprié
    return vehicle


def arm_and_takeoff(vehicle, aTargetAltitude):
    # Arming the drone and taking off to a specified altitude
    # Add your implementation here
    pass


def return_to_launch(vehicle):
    # Change mode to RTL
    vehicle.mode = VehicleMode("RTL")
    print("Returning to launch")


def check_for_rth_file(rth_file_path):
    if os.path.exists(rth_file_path):
        return True
    return False


def on_rc_channels_receive(vehicle, name, message):
    # récupération des valeurs des canaux RC
    rc_channels = message.channels

    log_mavlink_message("on_rc_channels_receive:")
    log_mavlink_message("Ch1: %s" % channels['1'])
    log_mavlink_message("Ch2: %s" % channels['2'])
    log_mavlink_message("Ch3: %s" % channels['3'])
    log_mavlink_message("Ch4: %s" % channels['4'])
    log_mavlink_message("Ch5: %s" % channels['5'])
    log_mavlink_message("Ch6: %s" % channels['6'])
    log_mavlink_message("Ch7: %s" % channels['7'])
    log_mavlink_message("Ch8: %s" % channels['8'])


def log_mavlink_message(message):
    log_file = "mavlink.log"

    # Obtention de l'horodatage actuel
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())

    # Construction du message de log avec horodatage
    log_message = f"{timestamp} - {message}\n"

    # Écriture du message de log dans le fichier
    with open(log_file, "a") as f:
        f.write(log_message)


def main():
    vehicle = connect_vehicle()
    vehicle.add_message_listener('RC_CHANNELS', on_rc_channels_receive)
    rth_file = "./RTH"

    try:
        while True:
            # Vérifiez les événements et agissez en conséquence
            # Par exemple, vous pouvez vérifier les waypoints atteints, les actions associées, etc.
            if check_for_rth_file(rth_file):
                return_to_launch(vehicle)
                os.remove(rth_file)

            time.sleep(1)  # À ajuster en fonction de la fréquence de vérification souhaitée

    except KeyboardInterrupt:
        print("Exiting...")

    finally:
        vehicle.close()

if __name__ == "__main__":
    main()
