#pragma once

#include <fstream>
#include <map>
#include <string>

enum class Type {
   undefined,
   integer,
};

struct MetaData {
   Type type = Type::undefined;
   int value = 0;
};

struct ConstMetaData {
   Type type = Type::integer;
};

/// <summary>
/// Класс для переменных таблиц. Является обёрткой над map\string,pair(int,
/// MetaData)\, использует string-строки в качестве ключа для поиска, int
/// значение для определения номера ключа в таблице и MetaData для получения
/// значения переменной
/// </summary>
template <typename T>
class VariableTable {
  private:
   // Счётчик числа переменных
   int counter = 0;

  public:
   std::map<std::string, std::pair<int, T>> data;

   /// <summary>
   /// Функция поиска элемента по ключу в таблице
   /// </summary>
   /// <param name="elem"> - ключ элемента в таблице</param>
   /// <returns>позиция элемента в таблице (по полю [int])</returns>
   int find(const std::string& elem) {
      // Пробуем найти элемент в таблице
      auto elemPtr = data.find(elem);

      // Если элемент не найден
      if (elemPtr == data.end()) {
         return -1;
      } else {
         // Внебрачный сын int и MetaData
         auto& inita = elemPtr->second;
         return inita.first;
      }
   }

   T* findMetaByIndex(int index) {
      // Если индекс элемента больше, чем в таблице есть
      if (index >= counter || index < 0) {
         return nullptr;
      }

      T* metaPtr = nullptr;
      for (auto& pair : data) {
         auto& value = pair.second;  // pair<int, T>
         if (value.first == index) {
            metaPtr = &(value.second);
            break;
         }
      }

      return metaPtr;
   }

   std::shared_ptr<std::pair<std::string, T&>> findByIndex(int index) {
      if (index >= counter || index < 0) {
         return nullptr;
      }

      for (auto& pair : data) {
         auto& value = pair.second;
         if (value.first == index) {
            auto elem = std::make_pair(pair.first, value.second);
            return std::make_shared(elem);
         }
      }
   }

   /// <summary>
   /// Добавляет элемент в таблицу, либо возвращает номер существующего
   /// элемента
   /// </summary>
   /// <param name="key"> - ключ элемента</param>
   /// <param name="metadata"> - метаданные элемента</param>
   /// <returns>номер вставленного или уже существующего в таблице
   /// элемента</returns>
   int add(std::string key, T metadata = T()) {
      int pos = this->find(key);

      // Если элемент с таким ключом уже существует
      if (pos >= 0) {
         // Обновляем метаданные существующего элемента
         data.at(key).second = metadata;
         return pos;
      }

      // формируем новую пару для хэш-таблицы и вставляем её в таблицу
      auto pair = std::make_pair(key, std::make_pair(counter, metadata));
      data.insert(pair);
      return counter++;
   }
};