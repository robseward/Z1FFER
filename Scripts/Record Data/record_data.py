import serial
import time
import sys
import curses
import atexit
import imp

portLister = imp.load_source('portLister', '../Shared/portLister.py')

use_curses = 1

BAUD_RATE = 2000000
ser = None
#ser = serial.Serial('/dev/tty.usbmodem1411', 2000000)
#ser = serial.Serial('/dev/tty.usbmodemfd121', 230400)

# sample_size = 104857600
# sample_size = 314572800
# sample_size = 125000000
# sample_size = 39321600
sample_size = 100000



def exit_handler():
    if use_curses:
        curses.endwin()
    if ser != None:
        ser.close()
    print 'Monobit test Exiting'

def get_data_from_rng():
    screen.addstr(0, 0, "Burning initial bits...")
    screen.refresh()

    #burn some bytes (I think there may be some kind of serial buffer weirdness going on
    bytes_to_burn = 300
    bytes_burnt = 0
    while bytes_burnt < bytes_to_burn:
        ser.read(1)
        bytes_burnt += 1

    start_time = time.time()
    f = open("random.dat", 'w')
    bytes_list = []
    ones = 0
    zeros = 0
    while len(bytes_list) < sample_size:
        if len(bytes_list) % 100 == 0:
            report_progress(ones, zeros, bytes_list, start_time)
        #if ser.inWaiting() > 0:
        byte_string = ser.read(1)
        f.write(byte_string)
        byte_array = bytearray(byte_string)

        #byte_array should always have a length of 1
        for b in byte_array:
            one_count = bin(b).count("1")
            ones += one_count
            zeros += 8 - one_count
        bytes_list.append(byte_string)

    f.close()
    if not use_curses:
        #percent_ones = ones / float(ones + zeros)
        deviation = float(ones - zeros) / float(ones + zeros)
        print "\n-- monobit test --\n0s: {}\n1s: {}\ndifference: {}\ntotal: {}\nDeviation: {}".format(zeros, ones, zeros - ones, zeros + ones, deviation)


def report_progress(ones, zeros, bytes_list, start_time):
    if(ones + zeros <= 0):
        return
    deviation = float(ones - zeros) / float(ones + zeros)
    elapsed_time = time.time() - start_time
    kbs = (((zeros + ones) / 8.0) / elapsed_time) / 1000.0

    total_bytes = (zeros+ones) / 8
    if use_curses:
        screen.addstr(0, 0, "Total Bits: {}, Bytes: {}".format(zeros + ones, total_bytes))
        screen.addstr(1, 0, "Ones: {}, Zeros: {}".format(ones, zeros))
        screen.addstr(2, 0, "Difference: {0}    ".format(ones - zeros))
        screen.addstr(3, 0, "Deviation: {0:2.10f}    ".format(deviation))
        screen.addstr(4, 0, "Elapsed Time: {0:2.2f}".format(time.time() - start_time))
        screen.addstr(5, 0, "Percent Complete: {0:2.2f}".format((float(total_bytes) / float(sample_size)) * 100))
        screen.addstr(6, 0, "kb/s: {0:2.2f}".format(kbs))
        screen.refresh()
    else:
        progress = (len(bytes_list) / float(sample_size)) * 100.0
        progress_str = "{0:2.2f}%\r".format(progress)
        sys.stdout.write(progress_str)


def reset_arduino():
    ser.setDTR(0)
    time.sleep(0.22)
    ser.setDTR(1)


def take_sample():
    with Timer() as t:
        get_data_from_rng()


class Timer:
    def __enter__(self):
        self.start = time.clock()
        return self

    def __exit__(self, *args):
        self.end = time.clock()
        self.interval = self.end - self.start

def user_input_bytes():
    queryString = "Enter the number of bytes to record:\n"
    errorMessage = "Not a valid number.\n"

    userInput = raw_input(queryString)
    try:
        numBytes = int(userInput)
    except ValueError:
        print(errorMessage)
        return input_bytes()
    return numBytes

if __name__ == '__main__':
    portName = portLister.choose_port()
    ser = serial.Serial(portName, BAUD_RATE)
    sample_size = user_input_bytes()

    atexit.register(exit_handler)

    if use_curses:
        screen = curses.initscr()
        curses.noecho()
        curses.cbreak()
        screen.refresh()

    ser.timeout = 10.0
    reset_arduino()

    take_sample()

    if use_curses:
        curses.endwin()
    ser.close()
