COMPILER = g++
CFLAGS   = -g -O0 -std=c++11 -Wextra -g -MMD -MP
LDFLAGS  = -static-libgcc -static-libstdc++ -L../../libraries/boost_1_56_0/stage/lib
LIBS     = 
INCLUDE  = -I../../libraries/boost_1_56_0
TARGET   = tetris
OBJDIR   = ./obj
ifeq "$(strip $(OBJDIR))" ""
	OBJDIR = .
endif
SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))
DEPENDS  = $(OBJECTS:.o=.d)

# めんどいがこうするしかないらしい
$(TARGET).exe: $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	rm -f $(OBJECTS) $(DEPENDS) $(TARGET).exe
	#@rmdir --ignore-fail-on-non-empty `readlink -f $(OBJDIR)`

-include $(DEPENDS)
