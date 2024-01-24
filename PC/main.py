import serial
import time
import threading
import matplotlib.pyplot as plt
from collections import deque
from tkinter import Tk, Button


class SerialReader:
    def __init__(self, port):
        self.port = port
        self.ser = None
        self.is_running = False
        self.data1 = deque(maxlen=1000)
        self.data2 = deque(maxlen=1000)
        self.data3 = deque(maxlen=1000)
        self.data4 = deque(maxlen=1000)
        self.data5 = deque(maxlen=1000)
        self.data6 = deque(maxlen=1000)
        self.lock = threading.Lock()

    def start(self):
        self.ser = serial.Serial(self.port, baudrate=115200)
        self.is_running = True
        self.read_data()

    def stop(self):
        self.is_running = False
        if self.ser:
            self.ser.close()

    def read_data(self):
        while self.is_running:
            try:
                line = self.ser.readline().decode('utf-8').strip()
                values = line.split(',')
                if len(values) == 6:
                    pid_speed_set = float(values[0])
                    pid_speed_in = float(values[1])
                    pid_imu_set = float(values[2])
                    pid_imu_in = float(values[3])
                    pid_motor_set = float(values[4])
                    pid_motor_in = float(values[5])
                    with self.lock:
                        self.data1.append(pid_speed_set)
                        self.data2.append(pid_speed_in)
                        self.data3.append(pid_imu_set)
                        self.data4.append(pid_imu_in)
                        self.data5.append(pid_motor_set)
                        self.data6.append(pid_motor_in)
            except (UnicodeDecodeError, ValueError):
                pass

    def get_data(self):
        with self.lock:
            return (
                list(self.data1),
                list(self.data2),
                list(self.data3),
                list(self.data4),
                list(self.data5),
                list(self.data6),
            )


class Plotter:
    def __init__(self, reader):
        self.reader = reader
        self.is_running = False
        self.fig, (self.ax1, self.ax2, self.ax3) = plt.subplots(3, 1, sharex="all")
        self.line1, = self.ax1.plot([], label='set point')
        self.line2, = self.ax1.plot([], label='input')
        self.line3, = self.ax2.plot([], label='set point')
        self.line4, = self.ax2.plot([], label='input')
        self.line5, = self.ax3.plot([], label='set point')
        self.line6, = self.ax3.plot([], label='input')
        self.update_interval = 0.05  # Update plot every 0.1 seconds (10 times per second)
        self.lock = threading.Lock()

        # Set titles for subplots
        self.ax1.set_title('PID Speed')
        self.ax2.set_title('PID IMU')
        self.ax3.set_title('PID Motor')

    def start(self):
        self.is_running = True
        self.plot_data_thread()

    def stop(self):
        self.is_running = False

    def plot_data_thread(self):
        while self.is_running:
            (
                data1,
                data2,
                data3,
                data4,
                data5,
                data6,
            ) = self.reader.get_data()
            with self.lock:
                self.line1.set_data(range(len(data1)), data1)
                self.line2.set_data(range(len(data2)), data2)
                self.line3.set_data(range(len(data3)), data3)
                self.line4.set_data(range(len(data4)), data4)
                self.line5.set_data(range(len(data5)), data5)
                self.line6.set_data(range(len(data6)), data6)

            self.ax1.relim()
            self.ax1.autoscale_view(scaley=True)
            self.ax2.relim()
            self.ax2.autoscale_view(scaley=True)
            self.ax3.relim()
            self.ax3.autoscale_view(scaley=True)
            self.ax1.legend(loc='upper right', bbox_to_anchor=(1, 1))
            self.ax2.legend(loc='upper right', bbox_to_anchor=(1, 1))
            self.ax3.legend(loc='upper right', bbox_to_anchor=(1, 1))

            self.fig.canvas.draw()
            time.sleep(self.update_interval)

    def show_plot(self):
        self.ax1.legend()
        self.ax2.legend()
        self.ax3.legend()
        plt.subplots_adjust(hspace=0.4)  # Adjust vertical spacing between subplots
        plt.show()


class GUI:
    def __init__(self, reader):
        self.reader = reader
        self.root = Tk()
        self.root.title("UART Data Sender")
        
        # Create three buttons, each associated with a specific character
        self.button1 = Button(self.root, text="Send 0x80", command=lambda: self.send_data(0x80))
        self.button1.pack()
        
        self.button2 = Button(self.root, text="Send 0x7F", command=lambda: self.send_data(0x7F))
        self.button2.pack()
        
        self.button3 = Button(self.root, text="Send 0x00", command=lambda: self.send_data(0x00))
        self.button3.pack()

    def send_data(self, char_value):
        # Convert the integer to bytes and send it through UART
        data_to_send = char_value.to_bytes(1, byteorder='big')
        self.reader.ser.write(data_to_send)
        print(f"Data sent: {data_to_send}")

    def run(self):
        self.root.mainloop()

def main():
    reader = SerialReader('COM12')
    plotter = Plotter(reader)
    gui = GUI(reader)

    reader_thread = threading.Thread(target=reader.start)
    plotter_thread = threading.Thread(target=plotter.start)
    gui_thread = threading.Thread(target=gui.run)

    reader_thread.start()
    plotter_thread.start()
    gui_thread.start()

    try:
        plotter.show_plot()
    except KeyboardInterrupt:
        pass
    finally:
        reader.stop()
        plotter.stop()
        reader_thread.join()
        plotter_thread.join()
        gui_thread.join()

if __name__ == '__main__':
    main()