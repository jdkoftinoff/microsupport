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
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1386528437/us_test_queue_main.o \
	${OBJECTDIR}/_ext/1386528437/us_logger_printer.o \
	${OBJECTDIR}/_ext/1386528437/us_osc_dispatch.o \
	${OBJECTDIR}/_ext/1386528437/us_cgi.o \
	${OBJECTDIR}/_ext/1386528437/us_buffer_print.o \
	${OBJECTDIR}/_ext/1386528437/us_queue.o \
	${OBJECTDIR}/_ext/1386528437/us_test_net_main.o \
	${OBJECTDIR}/_ext/1386528437/us_platform.o \
	${OBJECTDIR}/_ext/1386528437/us_json_parser.o \
	${OBJECTDIR}/_ext/1386528437/us_logger_stdio.o \
	${OBJECTDIR}/_ext/1386528437/us_net.o \
	${OBJECTDIR}/_ext/1386528437/us_test_midi_main.o \
	${OBJECTDIR}/_ext/1386528437/us_test_osc_msg_main.o \
	${OBJECTDIR}/_ext/1386528437/us_trie_dump.o \
	${OBJECTDIR}/_ext/1386528437/us_http.o \
	${OBJECTDIR}/_ext/1386528437/us_test_slip_main.o \
	${OBJECTDIR}/_ext/1386528437/us_buffer.o \
	${OBJECTDIR}/_ext/1386528437/us_world.o \
	${OBJECTDIR}/_ext/1386528437/us_logger_syslog.o \
	${OBJECTDIR}/_ext/1386528437/us_reactor.o \
	${OBJECTDIR}/_ext/1386528437/us_parse.o \
	${OBJECTDIR}/_ext/1285622248/us_example_reactor.o \
	${OBJECTDIR}/_ext/1285622248/us_example_hex_udp_rx.o \
	${OBJECTDIR}/_ext/1386528437/us_osc_msg.o \
	${OBJECTDIR}/_ext/1285622248/us_example_mudp_tx.o \
	${OBJECTDIR}/_ext/1386528437/us_trie_rw.o \
	${OBJECTDIR}/_ext/1386528437/us_test_allocator_main.o \
	${OBJECTDIR}/_ext/1386528437/us_test_http_main.o \
	${OBJECTDIR}/_ext/1386528437/us_json.o \
	${OBJECTDIR}/_ext/1386528437/us_test_line_parse_main.o \
	${OBJECTDIR}/_ext/1386528437/us_test_buffer_main.o \
	${OBJECTDIR}/_ext/1386528437/us_line_parse.o \
	${OBJECTDIR}/_ext/1386528437/us_trie_ro.o \
	${OBJECTDIR}/_ext/1386528437/us_allocator_print.o \
	${OBJECTDIR}/_ext/1285622248/us_example_mudp_rx.o \
	${OBJECTDIR}/_ext/1285622248/us_example_json.o \
	${OBJECTDIR}/_ext/1386528437/us_print.o \
	${OBJECTDIR}/_ext/1285622248/us_example_hex_udp_tx.o \
	${OBJECTDIR}/_ext/1386528437/us_test_trie_main.o \
	${OBJECTDIR}/_ext/1386528437/us_slip.o \
	${OBJECTDIR}/_ext/1386528437/us_logger.o \
	${OBJECTDIR}/_ext/1386528437/us_midi.o \
	${OBJECTDIR}/_ext/1285622248/us_example_httpd.o \
	${OBJECTDIR}/_ext/1386528437/us_serial.o \
	${OBJECTDIR}/_ext/1386528437/us_logger_udp.o \
	${OBJECTDIR}/_ext/1386528437/us_osc_msg_print.o \
	${OBJECTDIR}/_ext/1386528437/us_time.o \
	${OBJECTDIR}/_ext/1386528437/us_testutil.o \
	${OBJECTDIR}/_ext/1386528437/us_test_json_main.o \
	${OBJECTDIR}/_ext/1386528437/us_allocator.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-MacOSX/libmicrosupport.a

dist/Debug/GNU-MacOSX/libmicrosupport.a: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-MacOSX
	${RM} dist/Debug/GNU-MacOSX/libmicrosupport.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmicrosupport.a ${OBJECTFILES} 
	$(RANLIB) dist/Debug/GNU-MacOSX/libmicrosupport.a

${OBJECTDIR}/_ext/1386528437/us_test_queue_main.o: ../../../src/us_test_queue_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_queue_main.o ../../../src/us_test_queue_main.c

${OBJECTDIR}/_ext/1386528437/us_logger_printer.o: ../../../src/us_logger_printer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_logger_printer.o ../../../src/us_logger_printer.c

${OBJECTDIR}/_ext/1386528437/us_osc_dispatch.o: ../../../src/us_osc_dispatch.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_osc_dispatch.o ../../../src/us_osc_dispatch.c

${OBJECTDIR}/_ext/1386528437/us_cgi.o: ../../../src/us_cgi.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_cgi.o ../../../src/us_cgi.c

${OBJECTDIR}/_ext/1386528437/us_buffer_print.o: ../../../src/us_buffer_print.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_buffer_print.o ../../../src/us_buffer_print.c

${OBJECTDIR}/_ext/1386528437/us_queue.o: ../../../src/us_queue.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_queue.o ../../../src/us_queue.c

${OBJECTDIR}/_ext/1386528437/us_test_net_main.o: ../../../src/us_test_net_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_net_main.o ../../../src/us_test_net_main.c

${OBJECTDIR}/_ext/1386528437/us_platform.o: ../../../src/us_platform.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_platform.o ../../../src/us_platform.c

${OBJECTDIR}/_ext/1386528437/us_json_parser.o: ../../../src/us_json_parser.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_json_parser.o ../../../src/us_json_parser.c

${OBJECTDIR}/_ext/1386528437/us_logger_stdio.o: ../../../src/us_logger_stdio.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_logger_stdio.o ../../../src/us_logger_stdio.c

${OBJECTDIR}/_ext/1386528437/us_net.o: ../../../src/us_net.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_net.o ../../../src/us_net.c

${OBJECTDIR}/_ext/1386528437/us_test_midi_main.o: ../../../src/us_test_midi_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_midi_main.o ../../../src/us_test_midi_main.c

${OBJECTDIR}/_ext/1386528437/us_test_osc_msg_main.o: ../../../src/us_test_osc_msg_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_osc_msg_main.o ../../../src/us_test_osc_msg_main.c

${OBJECTDIR}/_ext/1386528437/us_trie_dump.o: ../../../src/us_trie_dump.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_trie_dump.o ../../../src/us_trie_dump.c

${OBJECTDIR}/_ext/1386528437/us_http.o: ../../../src/us_http.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_http.o ../../../src/us_http.c

${OBJECTDIR}/_ext/1386528437/us_test_slip_main.o: ../../../src/us_test_slip_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_slip_main.o ../../../src/us_test_slip_main.c

${OBJECTDIR}/_ext/1386528437/us_buffer.o: ../../../src/us_buffer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_buffer.o ../../../src/us_buffer.c

${OBJECTDIR}/_ext/1386528437/us_world.o: ../../../src/us_world.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_world.o ../../../src/us_world.c

${OBJECTDIR}/_ext/1386528437/us_logger_syslog.o: ../../../src/us_logger_syslog.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_logger_syslog.o ../../../src/us_logger_syslog.c

${OBJECTDIR}/_ext/1386528437/us_reactor.o: ../../../src/us_reactor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_reactor.o ../../../src/us_reactor.c

${OBJECTDIR}/_ext/1386528437/us_parse.o: ../../../src/us_parse.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_parse.o ../../../src/us_parse.c

${OBJECTDIR}/_ext/1285622248/us_example_reactor.o: ../../../examples/us_example_reactor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1285622248
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1285622248/us_example_reactor.o ../../../examples/us_example_reactor.c

${OBJECTDIR}/_ext/1285622248/us_example_hex_udp_rx.o: ../../../examples/us_example_hex_udp_rx.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1285622248
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1285622248/us_example_hex_udp_rx.o ../../../examples/us_example_hex_udp_rx.c

${OBJECTDIR}/_ext/1386528437/us_osc_msg.o: ../../../src/us_osc_msg.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_osc_msg.o ../../../src/us_osc_msg.c

${OBJECTDIR}/_ext/1285622248/us_example_mudp_tx.o: ../../../examples/us_example_mudp_tx.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1285622248
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1285622248/us_example_mudp_tx.o ../../../examples/us_example_mudp_tx.c

${OBJECTDIR}/_ext/1386528437/us_trie_rw.o: ../../../src/us_trie_rw.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_trie_rw.o ../../../src/us_trie_rw.c

${OBJECTDIR}/_ext/1386528437/us_test_allocator_main.o: ../../../src/us_test_allocator_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_allocator_main.o ../../../src/us_test_allocator_main.c

${OBJECTDIR}/_ext/1386528437/us_test_http_main.o: ../../../src/us_test_http_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_http_main.o ../../../src/us_test_http_main.c

${OBJECTDIR}/_ext/1386528437/us_json.o: ../../../src/us_json.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_json.o ../../../src/us_json.c

${OBJECTDIR}/_ext/1386528437/us_test_line_parse_main.o: ../../../src/us_test_line_parse_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_line_parse_main.o ../../../src/us_test_line_parse_main.c

${OBJECTDIR}/_ext/1386528437/us_test_buffer_main.o: ../../../src/us_test_buffer_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_buffer_main.o ../../../src/us_test_buffer_main.c

${OBJECTDIR}/_ext/1386528437/us_line_parse.o: ../../../src/us_line_parse.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_line_parse.o ../../../src/us_line_parse.c

${OBJECTDIR}/_ext/1386528437/us_trie_ro.o: ../../../src/us_trie_ro.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_trie_ro.o ../../../src/us_trie_ro.c

${OBJECTDIR}/_ext/1386528437/us_allocator_print.o: ../../../src/us_allocator_print.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_allocator_print.o ../../../src/us_allocator_print.c

${OBJECTDIR}/_ext/1285622248/us_example_mudp_rx.o: ../../../examples/us_example_mudp_rx.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1285622248
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1285622248/us_example_mudp_rx.o ../../../examples/us_example_mudp_rx.c

${OBJECTDIR}/_ext/1285622248/us_example_json.o: ../../../examples/us_example_json.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1285622248
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1285622248/us_example_json.o ../../../examples/us_example_json.c

${OBJECTDIR}/_ext/1386528437/us_print.o: ../../../src/us_print.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_print.o ../../../src/us_print.c

${OBJECTDIR}/_ext/1285622248/us_example_hex_udp_tx.o: ../../../examples/us_example_hex_udp_tx.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1285622248
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1285622248/us_example_hex_udp_tx.o ../../../examples/us_example_hex_udp_tx.c

${OBJECTDIR}/_ext/1386528437/us_test_trie_main.o: ../../../src/us_test_trie_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_trie_main.o ../../../src/us_test_trie_main.c

${OBJECTDIR}/_ext/1386528437/us_slip.o: ../../../src/us_slip.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_slip.o ../../../src/us_slip.c

${OBJECTDIR}/_ext/1386528437/us_logger.o: ../../../src/us_logger.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_logger.o ../../../src/us_logger.c

${OBJECTDIR}/_ext/1386528437/us_midi.o: ../../../src/us_midi.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_midi.o ../../../src/us_midi.c

${OBJECTDIR}/_ext/1285622248/us_example_httpd.o: ../../../examples/us_example_httpd.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1285622248
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1285622248/us_example_httpd.o ../../../examples/us_example_httpd.c

${OBJECTDIR}/_ext/1386528437/us_serial.o: ../../../src/us_serial.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_serial.o ../../../src/us_serial.c

${OBJECTDIR}/_ext/1386528437/us_logger_udp.o: ../../../src/us_logger_udp.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_logger_udp.o ../../../src/us_logger_udp.c

${OBJECTDIR}/_ext/1386528437/us_osc_msg_print.o: ../../../src/us_osc_msg_print.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_osc_msg_print.o ../../../src/us_osc_msg_print.c

${OBJECTDIR}/_ext/1386528437/us_time.o: ../../../src/us_time.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_time.o ../../../src/us_time.c

${OBJECTDIR}/_ext/1386528437/us_testutil.o: ../../../src/us_testutil.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_testutil.o ../../../src/us_testutil.c

${OBJECTDIR}/_ext/1386528437/us_test_json_main.o: ../../../src/us_test_json_main.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_test_json_main.o ../../../src/us_test_json_main.c

${OBJECTDIR}/_ext/1386528437/us_allocator.o: ../../../src/us_allocator.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} $@.d
	$(COMPILE.c) -g -I../../../include -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1386528437/us_allocator.o ../../../src/us_allocator.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-MacOSX/libmicrosupport.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
