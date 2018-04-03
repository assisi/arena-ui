Logging
=======

All data received by the Arena UI from CASUs is logged by
default. Logging can be toggled on and off using the **Toggle Log**
Menu pane button. 

Global options
--------------

Global options affecting logging, such as *Always on on startup* and
the *Log save folder location* where all logs are saved can be
configured from the **Settings** dialog.

Log folder topology
-------------------

A new subfolder, named after
the `YY-MM-DD` pattern is created inside the Log save folder location when the
first experiment of the day is run. A log folder is created for every
connected CASU with its name. Inside every CASU folder are separate folders for each device where every new logging session creates individual file whose name indicates timestamp when that session stared `YY-MM-DD_HH-MM.log`.

 |  **Log folder**
 |    **YY-MM-DD** #1
 |        **casu-001**
 |            **Airflow**
 |                *YY-MM-DD_HH-MM.log* #1
 |                *YY-MM-DD_HH-MM.log* #2
 |                ...
 |            **Fft**
 |                *YY-MM-DD_HH-MM.log* #1
 |                *YY-MM-DD_HH-MM.log* #2
 |            ...
 |        **casu-002**
 |            **Airflow**
 |            ...
 |        ...
 |    **YY-MM-DD** #2
 |        **casu-001**
 |        ...
 |    ...

Log file format
---------------

The log file is a `.csv` file where each row has the following format:

    =========== ======== ===== ======
    Timestamp ; Value1 ; ... ; ValueN
    =========== ======== ===== ======

**Proximity sensor readings**
 |    Subfolder: **IR**
 |    Values: *IR_F, IR_FL, IR_BL, IR_B, IR_BR, IR_FR*

**Temperature sensor readings**
 |    Subfolder: **Temp**
 |    Values: *Temp_F, Temp_L, Temp_B, Temp_R, Temp_Top, Temp_Pcb, Temp_Ring, Temp_Wax*

**Vibration readings** (1st & 2nd Fft harmonics)
 |    Subfolder: **Fft**
 |    Values(2x): *Freq, Amp*

**Peltier setpoint**
 |    Subfolder: **Peltier**
 |    Values: *Temp, State*

**Speaker setpoint** (vibration)
 |    Subfolder: **Speaker**
 |    Values: *Freq, Amp, State*

**Airflow setpoint**
 |    Subfolder: **Airflow**
 |    Values: *Intensity, State*

**Diagnostic LED setpoint**
 |    Subfolder: **DiagnosticLed**
 |    Values: *color* [hex RRGGBB]

**Vibration pattern setpoint** (N segments)
 |    Subfolder: **VibrationPattern**
 |    Values(Nx): *Period, Freq, Amp, State*
