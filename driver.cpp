#include "branch.h"

using namespace branch_info;

const static string NAME_FILE_COMPARISON_NAME_FIRST_SECOND = "comparison_package_name_first_with_second.json";

const static string NAME_FILE_COMPARISON_NAME_SECOND_FIRST = "comparison_package_name_second_with_first.json";

const static string NAME_FILE_COMPARISON_VERSION_FIRST_SECOND = "comparison_package_version_frist_with_second.json";

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