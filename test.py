import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from PyQt5 import QtCore, QtWidgets, QtGui
import matplotlib.pyplot as plt
import sys
import pandas as pd
from mpl_toolkits.mplot3d import Axes3D


class plot_window(QtWidgets.QDialog):

    def __init__(self,parent=None):
        super(plot_window,self).__init__(parent)
        self.window = QtWidgets.QWidget(self)
        self.resize(653, 500)

        self.figure = plt.figure(figsize=(2, 2))
        self.canvas = FigureCanvas(self.figure)
        self.ax = plt.axes(projection='3d')

        self.import_button = QtWidgets.QPushButton('import')
        self.plot_button = QtWidgets.QPushButton('plot')

        self.import_button.clicked.connect(self.import_csv)
        self.plot_button.clicked.connect(self.plot)

        self.main_layout = QtWidgets.QVBoxLayout()
        
        self.main_layout.addWidget(self.canvas)
        self.main_layout.addWidget(self.import_button)
        self.main_layout.addWidget(self.plot_button)
        self.window.setLayout(self.main_layout)

    def import_csv(self):
        self.fileName_choose, self.filetype = QtWidgets.QFileDialog.getOpenFileName(self, "Find Files", QtCore.QDir.currentPath(), 'CSV (*.csv)')
        self.csv = pd.read_csv(self.fileName_choose, sep='\s+')
        self.nbody = int(((len(self.csv.columns) - 1) / 3))
        self.timeticks = int(len(self.csv))
    def plot(self):
        for i in range(1, self.timeticks):
            x = [0]
            y = [0]
            z = [0]
            for j in range(0, self.nbody):
                x.append(self.csv.iloc[i, int(j*3 + 1)])
                y.append(self.csv.iloc[i, int(j*3 + 2)])
                z.append(-self.csv.iloc[i, int(j*3 + 3)])
            self.ax.plot(x, y, z)
            self.canvas.draw()
            self.canvas.flush_events()

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    main_window = plot_window()
    main_window.show()
    app.exec()