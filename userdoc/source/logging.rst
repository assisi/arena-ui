Logging
=======

All data received by the Arena UI from CASUs is logged by
default. Logging can be toggled on and off using the **Toggle Log**
Menu pane button. 

Global options
--------------

Global options affecting logging, such as *Always on on startup* and
the *Log save folder location* where all logs are saved can be
configured from the **Settings** dialog. A new subfolder, named after
the `YY-MM-DD` pattern is created inside the Log save folder location when the
first experiment of the day is run. A log file is created for every
connected CASU, named after the pattern
`YY-MM-DD-HH-MM_casuname.log`. The same file is used as long as the
Arena UI is not restarted.

Log file format
---------------

The log file is a `.csv` file where each row has the following format:

+----------+-----------+--------+-----+--------+
| DeviceID | Timestamp | Value1 | ... | ValueN |
+----------+-----------+--------+-----+--------+

Proximity sensor readings
~~~~~~~~~~~~~~~~~~~~~~~~~

IR 6x

Accelerometer readings
~~~~~~~~~~~~~~~~~~~~~~

Acc 1x

Temperature sensor readings
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Temp 4x

Peltier setpoint
~~~~~~~~~~~~~~~~

Peltier 1x

Speaker setpoint
~~~~~~~~~~~~~~~~

Speaker 2x (freq, amp?)

Airflow setpoint
~~~~~~~~~~~~~~~~

Airflow 1x
