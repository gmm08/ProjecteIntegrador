#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Pi.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Pi.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=motorDriver.c PRAC4.c RS232.C pid.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/motorDriver.o ${OBJECTDIR}/PRAC4.o ${OBJECTDIR}/RS232.o ${OBJECTDIR}/pid.o
POSSIBLE_DEPFILES=${OBJECTDIR}/motorDriver.o.d ${OBJECTDIR}/PRAC4.o.d ${OBJECTDIR}/RS232.o.d ${OBJECTDIR}/pid.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/motorDriver.o ${OBJECTDIR}/PRAC4.o ${OBJECTDIR}/RS232.o ${OBJECTDIR}/pid.o

# Source Files
SOURCEFILES=motorDriver.c PRAC4.c RS232.C pid.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Pi.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ128GA010
MP_LINKER_FILE_OPTION=,-Tp24FJ128GA010.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/motorDriver.o: motorDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/motorDriver.o.d 
	@${RM} ${OBJECTDIR}/motorDriver.o.ok ${OBJECTDIR}/motorDriver.o.err 
	@${RM} ${OBJECTDIR}/motorDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/motorDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../prac4" -I"." -MMD -MF "${OBJECTDIR}/motorDriver.o.d" -o ${OBJECTDIR}/motorDriver.o motorDriver.c    
	
${OBJECTDIR}/PRAC4.o: PRAC4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PRAC4.o.d 
	@${RM} ${OBJECTDIR}/PRAC4.o.ok ${OBJECTDIR}/PRAC4.o.err 
	@${RM} ${OBJECTDIR}/PRAC4.o 
	@${FIXDEPS} "${OBJECTDIR}/PRAC4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../prac4" -I"." -MMD -MF "${OBJECTDIR}/PRAC4.o.d" -o ${OBJECTDIR}/PRAC4.o PRAC4.c    
	
${OBJECTDIR}/RS232.o: RS232.C  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RS232.o.d 
	@${RM} ${OBJECTDIR}/RS232.o.ok ${OBJECTDIR}/RS232.o.err 
	@${RM} ${OBJECTDIR}/RS232.o 
	@${FIXDEPS} "${OBJECTDIR}/RS232.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../prac4" -I"." -MMD -MF "${OBJECTDIR}/RS232.o.d" -o ${OBJECTDIR}/RS232.o RS232.C    
	
${OBJECTDIR}/pid.o: pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pid.o.d 
	@${RM} ${OBJECTDIR}/pid.o.ok ${OBJECTDIR}/pid.o.err 
	@${RM} ${OBJECTDIR}/pid.o 
	@${FIXDEPS} "${OBJECTDIR}/pid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../prac4" -I"." -MMD -MF "${OBJECTDIR}/pid.o.d" -o ${OBJECTDIR}/pid.o pid.c    
	
else
${OBJECTDIR}/motorDriver.o: motorDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/motorDriver.o.d 
	@${RM} ${OBJECTDIR}/motorDriver.o.ok ${OBJECTDIR}/motorDriver.o.err 
	@${RM} ${OBJECTDIR}/motorDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/motorDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../prac4" -I"." -MMD -MF "${OBJECTDIR}/motorDriver.o.d" -o ${OBJECTDIR}/motorDriver.o motorDriver.c    
	
${OBJECTDIR}/PRAC4.o: PRAC4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PRAC4.o.d 
	@${RM} ${OBJECTDIR}/PRAC4.o.ok ${OBJECTDIR}/PRAC4.o.err 
	@${RM} ${OBJECTDIR}/PRAC4.o 
	@${FIXDEPS} "${OBJECTDIR}/PRAC4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../prac4" -I"." -MMD -MF "${OBJECTDIR}/PRAC4.o.d" -o ${OBJECTDIR}/PRAC4.o PRAC4.c    
	
${OBJECTDIR}/RS232.o: RS232.C  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RS232.o.d 
	@${RM} ${OBJECTDIR}/RS232.o.ok ${OBJECTDIR}/RS232.o.err 
	@${RM} ${OBJECTDIR}/RS232.o 
	@${FIXDEPS} "${OBJECTDIR}/RS232.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../prac4" -I"." -MMD -MF "${OBJECTDIR}/RS232.o.d" -o ${OBJECTDIR}/RS232.o RS232.C    
	
${OBJECTDIR}/pid.o: pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pid.o.d 
	@${RM} ${OBJECTDIR}/pid.o.ok ${OBJECTDIR}/pid.o.err 
	@${RM} ${OBJECTDIR}/pid.o 
	@${FIXDEPS} "${OBJECTDIR}/pid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../prac4" -I"." -MMD -MF "${OBJECTDIR}/pid.o.d" -o ${OBJECTDIR}/pid.o pid.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Pi.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/Pi.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,--heap=512,--stack=16,-L"..",-L".",-Map="${DISTDIR}/Pi.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/Pi.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Pi.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,--heap=512,--stack=16,-L"..",-L".",-Map="${DISTDIR}/Pi.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Pi.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=coff
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
