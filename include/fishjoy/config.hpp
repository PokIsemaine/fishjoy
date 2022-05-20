//
// Created by zsl on 5/18/22.
//

#ifndef FISHJOY_CONFIG_HPP
#define FISHJOY_CONFIG_HPP

#include "yaml-cpp/yaml.h"
#include <boost/lexical_cast.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include <unordered_map>
#include <unordered_set>


#include "fishjoy/log.hpp"

namespace fishjoy
{
  class ConfigVarBase
  {
   public:
    using ptr = std::shared_ptr<ConfigVarBase>;
    ConfigVarBase(const std::string& name, const std::string& description) : m_name(name), m_description(description)
    {
      std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    ~ConfigVarBase() = default;

    const std::string& getName() const
    {
      return m_name;
    }
    const std::string& getDescription() const
    {
      return m_description;
    }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;

    virtual std::string getTypeName() const = 0;
   private:
    std::string m_name;
    std::string m_description;
  };

  template<typename T>
  class ConfigVar : public ConfigVarBase
  {
   public:
    using ptr = std::shared_ptr<ConfigVar>;
    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
        : ConfigVarBase(name, description),
          m_val(default_value)
    {
    }

    std::string toString() override
    {
      try
      {
        return boost::lexical_cast<std::string>(m_val);
      }
      catch (std::exception& e)
      {
        FISHJOY_LOG_ERROR(FISHJOY_LOG_ROOT())
            << "ConfigVar::toString exception" << e.what() << " convert: " << typeid(m_val).name() << " to string ";
      }
      return "";
    }

    bool fromString(const std::string& val) override
    {
      try
      {
        m_val = boost::lexical_cast<T>(val);
      }
      catch (std::exception& e)
      {
        FISHJOY_LOG_ERROR(FISHJOY_LOG_ROOT())
            << "ConfigVar::fromString exception" << e.what() << " convert: string to " << typeid(m_val).name();
      }
      return false;
    }

    const T getValue() const
    {
      return m_val;
    }
    void setValue(const T& v)
    {
      m_val = v;
    }

   private:
    T m_val;
  };

  //F from_type, T to_type
  template<class F, class T>
  class LexicalCast {
   public:
    T operator()(const F& v) {
      return boost::lexical_cast<T>(v);
    }
  };

  template<class T>
  class LexicalCast<std::string, std::vector<T> > {
   public:
    std::vector<T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::vector<T> vec;
      std::stringstream ss;
      for(size_t i = 0; i < node.size(); ++i) {
        ss.str("");
        ss << node[i];
        vec.push_back(LexicalCast<std::string, T>()(ss.str()));
      }
      return vec;
    }
  };

  template<class T>
  class LexicalCast<std::vector<T>, std::string> {
   public:
    std::string operator()(const std::vector<T>& v) {
      YAML::Node node;
      for(auto& i : v) {
        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };

  template<class T>
  class LexicalCast<std::string, std::list<T> > {
   public:
    std::list<T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::list<T> vec;
      std::stringstream ss;
      for(size_t i = 0; i < node.size(); ++i) {
        ss.str("");
        ss << node[i];
        vec.push_back(LexicalCast<std::string, T>()(ss.str()));
      }
      return vec;
    }
  };

  template<class T>
  class LexicalCast<std::list<T>, std::string> {
   public:
    std::string operator()(const std::list<T>& v) {
      YAML::Node node;
      for(auto& i : v) {
        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };

  template<class T>
  class LexicalCast<std::string, std::set<T> > {
   public:
    std::set<T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::set<T> vec;
      std::stringstream ss;
      for(size_t i = 0; i < node.size(); ++i) {
        ss.str("");
        ss << node[i];
        vec.insert(LexicalCast<std::string, T>()(ss.str()));
      }
      return vec;
    }
  };

  template<class T>
  class LexicalCast<std::set<T>, std::string> {
   public:
    std::string operator()(const std::set<T>& v) {
      YAML::Node node;
      for(auto& i : v) {
        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };

  template<class T>
  class LexicalCast<std::string, std::unordered_set<T> > {
   public:
    std::unordered_set<T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::unordered_set<T> vec;
      std::stringstream ss;
      for(size_t i = 0; i < node.size(); ++i) {
        ss.str("");
        ss << node[i];
        vec.insert(LexicalCast<std::string, T>()(ss.str()));
      }
      return vec;
    }
  };

  template<class T>
  class LexicalCast<std::unordered_set<T>, std::string> {
   public:
    std::string operator()(const std::unordered_set<T>& v) {
      YAML::Node node;
      for(auto& i : v) {
        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };
  
  class Config
  {
   public:
    using ConfigVarMap = std::map<std::string, ConfigVarBase::ptr>;

    template<typename T>
    static typename ConfigVar<T>::ptr
    Lookup(const std::string& name, const T& default_value, const std::string& description = "")
    {
      auto tmp = Lookup<T>(name);
      if (tmp)
      {
        FISHJOY_LOG_INFO(FISHJOY_LOG_ROOT()) << "Lookup name=" << name << " exists";
        return tmp;
      }

      if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") != std::string::npos)
      {
        FISHJOY_LOG_ERROR(FISHJOY_LOG_ROOT()) << "Lookup name invalid" << name;
        throw std::invalid_argument(name);
      }

      typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
      s_datas[name] = v;
      return v;
    }

    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name)
    {
      auto it = s_datas.find(name);
      if (it == s_datas.end())
      {
        return nullptr;
      }
      return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }

    static void LoadFromYaml(const YAML::Node& root);
    static ConfigVarBase::ptr LookupBase(const std::string& name);

   private:
    static ConfigVarMap s_datas;
  };
}  // namespace fishjoy

#endif  // FISHJOY_CONFIG_HPP
