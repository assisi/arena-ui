.. _treeview_link:

Displaying CASU data
======================

Tab is populated with two treeviews whose relative size to each other can be
    changed draging the **slider** between them.

    **Top treeview** shows CASUs selected in arena visualization area. The treeview can be expanded to show numerical values for all sensor values and actuator setpoints. Actuator setpoints values are colored in green/red depending on their state ON/OFF.

    **Bottom treeview**'s purpouse is for easier accessing devices from multiple CASUs at once. The treeview is populated by all selected CASU groups and one unique entry **"Selected CASUs"** which represents all currently selected CASUs. Selected groups can be distinguished by color which is same as their colors in arena visualization area.

    Several devices can be selected simultaneously by clicking and dragging on their name
    in the treeviews. User can select multiple devices by holding **Ctrl** modifier. Mixed selection between both treeviews is possible. A context menu, accessed by right-clicking on a device name, enables the plotting of sensor value trends in time, as
    described in more detail in `Displaying data trends`_ .
