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

        self.figure = plt.figure()
        self.canvas = FigureCanvas(self.figure)

        self.import_button = QtWidgets.QPushButton('import')
        self.plot_button = QtWidgets.QPushButton('plot')

        self.import_button.clicked.connect(self.import_csv)
        self.plot_button.clicked.connect(self.plot)

        self.main_layout = QtWidgets.QGridLayout()
        
        self.main_layout.addWidget(self.canvas, 0, 0, 0, 2)
        self.main_layout.addWidget(self.import_button,1, 0)
        self.main_layout.addWidget(self.plot_button,1, 1)
        self.window.setLayout(self.main_layout)

    def import_csv(self):
        self.fileName_choose, self.filetype = QtWidgets.QFileDialog.getOpenFileName(self, "Find Files", QtCore.QDir.currentPath(), 'CSV (*.csv)')
        self.csv = pd.read_csv(self.fileName_choose, sep='\s+')
        self.nbody = int(((len(self.csv.columns) - 1) / 3))
        self.timeticks = int(len(self.csv))

    def plot(self):
        ax = plt.axes(projection='3d')
        for i in range(1, self.timeticks):
            for j in range(2, self.nbody):
                ax.plot([self.csv.iloc[i, int((j-1)*3 + 2)], self.csv.iloc[i, int((j)*3 + 2)]], [self.csv.iloc[i, int((j-1)*3 + 3)], self.csv.iloc[i, int((j)*3 + 3)]], [self.csv.iloc[i, int((j-1)*3 + 4)], self.csv.iloc[i, int((j)*3 + 4)]])
                self.canvas.draw()
                self.canvas.flush_events()
if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    main_window = plot_window()
    main_window.show()
    app.exec()