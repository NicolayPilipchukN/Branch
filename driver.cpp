#include "branch.h"

enum Args
{
    NO_ARGS = 1,
    NAME_BRANCH = 3,
    NAME_BRANCH_ARCH = 5
};

using namespace branch_info;

static const string NAME_FILE_COMPARISON_NAME = "comparison_package_name_";

static const string NAME_FILE_COMPARISON_VERSION = "comparison_package_version_";

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