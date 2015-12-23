import matplotlib.pyplot as plt
plt.ion()
class DynamicUpdate():


    def on_launch(self):
        #Set up plot
        self.figure, self.ax = plt.subplots()
        self.lines, = self.ax.plot([],[])
        #Autoscale on unknown axis and known lims on the other
        self.ax.set_autoscaley_on(True)
        self.ax.set_xlim(0, 1)
        #Other stuff
        self.ax.grid()
        

    def on_running(self, xdata, ydata):
        #Update data (with the new _and_ the old points)
        self.lines.set_xdata(xdata)
        self.lines.set_ydata(ydata)
        #Need both of these in order to rescale
        self.ax.relim()
        self.ax.set_xlim(0, self.x)
        self.ax.autoscale_view()
        #We need to draw *and* flush
        self.figure.canvas.draw()
        self.figure.canvas.flush_events()

    #Example
    def __call__(self):
        self.on_launch()
        xdata = []
        ydata = []
        self.x = 0
        while True:
            t = input()
            
            if t == '-1':
                xdata = []
                ydata = []
                self.x = 0
            else:
                xdata.append(self.x)
                ydata.append(t)
                self.x = self.x + 1
                self.on_running(xdata, ydata)
            
        return xdata, ydata

d = DynamicUpdate()
d()