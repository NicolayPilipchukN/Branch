# Compare Branch
## Install
* Clone the repository
* Enter the command in the downloaded folder "make run_all"
* Enter the password to access root
## Starting after installation
### The application can be launched in two ways:
* The first option: without parameters.<br>
The application interface will prompt you to enter the names of the branches.
![image](https://github.com/NicolayPilipchukN/Branch/assets/122270528/7577bf11-aee4-4039-85d4-4db85ddf0673)
* Second option: with two additional parameters.<br>
The parameters indicate the names of branches.
![image](https://github.com/NicolayPilipchukN/Branch/assets/122270528/7945ac58-5dd5-421c-821a-63337b2ffa0a)
## Libraries used:
* For a get request: curl
* For processing json data: nlohmann-json
## Description
### Makefile
#### The run_all command runs various scripts. 
The installation of the application takes place in several stages:<br>
The first stage is downloading the necessary libraries.<br>
The second stage is the creation of shared libraries.<br>
The third stage is the installation of the created shared library into the system.<br>
The fourth stage is the creation of a CLI application.<br>
The fifth stage is the first launch of the created application without external arguments.<br><br>
The list of all scenarios is below.
```Makefile
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
```
List of variables for scripts:
```Makefile
h_name = branch.h
cpp_name = branch.cpp
o_name = branch.o
driver_name = driver.cpp
app_name = driver
lib_name = libbranch.so.1
lib_full_name = libbranch.so.1.0.5
lib_short_name = libbranch.so
lib_ref_name = -lbranch
lib_install_path = /usr/lib
lib_include_path = /usr/local/include
```
There is an unused 'del_temp_files' command that deletes used files from the working directory
```Makefile
del_temp_files:
	@sudo rm $(driver_name) $(o_name) $(cpp_name) $(h_name) $(lib_full_name)
```
### driver.cpp
This file describes a list of actions:<br>
The first action is initialization and loading of data by the specified branch names.<br>
The second step is to compare the specified tasks and output the results to the corresponding json files.<br>
```C++
int main(int argc, char **argv)
{
    if (argc == 1 || argc == 3)
    {
        // Creating Branch instances
        unique_ptr<Branch> branch_first;
        unique_ptr<Branch> branch_second;

        // Filling in branch names depending on startup and initializing objects
        if (argc == 1)
        {
            branch_first = make_unique<Branch>();
            branch_second = make_unique<Branch>();
        }
        else
        {
            branch_first = make_unique<Branch>(argv[1]);
            branch_second = make_unique<Branch>(argv[2]);
        }

        // Loading data
        if (!branch_first->getDataFromUrl())
        {
            cout << "Exit app" << endl;
            return -1;
        }
        if (!branch_second->getDataFromUrl())
        {
            cout << "Exit app" << endl;
            return -1;
        }

        // Comparison of package names from the first branch with the second
        branch_first->comparePackagesName(NAME_FILE_COMPARISON_NAME_FIRST_SECOND,
                                          branch_second->getDataPackages(),
                                          branch_second->getBranchName());

        // Comparison of package names from the second branch with the first
        branch_second->comparePackagesName(NAME_FILE_COMPARISON_NAME_SECOND_FIRST,
                                           branch_first->getDataPackages(),
                                           branch_first->getBranchName());

        // Comparing versions of shared packages from the first branch with the second
        branch_first->compareSharedPackagesVersion(NAME_FILE_COMPARISON_VERSION_FIRST_SECOND,
                                                   branch_second->getDataPackages(),
                                                   branch_second->getBranchName());
    }
    else
    {
        cout << "Invalid launch app" << endl;
    }

    return 0;
}
```
### branch.h and branch.cpp
These files contain the logic of processing branches.
Functions available for external call:
```C++
public:
  Branch();
  Branch(std::string);
  ~Branch();

  string getBranchName();

  // Launching a get request
  bool getDataFromUrl();

  // Comparing the names of two branches
  void comparePackagesName(const string file_path, unordered_map<string, string> data_other, string branch_name_other);

  // Comparison of versions of common packages
  void compareSharedPackagesVersion(const string file_path, unordered_map<string, string> data_other, string branch_name_other);

  // Getting a list of packages and their versions
  unordered_map<string, string> getDataPackages();
```
Functions used privately:
```C++
private:
  // Function for determining the size of uploaded data by a get request
  static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

  // Getting REST API data
  json get();

  // Creating and filling in a JSON file
  void writeFile(const string file_path, unique_ptr<Data> data);

  bool fromJson(json branch_data);
  json toJson(unique_ptr<Data> answer);
```
Class fields and Data structure:
```C++
//class Branch
private:
        string branch_name;
        string branch_url;
        shared_ptr<Data> data;
        
//struct Data
  unsigned long length;
  // Key: name_package, value: version package
  unordered_map<string, string> packages;

  Data();
  // Add package in map
  void push(string name, string version);
```
