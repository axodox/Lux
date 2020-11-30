#pragma once
#include "shared_pch.h"

namespace Lux::Serialization
{
  typedef uint16_t type_id_t;

  template <typename TBase, typename... TArgs>
  struct type_registration_base
  {
    virtual std::unique_ptr<TBase> create_unique_instance(TArgs... args) const = 0;

    virtual std::shared_ptr<TBase> create_shared_instance(TArgs... args) const = 0;
  };

  template <typename TActual, typename TBase, typename... TArgs>
  struct type_registration : public type_registration_base<TBase, TArgs...>
  {
    virtual std::unique_ptr<TBase> create_unique_instance(TArgs... args) const override
    {
      return std::make_unique<TActual>(std::forward<TArgs>(args)...);
    }

    virtual std::shared_ptr<TBase> create_shared_instance(TArgs... args) const override
    {
      return std::make_shared<TActual>(std::forward<TArgs>(args)...);
    }
  };

  template <typename TBase, typename... TArgs>
  struct type_registry
  {
  private:
    std::unordered_map<type_id_t, std::shared_ptr<type_registration_base<TBase, TArgs...>>> _registrations;
    std::unordered_map<size_t, type_id_t> _typeMapping;

  public:
    template<typename TActual>
    void add(type_id_t id)
    {
      _typeMapping.emplace(typeid(TActual).hash_code(), id);
      _registrations.emplace(id, std::make_shared<type_registration<TActual, TBase, TArgs...>>());
    }

    template<typename TActual>
    void add()
    {
      add<TActual>((type_id_t)_typeMapping.size());
    }

    template<typename TActual>
    std::unique_ptr<TBase> create_unique(TArgs... args) const
    {
      return create_unique(get_id<TActual>(), std::forward<TArgs>(args)...);
    }

    std::unique_ptr<TBase> create_unique(type_id_t id, TArgs... args) const
    {
      auto& resolver = _registrations.at(id);
      return resolver->create_unique_instance(std::forward<TArgs>(args)...);
    }

    template<typename TActual>
    std::shared_ptr<TBase> create_shared(TArgs... args) const
    {
      return create_shared(get_id<TActual>(), std::forward<TArgs>(args)...);
    }

    std::shared_ptr<TBase> create_shared(type_id_t id, TArgs... args) const
    {
      auto& resolver = _registrations.at(id);
      return resolver->create_shared_instance(std::forward<TArgs>(args)...);
    }

    template<typename TActual>
    type_id_t get_id() const
    {
      return get_id(typeid(TActual));
    }

    type_id_t get_id(const type_info& type) const
    {
      auto mapping = _typeMapping.find(type.hash_code());
      if (mapping == _typeMapping.end()) throw std::exception("Attempting to instatiate unregistered type!");

      return mapping->second;
    }

    template<typename... TTypes>
    static type_registry make(std::function<type_id_t(const TBase&)> idResolver)
    {
      type_registry registry;
      build_from_types<TTypes...>(registry, idResolver);
      return registry;
    }

    template<typename... TTypes>
    static type_registry make()
    {
      type_registry registry;
      build_from_types<TTypes...>(registry);
      return registry;
    }

  private:
    template<typename TFirst, typename... TRest>
    static void build_from_types(type_registry& registry, std::function<type_id_t(const TBase&)> idResolver)
    {
      registry.add<TFirst>(idResolver(TFirst()));
      if constexpr (sizeof...(TRest) > 0) build_from_types<TRest...>(registry, idResolver);
    }

    template<typename TFirst, typename... TRest>
    static void build_from_types(type_registry& registry)
    {
      registry.add<TFirst>();
      if constexpr (sizeof...(TRest) > 0) build_from_types<TRest...>(registry);
    }
  };

  template<typename TBase, typename... TDerived>
  type_registry<TBase> make_sequential_type_registry()
  {
    return type_registry<TBase>::template make<TDerived...>();
  }

  template<typename TBase, typename... TDerived>
  type_registry<TBase> make_type_registry()
  {
    return type_registry<TBase>::template make<TDerived...>([](const TBase& value) -> type_id_t { return (type_id_t)value.type(); });
  }
}
