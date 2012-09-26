CXX=g++
#CPPFLAGS=-Wall -g -O2 
CPPFLAGS=-Wall -g -D_DEBUG 
LDFLAGS= -Lhttp-parser -LSimpleNet
LIBS= -lsimple_net -lhttp_parser
OBJECTS=http_sniff.o http_connect_session.o http_sniff_session.o
DEPEND=SimpleNet/libsimple_net.a http-parser/libhttp_parser.a
TARGET=http_sniff

all:simplenet $(TARGET) 

# make the SimpleNet lib
simplenet:
	make -C SimpleNet

$(TARGET):$(OBJECTS) $(DEPEND)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

# compile and generate dependency info;
# more complicated dependency computation, so all prereqs listed
# will also become command-less, prereq-less targets
#   sed:    strip the target (everything before colon)
#   sed:    remove any continuation backslashes
#   fmt -1: list words one per line
#   sed:    strip leading spaces
#   sed:    add trailing colons
$(OBJECTS):%.o:%.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@
	$(CXX) -MM $(CPPFLAGS) $*.cpp > $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

clean:
	-rm -f *.o *.d $(TARGET)
