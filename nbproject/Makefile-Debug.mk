#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as
QMAKE=qmake

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES=


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lxml2 -liconv -lws2_32 -lpthread "/C/Program Files/STAR-Dundee/STAR-System/lib/x86-32/star-api.lib" "/C/Program Files/STAR-Dundee/STAR-System/lib/x86-32/star_conf_api_brick_mk2.lib" "/C/Program Files/STAR-Dundee/STAR-System/lib/x86-32/star_conf_api_mk2.lib" "/C/Program Files/STAR-Dundee/STAR-System/lib/x86-32/star_conf_api_router.lib" "/C/Program Files/STAR-Dundee/SpaceWire USB Driver/code/lib/SpaceWireUSBAPI.lib" "/C/Program Files/STAR-Dundee/SpaceWire USB Driver/code/lib/ConfigLibraryUSB.lib" /C/Qwt-6.1.3/lib/libqwtd.a  

nbproject/qt-${CND_CONF}.mk: nbproject/qt-${CND_CONF}.pro FORCE
	${QMAKE} VPATH=. -spec win32-g++ -o qttmp-${CND_CONF}.mk nbproject/qt-${CND_CONF}.pro
	mv -f qttmp-${CND_CONF}.mk nbproject/qt-${CND_CONF}.mk
	@sed -e 's/\/qt\/bin/\/qt\/bin\//g' nbproject/qt-${CND_CONF}.mk >nbproject/qt-${CND_CONF}.tmp
	@mv -f nbproject/qt-${CND_CONF}.tmp nbproject/qt-${CND_CONF}.mk

FORCE:

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} nbproject/qt-${CND_CONF}.mk
	"${MAKE}" -f nbproject/qt-${CND_CONF}.mk ${CND_CONF}/massiva.exe

${CND_BUILDDIR}/Debug/%.o: nbproject/qt-${CND_CONF}.mk
	${MAKE} -f nbproject/qt-${CND_CONF}.mk "$@"

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS} nbproject/qt-${CND_CONF}.mk
	${MAKE} -f nbproject/qt-${CND_CONF}.mk distclean

# Subprojects
.clean-subprojects:
