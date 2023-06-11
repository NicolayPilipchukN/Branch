#include "branch.h"

enum CountArgs
{
    ONE = 1,
    THREE = 3,
    FIVE = 5
};

using namespace branch_info;

static const string NAME_FILE_COMPARISON_NAME = "comparison_package_name_";

static const string NAME_FILE_COMPARISON_VERSION = "comparison_package_version_";

int main(int argc, char **argv)
{
    if (argc != ONE && argc != THREE && argc != FIVE)
    {
        cout << "Invalid launch app" << endl;
        return -1;
    }

    unique_ptr<Branch> branch_first;
    unique_ptr<Branch> branch_second;

    switch (argc)
    {
    case ONE:
        branch_first = make_unique<Branch>();
        branch_second = make_unique<Branch>();
        break;
    case THREE:
        branch_first = make_unique<Branch>(argv[1]);
        branch_second = make_unique<Branch>(argv[2]);
        break;
    case FIVE:
        branch_first = make_unique<Branch>(argv[1], argv[3]);
        branch_second = make_unique<Branch>(argv[2], argv[4]);
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

    string file_name_first = NAME_FILE_COMPARISON_NAME +
                             branch_first->getBranchName() +
                             "_with_" +
                             branch_second->getBranchName() +
                             ".json";

    string file_name_second = NAME_FILE_COMPARISON_NAME +
                              branch_second->getBranchName() +
                              "_with_" +
                              branch_first->getBranchName() +
                              ".json";

    string file_name_third = NAME_FILE_COMPARISON_VERSION +
                             branch_second->getBranchName() +
                             "_with_" +
                             branch_first->getBranchName() +
                             ".json";

    // Comparison of package names from the first branch with the second
    branch_first->comparePackagesName(file_name_first, branch_second->getDataPackages(), branch_second->getBranchName());

    // Comparison of package names from the second branch with the first
    branch_second->comparePackagesName(file_name_second, branch_first->getDataPackages(), branch_first->getBranchName());

    // Comparing versions of shared packages from the first branch with the second
    branch_first->compareSharedPackagesVersion(file_name_third, branch_second->getDataPackages(), branch_second->getBranchName());

    return 0;
}