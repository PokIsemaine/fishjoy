//
// Created by zsl on 5/18/22.
//
#include "fishjoy/config.hpp"
#include "fishjoy/log.hpp"
#include "fishjoy/env.hpp"

namespace fishjoy {

  static fishjoy::Logger::ptr g_logger = FISHJOY_LOG_NAME("system");

  ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
    RWMutexType::ReadLock lock(GetMutex());
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
  }

  //"A.B", 10
  // A:
  //  B: 10
  //  C: str

  static void ListAllMember(
      const std::string& prefix,
      const YAML::Node& node,
      std::list<std::pair<std::string, const YAML::Node>>& output) {
    // TODO 消除硬编码 constexpr
    if (prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos) {
      FISHJOY_LOG_ERROR(FISHJOY_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
      return;
    }
    output.emplace_back(std::make_pair(prefix, node));
    if (node.IsMap()) {
      for (auto it = node.begin(); it != node.end(); ++it) {
        ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
      }
    }
  }

  void Config::LoadFromYaml(const YAML::Node& root) {
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);

    for (auto& i : all_nodes) {
      std::string key = i.first;
      if (key.empty()) {
        continue;
      }

      std::transform(key.begin(), key.end(), key.begin(), ::tolower);
      ConfigVarBase::ptr var = LookupBase(key);

      if (var) {
        if (i.second.IsScalar()) {
          var->fromString(i.second.Scalar());
        } else {
          std::stringstream ss;
          ss << i.second;
          var->fromString(ss.str());
        }
      }
    }
  }

  /// 记录每个文件的修改时间
  static std::map<std::string, uint64_t> s_file2modifytime;
  /// 是否强制加载配置文件，非强制加载的情况下，如果记录的文件修改时间未变化，则跳过该文件的加载
  static fishjoy::Mutex s_mutex;
  
  void Config::LoadFromDir(const std::string &path, bool force) {
    std::string absolute_path = fishjoy::EnvMgr::GetInstance()->getAbsolutePath(path);
    std::vector<std::string> files;
    FSUtil::ListAllFile(files, absolute_path, ".yml");

    for (auto &i : files) {
      {
        struct stat st;
        lstat(i.c_str(), &st);
        fishjoy::Mutex::Lock lock(s_mutex);
        if (!force && s_file2modifytime[i] == (uint64_t)st.st_mtime) {
          continue;
        }
        s_file2modifytime[i] = st.st_mtime;
      }
      try {
        YAML::Node root = YAML::LoadFile(i);
        LoadFromYaml(root);
        FISHJOY_LOG_INFO(g_logger) << "LoadConfFile file="
                                 << i << " ok";
      } catch (...) {
        FISHJOY_LOG_ERROR(g_logger) << "LoadConfFile file="
                                  << i << " failed";
      }
    }
  }
  
  /**
   * @brief 遍历所有配置，并执行传入的回调函数
   * @param callback 回调函数
   */
  void Config::Visit(std::function<void(ConfigVarBase::ptr)> callback) {
    RWMutexType::ReadLock lock(GetMutex());
    ConfigVarMap& m = GetDatas();
    for (auto it = m.begin(); it != m.end(); ++it) {
      callback(it->second);
    }
  }
}  // namespace fishjoy