#include "cspch.h"
#include "config.h"
#include "log.h"
#include <yaml-cpp/yaml.h>

const std::string xxx = CppServer::Config::OutputConfig();

CppServer::ConfigVar<int>::ptr g_int_value_config = CppServer::Config::Lookup("system.port", (int)8080, "system port");

CppServer::ConfigVar<float>::ptr g_float_value_config = CppServer::Config::Lookup("system.value", (float)10.2f, "system value");

void print_yaml(const YAML::Node& node, int level) {
    if(node.IsScalar()) {
        CS_LOG_INFO(CS_LOG_ROOT()) << std::string(level * 4, ' ')
            << node.Scalar() << " - " << node.Type() << " - " << level;
    } else if(node.IsNull()) {
        CS_LOG_INFO(CS_LOG_ROOT()) << std::string(level * 4, ' ')
            << "NULL - " << node.Type() << " - " << level;
    } else if(node.IsMap()) {
        for(auto it = node.begin();
                it != node.end(); ++it) {
            CS_LOG_INFO(CS_LOG_ROOT()) << std::string(level * 4, ' ')
                    << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); ++i) {
            CS_LOG_INFO(CS_LOG_ROOT()) << std::string(level * 4, ' ')
                << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml() {
    YAML::Node root = YAML::LoadFile("CppServer/conf/log.yml");
    print_yaml(root, 0);

    CS_LOG_INFO(CS_LOG_ROOT()) << root.Scalar();
}

void test_config() {
    CS_LOG_INFO(CS_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    CS_LOG_INFO(CS_LOG_ROOT()) << "before: " << g_float_value_config->toString();

    YAML::Node root = YAML::LoadFile("CppServer/conf/log.yml");
    CppServer::Config::LoadFromYaml(root);

    CS_LOG_INFO(CS_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    CS_LOG_INFO(CS_LOG_ROOT()) << "after: " << g_float_value_config->toString();
}

int main(int argc, char** argv) {
    // std::cout << xxx << std::endl;
    test_yaml();
    test_config();
    return 0;
}