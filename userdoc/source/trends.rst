Displaying data trends
======================

In order to monitor the progress of experiments, the User interface
has the ability to plot the trends (time responses) all sensor values.

Global configuration
--------------------

The basic global parameters for configuring data trends can be set
through the **Settings** menu:

* **Trend buffer size** - Defines the maximum duration of a trend that
  can be stored, in mm:ss (minutes:seconds) format.
* **Trend sample time** - How often should the displayed data be
  sampled, in miliseconds; This value is limited from below by the
  underlying communication mechanism between the User interface and
  the CASUs, so be careful when changing it; Setting it too low will
  put an unnecessary load on the system; Generally, considering
  the speed at which the bees move, a value of 500ms seems reasonable
  and should not be modified without a compelling reason;

Creating data trends
--------------------

To start trending some data, select the sensor names you are interested in
from the **Casu data Configuration tab**, and bring up the context
menu by *right-clicking* on one of the selected names. This will bring
up a context menu with two options:

* **Plot selected in same trend** - Plots data from all selected
  sources in the same graph;
* **Plot selected in different trends** - Creates a new graph for each
  selected source

Whichever option you choose, additional data can be added to any trend later.

Monitoring and manipulating data trends
---------------------------------------

Once you have created some trends, you can view an manipulate them
from the **Trend graphs Configuration tab**. 
