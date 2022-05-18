//
// Created by zsl on 5/18/22.
//

#ifndef FISHJOY_CONFIG_HPP
#define FISHJOY_CONFIG_HPP

#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include "log.hpp"


namespace fishjoy {
  class ConfigVarBase {
   public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description)
        :m_name(name)
        ,m_description(description) {
      std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    ~ConfigVarBase() = default;

    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;
   private:
    std::string m_name;
    std::string m_description;
  };

  template<typename T>
  class ConfigVar : public ConfigVarBase {
   public:
    typedef std::shared_ptr<ConfigVar> ptr;
    ConfigVar(const std::string& name,
              const std::string& description = "",
              const T& default_value)
        : ConfigVarBase(name,description),m_val(default_value) {

    }

    std::string toString() override {
      try {
        return boost::lexical_cast<std::string>(m_val);
      } catch (std::exception& e) {

      }
    }
   private:
    T m_val;
  };
}

#endif  // FISHJOY_CONFIG_HPP
