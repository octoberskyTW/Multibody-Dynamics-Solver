SRC = src
OUT = build
EXEC = main
INCLUDE = include
CXX = g++

CXXFLAGS = -I include -Wall -g -std=c++14
LDFLAGS = -larmadillo

.PHONY: run obj clean distclean

all: $(EXEC)

OBJS = $(patsubst $(SRC)/%.cpp, %.o, $(wildcard $(SRC)/*.cpp))

OBJS := $(addprefix $(OUT)/,$(OBJS))
deps := $(OBJS:%.o=%.o.d)

$(OUT) :
	mkdir -p $(OUT)

$(OUT)/%.o: $(SRC)/%.cpp $(OUT)
	$(CXX) $(CXXFLAGS) -o $@ -MMD -MF $@.d -c $<

$(EXEC): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)


run: $(EXEC)
	./$(EXEC)

obj: $(OBJS)

clean:
	${RM} $(OBJS) $(EXEC) $(deps)

distclean: clean
	$(RM) -rf build
	$(RM) $(EXEC)
-include $(deps)
