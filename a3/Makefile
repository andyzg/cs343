SENTINEL:=-1
KIND:=NOBUSY
OPT:= -multi # -multi -O2

CXX = u++					# compiler
CXXFLAGS = -g -Wall -Wno-unused-label ${OPT} -MMD -std=c++11 \
		-DSENTINEL=${SENTINEL} -D${KIND}# compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS01 = q1staticexits.o			# optional build of given program
EXEC01 = staticexits				# 0th executable name

OBJECTS1 = q1nostaticexits.o
EXEC1 = nostaticexits				# 1st executable name

OBJECTS2 = q2matrixmultiply.o # object files forming 2nd executable with prefix "q2"
EXEC2 = matrixmultiply				# 2nd executable name

OBJECTS3 = # object files forming 3rd executable with prefix "q3"
EXEC3 = buffer									# 3rd executable name

OBJECTS = ${OBJECTS1} ${OBJECTS2} ${OBJECTS3}	# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2} ${EXEC3}		# all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables

q1.o : q1.cc					# change compiler 1st executable, ADJUST SUFFIX (.cc)
	g++-4.9 ${CXXFLAGS} -c $< -o $@

q1%.o : q1%.cc					# change compiler 1st executable, ADJUST SUFFIX (.cc)
	g++-4.9 ${CXXFLAGS} -c $< -o $@

${EXEC01} : ${OBJECTS01}
	g++-4.9 ${CXXFLAGS} $^ -o $@

${EXEC1} : ${OBJECTS1}
	g++-4.9 ${CXXFLAGS} $^ -o $@

${EXEC2} : ${OBJECTS2}				# link step
	${CXX} ${CXXFLAGS} $^ -o $@

-include ImplKind

ifeq (${IMPLKIND},${KIND})			# same implementation type as last time ?
${EXEC3} : ${OBJECTS3}
	${CXX} ${CXXFLAGS} $^ -o $@
else
ifeq (${KIND},)					# no implementation type specified ?
# set type to previous type
KIND=${IMPLKIND}
${EXEC3} : ${OBJECTS3}
	${CXX} ${CXXFLAGS} $^ -o $@
else						# implementation type has changed
.PHONY : ${EXEC3}
${EXEC3} :
	rm -f ImplKind
	touch q3${EXEC3}.h
	sleep 1
	${MAKE} ${EXEC3} KIND="${KIND}"
endif
endif

ImplKind :
	echo "IMPLKIND=${KIND}" > ImplKind
	sleep 1

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} ImplKind
