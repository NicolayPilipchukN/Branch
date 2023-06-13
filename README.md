# Compare Branch
## Install
* install curl: sudo apt-get install libcurl-devel
* install nlohmann-json: sudo apt-get install nlohmann-json-devel
* Clone the repository
* Enter the command in the downloaded folder "make"
* Enter the password to access root
## Starting after installation
Executing the make command adds a new program to the system.  Enter 'driver' to call it.<br>
The result of the application will be stored in "result.json".<br>
The key of the first task: "cmp_nm_b1_b2".<br>
The key of the second task: "cmp_nm_b2_b1".<br>
The key of the third task: "cmp_v_b1_b2".<br>
### The application can be launched in three ways:
* The first option: without parameters.<br>
The application interface will prompt you to enter the names of branches and their architecture.<br>
![image](https://github.com/NicolayPilipchukN/Branch/assets/122270528/af6de946-76fb-484a-a9b6-e18c254b62a2)
* Second option: with two additional parameters.<br>
The names of the branches are specified in the parameters. You will be asked to enter their architectures.<br>
![image](https://github.com/NicolayPilipchukN/Branch/assets/122270528/8a4c2262-afcd-4c12-b8cd-03e5deff3281)
* Third option: specify four additional parameters.<br>
The first parameter is branch_1, the second parameter is branch_2, the third parameter is arch_1, the fourth parameter is arch_2.<br>
![image](https://github.com/NicolayPilipchukN/Branch/assets/122270528/817708b6-4ccf-460e-accf-ef59c026826a)
## Libraries used:
* For a get request: curl
* For processing json data: nlohmann-json
## Description
### Makefile
The list of all scenarios is below.
```Makefile
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
```
List of variables for scripts:
```Makefile
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
```
### driver.cpp
This file describes a list of actions:<br>
The first action is initialization and loading of data by the specified branch names.<br>
The second step is to compare the specified tasks and output the results to the corresponding json files.<br>
```C++
int main(int argc, char **argv)
{
    unique_ptr<Branch> branch_first;
    unique_ptr<Branch> branch_second;

    switch (argc)
    {
    case NO_ARGS:
        branch_first = make_unique<Branch>("", "");
        branch_second = make_unique<Branch>("", "");
        break;
    case NAME_BRANCH:
        branch_first = make_unique<Branch>(argv[1], "");
        branch_second = make_unique<Branch>(argv[2], "");
        break;
    case NAME_BRANCH_ARCH:
        branch_first = make_unique<Branch>(argv[1], argv[3]);
        branch_second = make_unique<Branch>(argv[2], argv[4]);
        break;
    default:
        cout << "Invalid launch app" << endl;
        return -1;
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

    string file_name = "result.json";

    json result;

    // Comparison of package names from the first branch with the second
    branch_first->comparePackagesName(result, "cmp_nm_b1_b2", branch_second->getBranchName(), branch_second->getDataPackages());

    // Comparison of package names from the second branch with the first
    branch_second->comparePackagesName(result, "cmp_nm_b2_b1", branch_first->getBranchName(), branch_first->getDataPackages());

    // Comparing versions of shared packages from the first branch with the second
    branch_first->compareSharedPackagesVersion(result, "cmp_v_b1_b2", branch_second->getBranchName(), branch_second->getDataPackages());

    writeData(file_name, result);

    return 0;
}
```
### branch.h and branch.cpp
These files contain the logic of processing branches.
Functions available for external call:
```C++
public:
    Branch(string, string);
    ~Branch();

    string getBranchName();

    // Launching a get request
    bool getDataFromUrl();

    // Comparing the names of two branches
    void comparePackagesName(json&, const string, const string, const unordered_map<string, string> &);

    // Comparison of versions of common packages
    void compareSharedPackagesVersion(json&, const string, const string, const unordered_map<string, string> &);

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

    bool fromJson(json branch_data);
    json toJson(unique_ptr<Data> answer);
```
Class fields and Data structure:
```C++
//class Branch
private:
    string branch_arch;
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
