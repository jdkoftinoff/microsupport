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
FC=
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1285622248/us_example_reactor.o


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
LDLIBSOPTIONS=../microsupport/dist/Debug/GNU-MacOSX/libmicrosupport.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Release.mk dist/Release/GNU-MacOSX/us_example_reactor

dist/Release/GNU-MacOSX/us_example_reactor: ../microsupport/dist/Debug/GNU-MacOSX/libmicrosupport.a

dist/Release/GNU-MacOSX/us_example_reactor: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-MacOSX
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/us_example_reactor ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1285622248/us_example_reactor.o: ../../../examples/us_example_reactor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1285622248
	${RM} $@.d
	$(COMPILE.c) -O2 -I../../../include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1285622248/us_example_reactor.o ../../../examples/us_example_reactor.c

# Subprojects
.build-subprojects:
	cd ../microsupport && ${MAKE}  -f Makefile CONF=Debug
	cd ../microsupport && ${MAKE}  -f Makefile CONF=Debug
	cd ../microsupport && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-MacOSX/us_example_reactor

# Subprojects
.clean-subprojects:
	cd ../microsupport && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../microsupport && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../microsupport && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
