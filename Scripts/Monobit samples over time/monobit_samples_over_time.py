import serial
import time
import sys
import curses
import atexit
import datetime

use_curses = 0

ser = serial.Serial('/dev/tty.usbmodem1411', 230400)
#ser = serial.Serial('/dev/tty.usbmodemfd121', 230400)

SAMPLE_SIZE = 10000000


def exit_handler():
    if use_curses:
        curses.endwin()
    ser.close()
    print 'Deviation Sample Test Exiting'

def get_data_from_rng():
    if use_curses:
        screen.addstr(0, 0, "Burning initial bits...")
        screen.refresh()

    #burn some bytes (I think there may be some kind of serial buffer weirdness going on
    bytes_to_burn = 1000
    bytes_burnt = 0
    while bytes_burnt <= bytes_to_burn:
        read_count = 500
        ser.read(read_count)
        bytes_burnt += read_count

    if use_curses: screen.addstr(0, 0, "Initial bits burnt...")
   
    start_time = time.time()

    filename = "deviations.csv"
    f = open(filename, 'w')
    time_string = datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')
    f.write(time_string + "\n")
    f.close

    if not use_curses: print(time_string + "\n")
    while True:
        ones = 0
        zeros = 0
        while (ones+zeros) < SAMPLE_SIZE:
            if use_curses:
                report_progress(ones, zeros, start_time)
            byte_string = ser.read(500)
            byte_array = bytearray(byte_string)

            for b in byte_array:
                one_count = bin(b).count("1")
                ones += one_count
                zeros += 8 - one_count
        deviation = float(ones - zeros) / float(ones + zeros)
        elapsed_time = time.time() - start_time
        f = open(filename, 'a')
        f.write ("{0:1.2f}, {1:1.5f}\n".format(elapsed_time, deviation)) 
        f.close()

    f.close()
    if not use_curses:
        #percent_ones = ones / float(ones + zeros)
        deviation = float(ones - zeros) / float(ones + zeros)
        print "\n-- monobit test --\n0s: {}\n1s: {}\ndifference: {}\ntotal: {}\nDeviation: {}".format(zeros, ones, zeros - ones, zeros + ones, deviation)


def report_progress(ones, zeros, start_time):
    if(ones + zeros <= 0):
        return
    deviation = float(ones - zeros) / float(ones + zeros)
    elapsed_time = time.time() - start_time
    kbs = (( (zeros + ones) / 8.0) / elapsed_time) / 1000.0

    total_bytes = (zeros+ones) / 8
    if use_curses:
        screen.addstr(0, 0, "Total Bits: {}, Bytes: {}".format(zeros + ones, total_bytes))
        screen.addstr(1, 0, "Ones: {}, Zeros: {}".format(ones, zeros))
        screen.addstr(2, 0, "Difference: {0}    ".format(ones - zeros))
        screen.addstr(3, 0, "Deviation: {0:2.10f}    ".format(deviation))
        screen.addstr(4, 0, "Elapsed Time: {0:2.2f}".format(time.time() - start_time))
        screen.addstr(5, 0, "Percent Complete: {0:2.2f}".format((float(ones + zeros) / float(SAMPLE_SIZE)) * 100))
        screen.addstr(6, 0, "kb/s: {0:2.2f}".format(kbs))
        screen.refresh()
    else:
        progress = ((ones + zeros) / float(SAMPLE_SIZE)) * 100.0
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


if __name__ == '__main__':
    atexit.register(exit_handler)

    ser.timeout = 10.0

    reset_arduino()

    if use_curses:
        screen = curses.initscr()
        curses.noecho()
        curses.cbreak()
        screen.refresh()

    take_sample()

    if use_curses:
        curses.endwin()
    ser.close()

