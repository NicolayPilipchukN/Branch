APP_NAME ?= driver
APP_PATH_INSTALL = /usr/local/bin
APP_SRC = driver.cpp
APP_OBJ = $(APP_SRC:.cpp=.o)

LIB_PATH_INSTALL = /usr/lib
LIB_NAME = libbranch.so.1
LIB_NAME_FULL = libbranch.so.1.0.1
LIB_NAME_SHORT = libbranch.so
LIB_NAME_REF = lbranch
LIB_SRC = branch.cpp
LIB_HDR = branch.h
LIB_OBJ = $(LIB_SRC:.cpp=.o)

CXX ?= g++
CXXFLAGS ?= -lcurl -std=c++14
SOFLAGS = -$(LIB_NAME_REF)
LINKFLAGS = -Wl,-rpath,$(LIB_PATH_INSTALL)

.PHONY: all so clean install uninstall
.SILENT: all $(APP_NAME) $(APP_OBJ) so clean install uninstall

all: $(APP_NAME) install

$(APP_NAME): so $(APP_OBJ)
	$(CXX) $(LINKFLAGS) -o $(APP_NAME) $(APP_OBJ) $(SOFLAGS) $(CXXFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

so:
	$(CXX) -fpic -c $(LIB_SRC)
	$(CXX) -shared -Wl,-soname,$(LIB_NAME) -o $(LIB_NAME_FULL) $(LIB_OBJ)
	sudo install $(LIB_NAME_FULL) $(LIB_PATH_INSTALL)
	sudo ln -sf $(LIB_PATH_INSTALL)/$(LIB_NAME_FULL) $(LIB_PATH_INSTALL)/$(LIB_NAME_SHORT)
	sudo ldconfig -n $(LIB_PATH_INSTALL)

clean:
	rm -rf $(APP_NAME) $(APP_OBJ) $(LIB_OBJ) $(LIB_NAME_FULL)

install:
	sudo install $(APP_NAME) $(APP_PATH_INSTALL)

uninstall:
	sudo rm -rf $(APP_PATH_INSTALL)/$(APP_NAME)
	sudo rm -rf $(LIB_PATH_INSTALL)/$(LIB_NAME_SHORT)*
