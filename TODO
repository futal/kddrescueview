Kddrescueview To-do List
========================

Developement goals
------------------

 - Speed up the grid paintEvent.
 - Maybe derive RescueMap from QAbstractTableModel to avoid the overhead from QStandardItem.
 - Maybe use a custom delegate (as in the [Pixelator Example](http://doc.qt.io/qt-5/qtwidgets-itemviews-pixelator-example.html)).
 - Add a pie chart and table with the rescue totals (see QChartView, QPieSeries and QPieSlice).
 - Add a status bar (see QStatusBar)

 
Features to be on par with ddrescueview (not currently my goal)
---------------------------------------------------------------
 
 - Add a documentation.
 - Add command line options.
 - Add an option to save screenshots.
 - Add a setting to refresh the mapfile manually or automatically every 5s, 10s, 30s, 1min, 2 min,
   5min.
 - Add a setting to change the grid size.
 - Add a setting to change unit prefixes (SI prefixes as kB, MB and binary prefixes as kiB, MiB).
 - Add a setting to choose between automatic sector size and 512-, 2048- (optical drive) or 
   4096- (advanced format) byte sector size (and theoritically but not supported by ddrescue 
   520, 528, 4112, 4160, or 4224 bytes).
 - Add a zoom with a colored mini-map scroll bar (as in ddrescueview and Kate).
 - Add a view with the content of details in a grid square (as ddrescueview BlockInspector).
 
 
Features which could benefit ddrescueview as well
-------------------------------------------------

 - Refresh the mapfile automatically on mapfile changes using fswatch on all platforms or inotify 
   for Linux, kqueue for FreeBSD, ReadDirectoryChangesW on Windows, File System Events API on 
   Apple OS X or File Events Notification API on the Solaris, or automatically guessing ddrescue 
   update interval.   
 - Recognize and associate mapfiles to be open automatically with Kddrescueview (or ddrescuelog)
   See: https://freedesktop.org/wiki/Software/shared-mime-info/
   See: https://cgit.freedesktop.org/xdg/shared-mime-info/tree/HACKING
   Dependency: shared-mime-info package
   Command at installation: update-mime-database
   Create /usr/local/share/mime/packages/gddrescue.xml:

        <?xml version="1.0" encoding="UTF-8"?>
        <mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
            <mime-type type="text/gnu.ddrescue.mapfile">
                <_comment>GNU ddrescue mapfile</_comment>
                <comment xml:lang="en">GNU ddrescue mapfile</comment>
                <sub-class-of type="text/plain"/>
                <glob pattern="*.mapfile"/>  <!-- should we add this -->
                <magic priority="50">
                    <match type="string" value="# Mapfile. Created by GNU ddrescue version " offset="0"/>
                </magic>
                <magic priority="50">
                    <match type="string" value="# Logfile. Created by GNU ddrescue version " offset="0"/>
                </magic>            


                <generic-icon name="x-ddrescue-mapfile"/>
                <desktop:can-read-with>kddrescueview.desktop</desktop:can-read-with>
                <desktop:can-read-with>ddrescueview.desktop</desktop:can-read-with>
                <commandline:can-edit-with>ddrescue.desktop</desktop:can-edit-with>
                <commandline:can-edit-with>ddrescuelog.desktop</desktop:can-edit-with>
            </mime-type>
        </mime-info>

   Once you have installed the program-name.xml file, run the update-mime-database command to 
   rebuild the output files. When the Program is uninstalled, it should remove the 
   program-name.xml file and run update-mime-database again to remove the information from the 
   database.


 
