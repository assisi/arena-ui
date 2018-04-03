Deploying CASU controller code
==============================

The Arena UI provides a graphical interface to the `assisipy
deployment tools <https://assisipy.readthedocs.org/en/latest/deploy.html>`_.

Network setup
-------------

The required network setup is described in detail in the `assisipy
documentation
<https://assisipy.readthedocs.org/en/latest/deploy.html#network-setup>`_. This
generally has to be configured only once for a particular host PC.

Deploying controllers
---------------------

Deployment is performed by pressing on the *Deploy* button on the
**Deployment Configuration tab**, according to the contents of the
loaded poject (`.assisi` file). The output can be monitored in the
bottom part of the Deployment tab.


Running controllers
-------------------

The controllers are run by pressing on the *Run* button on the
**Deployment Configuration tab**, according to the contents of the
loaded poject (`.assisi` file). The output can be monitored in the
bottom part of the Deployment tab.

There is currently no way to directly interact with the running
controllers, apart from stopping all controllers with the *Stop* button.

Retrieving experimental data
----------------------------

Experimental data is collected according to project specifications
(`.assisi` file) by pressing the *Collect data* button.

Interacting with the simulator
------------------------------

If the simulator path is set correctly through the **Settings**
dialog, the simulator can be started and stopped using the
corresponding buttons of the **Deployment Configuration tab**. If a
project (`.assisi` file) is loaded when starting the simulator, CASUs
are spawned automatically.

