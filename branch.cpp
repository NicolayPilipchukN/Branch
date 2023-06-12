#include "branch.h"

using namespace branch_info;

static const std::string API = R"(https://rdb.altlinux.org/api/export/branch_binary_packages/)";

static const string REQUEST_ARCH = "?arch=";

Data::Data()
{
    this->length = 0;
}

void Data::push(string name, string version)
{
    this->length += 1;
    this->packages[name] = version;
}

Branch::Branch(string branch_name, string branch_arch)
{
    this->branch_name = branch_name;
    if (branch_name == "")
    {
        cout << "\nEnter branch (example: p10): ";
        cin >> this->branch_name;
    }

    this->branch_arch = branch_arch;
    if (branch_arch == "")
    {
        cout << "\nEnter \'all\' to get all architectures for branch " << this->branch_name << "\nEnter arch (example: aarch64): ";
        cin >> this->branch_arch;
    }

    this->branch_url = API + this->branch_name;

    if (this->branch_arch != "all")
    {
        this->branch_url += REQUEST_ARCH + this->branch_arch;
    }

    this->data = make_shared<Data>();
}

string Branch::getBranchName()
{
    return this->branch_name;
}

bool Branch::getDataFromUrl()
{
    cout << endl
         << "Load branch " << this->branch_name << endl;
    json temp = get();

    if (temp != NULL)
    {
        return fromJson(temp);
    }

    return false;
}

size_t Branch::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

json Branch::get()
{
    CURL *curl;
    CURLcode http_code;
    string buffer = "";
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, this->branch_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);
        if (http_code == 200)
        {
            cout << "Load successful" << endl;
            return json::parse(buffer);
        }
        cout << "Error: Data loading failed" << endl;
    }
    return NULL;
}

void Branch::comparePackagesName(json& result,
                                 const string key_name,
                                 const string branch_name_other,
                                 const unordered_map<string, string> &data_other)
{
    cout << "\nCompare name packages " << this->branch_name << " with " << branch_name_other << endl;
    unique_ptr<Data> res = make_unique<Data>();

    for (auto elem : this->data->packages)
    {
        if (data_other.find(elem.first) == data_other.end())
        {
            res->push(elem.first, elem.second);
        }
    }

    result[key_name] = toJson(move(res));

    cout << "Compare successful" << endl;
}

void Branch::compareSharedPackagesVersion(json& result,
                                          const string key_name,
                                          const string branch_name_other,
                                          const unordered_map<string, string> &data_other)
{
    cout << "\nCompare version shared packages " << this->branch_name << " with " << branch_name_other << endl;

    unique_ptr<Data> res = make_unique<Data>();

    for (auto elem : this->data->packages)
    {
        if (data_other.find(elem.first) != data_other.end())
        {
            if (strverscmp(elem.second.c_str(), data_other.find(elem.first)->second.c_str()) > 0)
            {
                res->push(elem.first, elem.second);
            }
        }
    }

    result[key_name] = toJson(move(res));

    cout << "Compare successful" << endl;
}

bool Branch::fromJson(json branch_data)
{
    this->data->length = branch_data["length"];

    for (auto elem : branch_data["packages"])
    {
        data->packages[elem["name"]] = elem["version"];
    }

    return true;
}

json Branch::toJson(unique_ptr<Data> answer)
{
    json res = {{"length", 0},
                {"packages", {{{"name", ""}, {"version", ""}}}}};

    res["length"] = answer->length;

    auto it = answer->packages.begin();
    for (size_t i = 0, length = res["length"]; i < length; i++, it++)
    {
        res["packages"][i]["name"] = it->first;
        res["packages"][i]["version"] = it->second;
    }

    return res;
}

unordered_map<string, string> Branch::getDataPackages()
{
    return this->data->packages;
}

Branch::~Branch()
{
}

void branch_info::writeData(const string file_path, json& data)
{
    ofstream file(file_path, ios::out);

    if (file.is_open())
    {
        file << data << endl;
        file.close();
        return;
    }

    cout << "Json file doesn't open or create!" << endl;
}