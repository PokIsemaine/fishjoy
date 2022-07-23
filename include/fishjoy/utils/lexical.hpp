//
// Created by zsl on 7/23/22.
//

#ifndef FISHJOY_LEXICAL_HPP
#define FISHJOY_LEXICAL_HPP

#include "boost/lexical_cast.hpp"
#include "yaml-cpp/yaml.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <list>

namespace fishjoy {
  /**
   * @brief 类型转换模板类(F 源类型, T 目标类型)
   */
  template<class F, class T>
  class LexicalCast {
   public:
    /**
     * @brief 类型转换
     * @param[in] v 源类型值
     * @return 返回v转换后的目标类型
     * @exception 当类型不可转换时抛出异常
     */
    T operator()(const F& v) { return boost::lexical_cast<T>(v); }
  };

  /**
    * @brief 类型转换模板类偏特化(YAML String 转换成 std::vector<T>)
   */
  template<class T>
  class LexicalCast<std::string, std::vector<T>> {
   public:
    std::vector<T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::vector<T> vec;
      std::stringstream ss;
      for (size_t i = 0; i < node.size(); ++i) {
        ss.str("");
        ss << node[i];
        vec.push_back(LexicalCast<std::string, T>()(ss.str()));
      }
      return vec;
    }
  };

  /**
    * @brief 类型转换模板类偏特化(std::vector<T> 转换成 YAML String)
   */
  template<class T>
  class LexicalCast<std::vector<T>, std::string> {
   public:
    std::string operator()(const std::vector<T>& v) {
      YAML::Node node;
      for (auto& i : v) {
        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };

  /**
    * @brief 类型转换模板类片特化(YAML String 转换成 std::list<T>)
   */
  template<class T>
  class LexicalCast<std::string, std::list<T>> {
   public:
    std::list<T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::list<T> vec;
      std::stringstream ss;
      for (size_t i = 0; i < node.size(); ++i) {
        ss.str("");
        ss << node[i];
        vec.push_back(LexicalCast<std::string, T>()(ss.str()));
      }
      return vec;
    }
  };


  /**
    * @brief 类型转换模板类片特化(std::list<T> 转换成 YAML String)
   */
  template<class T>
  class LexicalCast<std::list<T>, std::string> {
   public:
    std::string operator()(const std::list<T>& v) {
      YAML::Node node;
      for (auto& i : v) {
        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };

  /**
    * @brief 类型转换模板类片特化(YAML String 转换成 std::set<T>)
   */
  template<class T>
  class LexicalCast<std::string, std::set<T>> {
   public:
    std::set<T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::set<T> vec;
      std::stringstream ss;
      for (size_t i = 0; i < node.size(); ++i) {
        ss.str("");
        ss << node[i];
        vec.insert(LexicalCast<std::string, T>()(ss.str()));
      }
      return vec;
    }
  };

  /**
    * @brief 类型转换模板类片特化(std::set<T> 转换成 YAML String)
   */
  template<class T>
  class LexicalCast<std::set<T>, std::string> {
   public:
    std::string operator()(const std::set<T>& v) {
      YAML::Node node;
      for (auto& i : v) {
        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };

  /**
    * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_set<T>)
   */
  template<class T>
  class LexicalCast<std::string, std::unordered_set<T>> {
   public:
    std::unordered_set<T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::unordered_set<T> vec;
      std::stringstream ss;
      for (size_t i = 0; i < node.size(); ++i) {
        ss.str("");
        ss << node[i];
        vec.insert(LexicalCast<std::string, T>()(ss.str()));
      }
      return vec;
    }
  };

  /**
    * @brief 类型转换模板类片特化(std::unordered_set<T> 转换成 YAML String)
   */
  template<class T>
  class LexicalCast<std::unordered_set<T>, std::string> {
   public:
    std::string operator()(const std::unordered_set<T>& v) {
      YAML::Node node;
      for (auto& i : v) {
        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };

  /**
    * @brief 类型转换模板类片特化(YAML String 转换成 std::map<std::string, T>)
   */
  template<class T>
  class LexicalCast<std::string, std::map<std::string, T>> {
   public:
    std::map<std::string, T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::map<std::string, T> vec;
      std::stringstream ss;
      for (auto it = node.begin(); it != node.end(); ++it) {
        ss.str("");
        ss << it->second;
        vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
      }
      return vec;
    }
  };

  /**
    * @brief 类型转换模板类片特化(std::map<std::string, T> 转换成 YAML String)
   */
  template<class T>
  class LexicalCast<std::map<std::string, T>, std::string> {
   public:
    std::string operator()(const std::map<std::string, T>& v) {
      YAML::Node node;
      for (auto& i : v) {
        node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };

  /**
    * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_map<std::string, T>)
   */
  template<class T>
  class LexicalCast<std::string, std::unordered_map<std::string, T>>   {
   public:
    std::unordered_map<std::string, T> operator()(const std::string& v) {
      YAML::Node node = YAML::Load(v);
      typename std::unordered_map<std::string, T> vec;
      std::stringstream ss;
      for (auto it = node.begin(); it != node.end(); ++it) {
        ss.str("");
        ss << it->second;
        vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
      }
      return vec;
    }
  };

  /**
    * @brief 类型转换模板类片特化(std::unordered_map<std::string, T> 转换成 YAML String)
   */
  template<class T>
  class LexicalCast<std::unordered_map<std::string, T>, std::string> {
   public:
    std::string operator()(const std::unordered_map<std::string, T>& v) {
      YAML::Node node;
      for (auto& i : v) {
        node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
      }
      std::stringstream ss;
      ss << node;
      return ss.str();
    }
  };
}
#endif  // FISHJOY_LEXICAL_HPP
