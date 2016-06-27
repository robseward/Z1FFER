import sys
import glob
import serial

def choose_port(portNumber = None):
    ports = serial_ports()
    errorMessage = "Not a valid port.\n"

    if portNumber == None:
        optionsQueryString = "Select the serial port:\n"
        count = 1
        for port in ports:
            optionsQueryString += "{0}: {1}\n".format(count, port)
            count += 1
        portNumber = raw_input(optionsQueryString)
    try:
        choice = int(portNumber) - 1
    except ValueError:
        print errorMessage
        return choose_port()
    if choice < 0 or choice >= len(ports):
        print errorMessage
        return choose_port()
    return ports[choice]

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

if __name__ == '__main__':
    port = choose_port(2)
    print port
