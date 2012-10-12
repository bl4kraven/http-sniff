CXX=g++
#CPPFLAGS=-Wall -g -O2 
CPPFLAGS=-Wall -g -D_DEBUG -I.
LDFLAGS= -Lhttp-parser -LSimpleNet
# source code root dir
SOURCE_DIR=src/
FILES=$(shell find $(SOURCE_DIR) -name "*.cpp")
HEADERS=$(shell find $(SOURCE_DIR) -name "*.h")
LIBS= -lsimple_net -lhttp_parser
DEPEND=SimpleNet/libsimple_net.a http-parser/libhttp_parser.a
OBJECTS=$(FILES:.cpp=.o)
TARGET=http_sniff

all:simplenet $(TARGET) 

# make the SimpleNet lib
simplenet:
	make -C SimpleNet

# include all source file depends.
-include Makefile.deps

$(TARGET): Makefile.deps $(OBJECTS) $(DEPEND)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

Makefile.deps: $(FILES) $(HEADERS)
	./makedepend -f "$(FILES)" -c "$(CXX)" -p "$(CPPFLAGS)" > Makefile.deps

clean:
	-rm -f $(OBJECTS) $(TARGET) Makefile.deps
