#pragma once
#include "shared_pch.h"

namespace Lux::Infrastructure
{
  enum class dependency_lifetime
  {
    singleton,
    transient
  };

  class dependency_container
  {
  private:
    struct dependency_registration_base 
    {
      virtual ~dependency_registration_base() = default;
    };

    template<typename T>
    class dependency_registration : public dependency_registration_base
    {
    private:
      dependency_lifetime _lifetime;
      std::shared_ptr<T> _value;
      std::function<std::unique_ptr<T>()> _factory;
      std::mutex _mutex;

    public:
      dependency_registration(dependency_lifetime lifetime) :
        _lifetime(lifetime)
      { 
        if constexpr (std::is_default_constructible<T>::value)
        {
          _factory = [] { return std::make_unique<T>(); };
        }
        else
        {
          _factory = []() -> std::unique_ptr<T> { throw std::exception("Cannot construct abstract type."); };
        }
      }

      dependency_registration(dependency_lifetime lifetime, std::function<std::unique_ptr<T>()>&& factory) :
        _lifetime(lifetime),
        _factory{ std::move(factory) }
      { }

      std::shared_ptr<T> get()
      {
        switch (_lifetime)
        {
        case dependency_lifetime::singleton:
        {
          std::lock_guard<std::mutex> lock(_mutex);

          if (!_value)
          {
            _value = _factory();
          }

          return _value;
        }
        case dependency_lifetime::transient:
        {
          return _factory();
        }
        default:
          throw std::exception("Unknown dependency lifetime.");
        }
      }
    };

    std::unordered_map<std::type_index, std::unique_ptr<dependency_registration_base>> _registrations;

    std::mutex _mutex;

  public:
    template<typename T>
    std::shared_ptr<T> resolve()
    {
      std::type_index index{ typeid(T) };

      dependency_registration<T>* registration;
      {
        std::lock_guard<std::mutex> lock(_mutex);

        auto& item = _registrations[index];
        if (!item)
        {
          item = std::make_unique<dependency_registration<T>>(dependency_lifetime::singleton);
        }

        registration = static_cast<dependency_registration<T>*>(item.get());
      }

      return registration->get();
    }

    template<typename T>
    bool add(dependency_lifetime lifetime)
    {
      std::type_index index{ typeid(T) };

      std::lock_guard<std::mutex> lock(_mutex);
      return _registrations.emplace(index, std::make_unique<dependency_registration<T>>(lifetime)).second;
    }

    template<typename T>
    bool add(dependency_lifetime lifetime, std::function<std::unique_ptr<T>()>&& factory)
    {
      std::type_index index{ typeid(T) };

      std::lock_guard<std::mutex> lock(_mutex);
      return _registrations.emplace(index, std::make_unique<dependency_registration<T>>(lifetime, std::move(factory))).second;
    }
  };
}