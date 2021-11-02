CXX = mpic++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -pedantic -g


lib$(LIB_NAME).so: $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

NB_SERVER ?= 5
NB_CLIENT ?= 5
HOSTFILE ?= hostfile


run: $(BIN) gen_commands
	mpirun -np $$(($(NB_SERVER) + $(NB_CLIENT) + 1)) -hostfile $(HOSTFILE) $(BIN) $(NB_SERVER) $(NB_CLIENT)

$(BIN): $(OBJ)
	$(CXX) -o $@ $^

clean:
	$(RM) lib$(LIB_NAME).so $(LIB_OBJS)


.PHONY: clean

