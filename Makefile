h_name = branch.h
cpp_name = branch.cpp
o_name = branch.o
driver_name = driver.cpp
app_name = driver
lib_name = libbranch.so.1
lib_full_name = libbranch.so.1.0.6
lib_short_name = libbranch.so
lib_ref_name = -lbranch
lib_install_path = /usr/lib
lib_include_path = /usr/local/include

run_all:
	@make -s download_libraries
	@make -s create_shared_library
	@make -s install_shared_library
	@make -s create_driver
	@make -s launch_driver

download_libraries:
	@sudo apt-get install gcc
	@sudo apt-get install gcc-c++
	@sudo apt-get install libcurl-devel
	@sudo apt-get install nlohmann-json-devel

create_shared_library:
	@g++ -fpic -c $(cpp_name)
	@g++ -shared -Wl,-soname,$(lib_name) -o $(lib_full_name) $(o_name)


install_shared_library:
	@sudo install -m 0644 $(lib_full_name) $(lib_install_path)
	@sudo ln -sf $(lib_install_path)/$(lib_full_name) $(lib_install_path)/$(lib_short_name)
	@sudo cp $(h_name) $(lib_include_path)
	@sudo ldconfig -n $(lib_install_path)
	@echo 'export LD_LIBRARY_PATH='$(lib_install_path)':$$LD_LIBRARY_PATH' >> /home/$(USER)/.bashrc
	@export LD_LIBRARY_PATH=$(lib_install_path):$LD_LIBRARY_PATH

create_driver:
	@g++ $(driver_name) $(lib_ref_name) -L$(lib_install_path) -o $(app_name) -lcurl -std=c++14

del_temp_files:
	@sudo rm $(driver_name) $(o_name) $(cpp_name) $(h_name) $(lib_full_name)

launch_driver:
	@./$(app_name)

.PHONY: install


