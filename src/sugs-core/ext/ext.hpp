/* Copyright 2011 Jeffery Olson <olson.jeffery@gmail.com>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list
 *     of conditions and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Jeffery Olson <olson.jeffery@gmail.com>.
 *
 */

#ifndef __sugs_core_ext_hpp__
#define __sugs_core_ext_hpp__

#include <map>
#include <jsapi.h>
#include "../common.hpp"
#include "../corejs/corejs.h"
#include "../fs/fs.hpp"

namespace sugs
{
namespace core
{
namespace ext
{

class Component
{
public:
  Component() {}
  virtual void registerNativeFunctions(jsEnv jsEnv, sugsConfig config);
  virtual bool doWork(jsEnv jsEnv, sugsConfig config);
};

class ComponentFactory
{
public:
  virtual Component* create(jsEnv jsEnv, JSObject* configObj);
  virtual std::string getName();
};

class ComponentLibrary
{
public:
  static void registerComponentFactory(ComponentFactory* factory);
  static ComponentFactory* getComponentFactory(std::string componentName);

private:
  static std::map<std::string, ComponentFactory*> _library;
};

class ComponentPair
{
public:
  ComponentPair(ComponentFactory* factory, std::string configJson)
  {
    this->_factory = factory;
    this->_configJson = configJson;
  }

  std::string getConfigJson();
  ComponentFactory* getComponentFactory();

private:
  std::string _configJson;
  ComponentFactory* _factory;
};

class ScriptRunnerComponent : public Component
{
public:
  ScriptRunnerComponent(std::string entryPointScript, JSObject* data)
  {
    this->_lastMs = getCurrentMilliseconds();
    this->_entryPoint = entryPointScript;
    this->_data = data;
  }

  virtual void registerNativeFunctions(jsEnv jsEnv, sugsConfig config);
  virtual bool doWork(jsEnv jsEnv, sugsConfig config);

private:
  JSObject* _data;
  time_t _lastMs;
  std::string _entryPoint;

  void loadEntryPointScript(jsEnv jsEnv, pathStrings paths, const char* path);
};

class ScriptRunnerComponentFactory : public ComponentFactory
{
  public:
    virtual Component* create(jsEnv jsEnv, JSObject* configObj);
    virtual std::string getName();
};

class FilesystemComponent : public Component
{
public:
  FilesystemComponent() {}

  virtual void registerNativeFunctions(jsEnv jsEnv, sugsConfig config);

private:
};

class FilesystemComponentFactory : public ComponentFactory
{
  public:
    virtual Component* create(jsEnv jsEnv, JSObject* configObj);
    virtual std::string getName();
};

}
}
} // namespace sugs::core::ext

#endif
