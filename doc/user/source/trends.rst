.. _trends_link:

Displaying data trends
======================

In order to monitor the progress of experiments, the User interface
has the ability to plot the trends (time responses) all sensor values.

 | **Graph** - representation of data from single device
 | **Trend** - window that contains one or multiple graphs, legend etc.
 
Global configuration
--------------------

The basic global parameters for configuring data trends can be set
through the **Settings** menu:

  **Trend buffer size**
    Defines the maximum duration of a trend that
    can be stored, in mm:ss (minutes:seconds) format.
  **Trend sample time**
    How often should the displayed data be
    sampled, in seconds. This value is limited from below on 0.5 seconds to
    avoid unnecessary load on the system. If user is experiencing system stutter,
    increasing this parameter will lessen memory usage on the long runs.

Creating data trends
--------------------

To start trending some data, select the sensor names you are interested in
from the **Casu data Configuration tab**, and bring up the context
menu by *right-clicking* on one of the selected names. This will bring
up a context menu with two options:

  **Plot selected in same trend**
    Plots data from all selected sources in the same trend.
  **Plot selected in different trends**
    Creates a new trend with single graph for each selected source.

Whichever option you choose, additional data can be added to any trend later.

Monitoring and manipulating data trends
---------------------------------------

Once you have created some trends, you can view an manipulate them
from the **Trend graphs Configuration tab**. The following *mouse
controls* are available for manipulating data trends:

  Trend window viewport manipulation:
    **Click + mouse drag**
      Reposition graphs and stop realtime graph
      adjusting (so you can study an area of interest)
    **Ctrl + mouse scroll**
      y-axis zoom
    **Shift + mouse scroll**
      x-axis zoom
    **mouse scroll**
      x-axis & y-axis zoom
    **double click**
      resume realtime graph adjusting

  Graph selection:
    **left mouse click**
      Select single graph and deselect others
    **Ctrl + left mouse click**
      Toggle selection of single graph, while others are uneffected
    *Graph selection works either by clicking on plotted graph or by clicking its representation in legend display*

  Context menu *(right mouse click)*
    **(Un)Dock from main window**
      Depending on the current docking
      state, the trend can either be docked or undocked.
    **Hide/Show legend**
      Allows toggling of legend display.
    **Remove selected graphs**
      The graphs that are selected, on current axis, are removed from the axis.
    **Add graphs (selected in tree)**
      Data that is currently selected
      in the **Casu data Configuration tab** is added to the current axis.
    **Save to pdf**
      The current view is saved to a pdf document.
    **Close trend**
      Closes the trend.
