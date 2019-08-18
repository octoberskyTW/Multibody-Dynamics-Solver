SRC = src
OUT = build
EXEC = main
INCLUDE = include
CXX = g++

CXXFLAGS = -I include -Wall -g
LDFLAGS = -larmadillo

OBJS = $(patsubst $(SRC)/%.cpp, %.o, $(wildcard $(SRC)/*.cpp))

OBJS := $(addprefix $(OUT)/,$(OBJS))
deps := $(OBJS:%.o=%.o.d)

$(OUT)/%.o: $(SRC)/%.cpp
	mkdir -p $(OUT)
	$(CXX) $(CXXFLAGS) -o $@ -MMD -MF $@.d -c $<

$(EXEC): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

.PHONY: run obj clean distclean

all: $(EXEC)

run: $(EXEC)
	./$(EXEC)

obj: $(OBJS)

clean:
	${RM} $(OBJS) $(EXEC) $(deps)

distclean: clean
	$(RM) -rf build
	$(RM) $(EXEC)

-include $(deps)