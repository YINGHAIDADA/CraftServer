// #include "cspch.h"
#include <iostream>

// int main() {
//     std::cout << __FILE__ << std::endl;
//     return 0;
// }


// #include <yaml-cpp/yaml.h>
#include <string>
#include <map>
 
// using namespace std;
 
// int main()
// {
//     string filename = "CraftServer/src/config.yaml";
//     YAML::Node config = YAML::LoadFile(filename);
//     vector<string> illegal_rules = config["illegal_rules"].as<vector<string>>();
//     cout << illegal_rules.size() << endl;
//     for (const string s : illegal_rules) {
//         cout << s << endl;
//     }
//     return 0;
// }

class Config
{
    typedef std::map<std::string, int> SIMap;
private:
    static SIMap s_datas;
public:
    Config();

    static bool pop(const std::string& name, const int num){
        if (name == "" or num == 0)
            return false;
        s_datas[name] = num;
        return true;
    }
    static int find(const std::string& name)
    {
        return s_datas[name];
    }
};

Config::SIMap Config::s_datas;

int main()
{
    
    Config::pop("A",1);
    Config::pop("B",2);

    std::cout << Config::find("A") << std::endl;
}