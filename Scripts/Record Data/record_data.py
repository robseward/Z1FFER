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

sample_size = 1000000

def exit_handler():
    if use_curses:
        curses.endwin()
    if ser != None:
        ser.close()
    print 'Monobit test Exiting'

def get_data_from_rng():
    if use_curses:
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
    byte_count = 0
    ones = 0
    zeros = 0
    while byte_count < sample_size:
        if byte_count % 100 == 0 and byte_count > 0:
            report_progress(ones, zeros, byte_count, start_time)
        #if ser.inWaiting() > 0:
        byte_string = ser.read(1)
        f.write(byte_string)
        byte_array = bytearray(byte_string)

        #byte_array should always have a length of 1
        for b in byte_array:
            one_count = bin(b).count("1")
            ones += one_count
            zeros += 8 - one_count
            byte_count += 1

    f.close()
    if not use_curses:
        #percent_ones = ones / float(ones + zeros)
        deviation = float(ones - zeros) / float(ones + zeros)
        print "\n-- monobit test --\n0s: {}\n1s: {}\ndifference: {}\ntotal: {}\nDeviation: {}".format(zeros, ones, zeros - ones, zeros + ones, deviation)


def report_progress(ones, zeros, byte_count, start_time):
    if(ones + zeros <= 0):
        return
    deviation = float(ones - zeros) / float(ones + zeros)
    elapsed_time = time.time() - start_time
    total_bytes = float(zeros+ones) / 8.0
    kbs = (total_bytes / elapsed_time) / 1000.0

    if use_curses:
        screen.addstr(0, 0, "Total Bits: {}, Bytes: {}".format(zeros + ones, byte_count))
        screen.addstr(1, 0, "Ones: {}, Zeros: {}".format(ones, zeros))
        screen.addstr(2, 0, "Difference: {0}    ".format(ones - zeros))
        screen.addstr(3, 0, "Deviation: {0:2.10f}    ".format(deviation))
        screen.addstr(4, 0, "Elapsed Time: {0:2.2f}".format(elapsed_time))
        screen.addstr(5, 0, "Percent Complete: {0:2.2f}".format((float(total_bytes) / float(sample_size)) * 100))
        screen.addstr(6, 0, "kb/s: {0:2.2f}".format(kbs))
        screen.refresh()
    else:
        progress = (byte_count / float(sample_size)) * 100.0
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
    print "{0} bytes written to random.dat".format(sample_size)
