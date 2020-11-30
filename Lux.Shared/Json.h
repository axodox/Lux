#pragma once
#include "Traits.h"
#include "EnumNameMapping.h"

namespace Lux::Json
{
  class json_value
  {
  private:
    winrt::Windows::Data::Json::IJsonValue _value;
  
  public:
    template <typename T>
    static winrt::Windows::Data::Json::IJsonValue to_json(const T& value)
    {
      using namespace winrt::Windows::Data::Json;

      if constexpr (std::is_same<T, winrt::Windows::Data::Json::JsonValue>::value)
      {
        return value;
      }
      else if constexpr (std::is_same<T, bool>::value)
      {
        return JsonValue::CreateBooleanValue(value);
      }
      else if constexpr (std::is_enum<T>::value)
      {
        return JsonValue::CreateStringValue(Text::enum_value_name_mapping<T>::name(value));
      }
      else if constexpr (std::is_arithmetic<T>::value)
      {
        return JsonValue::CreateNumberValue(double(value));
      }
      else if constexpr (std::is_convertible<T, std::wstring_view>::value)
      {
        return JsonValue::CreateStringValue(winrt::hstring(value));
      }
      else if constexpr (Traits::is_instantiation_of<std::vector, T>::value)
      {
        JsonArray jsonArray{};
        for (auto& item : value)
        {
          jsonArray.Append(to_json(item));
        }
        return jsonArray;
      }
      else if constexpr (std::is_convertible<T, json_value>::value)
      {
        return (json_value)value;
      }      
      else
      {
        static_assert(false, "Cannot serialize this type to json automatically");
      }
    }

    template <typename T>
    static T from_json(const winrt::Windows::Data::Json::IJsonValue& value)
    {
      using namespace winrt::Windows::Data::Json;

      if constexpr (std::is_same<T, bool>::value)
      {
        return value.GetBoolean();
      }
      else if constexpr (std::is_enum<T>::value)
      {
        return Text::enum_value_name_mapping<T>::value(std::wstring(value.GetString()));
      }
      else if constexpr (std::is_arithmetic<T>::value)
      {
        return T(value.GetNumber());
      }
      else if constexpr (std::is_convertible<T, std::wstring_view>::value)
      {
        return T(value.GetString());
      }
      else if constexpr (Traits::is_instantiation_of<std::vector, T>::value)
      {
        auto jsonArray = value.GetArray();
        T result;
        for (const auto& item : jsonArray)
        {
          result.push_back(from_json<typename T::value_type>(item));
        }
        return result;
      }
      else if constexpr (std::is_convertible<json_value, T>::value)
      {
        return T(json_value(value));
      }
      else
      {
        static_assert(false, "Cannot deserialize this type from json automatically");
      }
    }

    json_value() :
      _value(winrt::Windows::Data::Json::JsonValue::CreateNullValue())
    { }

    json_value(const winrt::Windows::Data::Json::IJsonValue& value) :
      _value(value)
    { }

    template <typename T>
    json_value(const T& value) :
      _value(to_json(value))
    { }

    template <typename T>
    operator T() const
    {
      return from_json<T>(_value);
    }

    operator winrt::Windows::Data::Json::IJsonValue& ()
    {
      return _value;
    }

    operator const winrt::Windows::Data::Json::IJsonValue& () const
    {
      return _value;
    }
  };

  class json_object
  {
  private:
    winrt::Windows::Data::Json::JsonObject _value;

  public:
    json_object() = default;

    json_object(const json_value& value) :
      _value(winrt::Windows::Data::Json::IJsonValue(value).GetObject())
    { }

    template <typename T>
    void write(const wchar_t* name, const T& value)
    {
      _value.SetNamedValue(std::wstring(name), json_value(value));
    }

    template <typename T>
    void read(const wchar_t* name, T& value, const T& fallback = T{})
    {
      auto jsonValue = _value.TryLookup(name);
      if (jsonValue)
      {
        value = json_value::from_json<T>(jsonValue);
      }
      else
      {
        value = fallback;
      }
    }

    template <typename T>
    T read_or(const wchar_t* name, const T& fallback = T{})
    {
      T result;
      read(name, result, fallback);
      return result;
    }

    operator winrt::Windows::Data::Json::IJsonValue() const
    {
      return _value;
    }
  };
}