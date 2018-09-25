Kddrescueview
=============

Description
-----------

Kddrescueview is a graphical viewer for [GNU 
ddrescue](https://www.gnu.org/software/ddrescue/) mapfiles. It shows damaged 
and recovered disk areas on a colored grid. Kddrescueview provides a KPart for 
integration in other KDE applications.

Kddrescueview is just an exercise to improve my C++/Qt/KDE skills (see below 
the [Development goal](#development-goal) section). It is a blade copy of Martin 
Bittermann's excellent 
[ddrescueview](https://sourceforge.net/projects/ddrescueview/). Unless you need 
the KDE integration, you probably want to use ddrescueview which offers much 
more features.

This program is free software released under the GNU GPL version 2 or (at your 
option) version 3 or any later version accepted by the membership of KDE e.V.


Development goal
----------------

I first had a look at Martin Bittermann's 
[ddrescueview](https://sourceforge.net/projects/ddrescueview/) source code a 
few years ago. I had used ddrescueview to monitor the progress of disk 
recovery. Its internal is quite simple: it parses simple text files (GNU 
ddrescue mapfiles) and displays their contents on a colored grid which looks 
like the familiar defrag utility on Windows. ddrescueview is developed in [Free 
Pascal](https://www.freepascal.org/) with the [Lazarus 
IDE](https://www.lazarus-ide.org/). Although at the time I was learning Python 
programming and I was not interested in Pascal, it made ddrescueview a perfect 
choice for studying its source code. Pascal is a very clear programming 
language. It was also familiar to me as it is also the first programming 
language that I ever used when I was only 8. As an exercise, I wrote 
ddrescueview parser in Python, trying to make it the more Pythonic I could.

In 2018, I wanted to improve my skills in C++ and learn how to program with 
[Qt](https://www.qt.io/). As a long time user of KDE, I was also interested
in KPart technology. Back in 2000, KParts was what made KDE 2 stands appart. It 
allowed viewing PDF documents directly in the web browser, a feature that only 
Konqueror had at the time. Integration went further when most KDE 3 
applications featured a KPart. It was possible to view a DVI file directly in 
the browser when browsing an FTP server. This made it difficult for new users 
who where not familiar with network transparency and application integration. 
The technology lost its traction with KDE 4. The tutorial to convert an existing 
application into KPart was not updated with the move to KDE Plasma 5.

Luckily, KDevelop 5 features an updated version of the KPart template. The 
development of Kddrescueview started from this template. I based the parser on 
the Python version, separating it even more from the Pascal code it drew 
inspiration from. Using a different widget framework, Qt instead of Lazarus, the 
source code is rather different although I tried to get the same graphical 
result.

So far the program is usable but a bit slow to refresh with a full screen 
window.


How To Build This Project
-------------------------

--- On Unix:

cd kddrescueview
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$PROJECTINSTALLDIR -DCMAKE_BUILD_TYPE=Debug ..   <- do not forget the ..
make
make install or su -c 'make install'

where $PROJECTINSTALLDIR points to your installation prefix.

to uninstall the project:
make uninstall or su -c 'make uninstall'

Note: you can use another build path. Then cd in your build dir and:
export KDE_SRC=path_to_your_src
cmake $KDE_SRC -DCMAKE_INSTALL_PREFIX=$PROJECTINSTALLDIR 
-DCMAKE_BUILD_TYPE=Debug

--- On Windows (not tested):

cd kddrescueview
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=%PROJECTINSTALLDIR% -DCMAKE_BUILD_TYPE=Debug ..  <- do not forget the ..
[n]make
[n]make install

where %PROJECTINSTALLDIR% points to your installation prefix.

to uninstall the project:
[n]make uninstall

Note: use nmake if you're building with the Visual Studio compiler, or make
if you're using the minGW compiler
