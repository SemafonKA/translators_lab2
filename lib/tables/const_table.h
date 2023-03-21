#pragma once

#include <fstream>
#include <map>
#include <string>

/// <summary>
/// Класс для константных таблиц. Является обёрткой над map\string, int\,
/// использует string-строки в качестве ключа для поиска и int-значение в
/// качестве номера этого ключа в линейной таблице
/// </summary>
class ConstTable {
  public:
   std::map<std::string, int> data;

   /// <summary>
   /// Функция поиска элемента в таблице по ключу (названию элемента)
   /// </summary>
   /// <param name="elem"> - название элемента</param>
   /// <returns> -1, если элемента в таблице нет, иначе возвращает номер
   /// элемента в таблице</returns>
   int find(const std::string& elem) {
      // Пробуем найти элемент в таблице
      auto elemPtr = data.find(elem);

      // Если элемент не найден
      if (elemPtr == data.end()) {
         return -1;
      } else {
         // Возвращаем второй элемент (int) из пары значений в таблице
         return elemPtr->second;
      }
   }

   /// <summary>
   /// Метод чтения данных таблицы из файла
   /// </summary>
   /// <param name="filePath"> - путь до файла</param>
   void readFromFile(std::string filePath) {
      auto file = std::ifstream(filePath);
      if (!file.is_open()) {
         throw std::runtime_error("Cannot open file " + filePath);
      }

      while (!file.eof()) {
         int num;
         std::string str;
         // Считываем построчно пары (число строка)
         file >> num >> str;

         // Добавляем их в хэш-таблицу
         auto pair = std::make_pair(str, num);
         data.insert(pair);
      }
   }
};