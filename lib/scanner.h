#pragma once

#include <array>
#include <fstream>
#include <sstream>
#include <vector>

#include "error_or_t.h"
#include "tables/const_table.h"
#include "tables/variable_table.h"

enum TableNumbers {
   KEYWORDS,
   SPLITTERS,
   OPERATIONS,
   CONSTANTS,
   VARIABLES,
   TABLE_NUMBERS_COUNT,
};

class Token {
  public:
   TableNumbers tableNumber = TableNumbers::TABLE_NUMBERS_COUNT;
   int indexOfElement = -1;

   bool isEmpty = true;

   Token(TableNumbers tableNumber, int indexOfElement)
       : tableNumber(tableNumber), indexOfElement(indexOfElement), isEmpty{false} {}

   Token() {}

   std::string toString() const {
      std::stringstream ss;
      ss << "(" << tableNumber << ", " << indexOfElement << ")";
      return ss.str();
   }

   static Token empty() { return Token(); }
};

class Scanner {
  private:
   enum AutomatonStates {
      INITIAL,
      INT,
      WORD,
      KEYWORD,
      OP_EQ,
      OP_EQ_EQ,
      OP_NE,
      OP_NE_EQ,
      OP_OPERAT,
      S_SPLIT,
      WS_WHITESPACE,
      MINUS_OPERAT,
      END_SUCCESS,
      END_ERROR,
      AUTOMATON_STATES_COUNT,
   };

   std::array<std::array<AutomatonStates, 13>, AutomatonStates::AUTOMATON_STATES_COUNT> automatonMatrix;

   // Функция обработки символов, возвращает номер категории, которой принадлежит символ, либо
   // -1, если символ не принадлежит категориям.
   // Возможные категории:
   // * 0 - категория английский букв,
   // * 1 - категория цифр,
   // * 2 - категория разделителей (',', ';')
   // * 3 - категория скобок (круглые и фигурные)
   // * 4 - символ равенства,
   // * 5 - символ восклицания
   // * 6 - символ сложения
   // * 7 - символ вычитания
   // * 8 - символ умножения
   // * 9 - символ меньше
   // * 10 - пробел
   // * 11 - перенос строки
   // * 12 - несуществующий символ
   int getCharCategory(char ch) {
      // category 0: категория английский букв
      if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {
         return 0;
      }

      // category 1: категория цифр
      if (ch >= '0' && ch <= '9') {
         return 1;
      }

      // category 2: категория разделителей (',', ';')
      if (ch == ',' || ch == ';') {
         return 2;
      }

      // category 3: категория скобок (круглые и фигурные)
      if (ch == '(' || ch == ')' || ch == '{' || ch == '}') {
         return 3;
      }

      // category 4: символ равенства
      if (ch == '=') {
         return 4;
      }

      // category 5: символ восклицания
      if (ch == '!') {
         return 5;
      }

      // category 6: символ сложения
      if (ch == '+') {
         return 6;
      }

      // category 7: символ вычитания
      if (ch == '-') {
         return 7;
      }

      // category 8: символ умножения
      if (ch == '*') {
         return 8;
      }

      // category 9: символ меньше
      if (ch == '<') {
         return 9;
      }

      // category 10: пробел
      if (ch == ' ') {
         return 10;
      }

      // category 11: перенос строки
      if (ch == '\n') {
         return 11;
      }

      // category 12: несуществующий символ
      return 12;
   }

  public:
   std::shared_ptr<ConstTable> keywordTable;
   std::shared_ptr<ConstTable> splittersTable;
   std::shared_ptr<ConstTable> operationsTable;

   std::shared_ptr<VariableTable<ConstMetaData>> constantsTable;
   std::shared_ptr<VariableTable<MetaData>> variablesTable;

   Scanner(std::shared_ptr<ConstTable> keywordTable, std::shared_ptr<ConstTable> splittersTable,
           std::shared_ptr<ConstTable> operationsTable, std::shared_ptr<VariableTable<ConstMetaData>> constantsTable,
           std::shared_ptr<VariableTable<MetaData>> variablesTable)
       : keywordTable(keywordTable),
         splittersTable(splittersTable),
         operationsTable(operationsTable),
         constantsTable(constantsTable),
         variablesTable(variablesTable) {
      automatonMatrix.at(AutomatonStates::INITIAL) = {
          AutomatonStates::WORD,      AutomatonStates::INT,           AutomatonStates::S_SPLIT,
          AutomatonStates::S_SPLIT,   AutomatonStates::OP_EQ,         AutomatonStates::OP_NE,
          AutomatonStates::OP_OPERAT, AutomatonStates::MINUS_OPERAT,  AutomatonStates::OP_OPERAT,
          AutomatonStates::OP_OPERAT, AutomatonStates::WS_WHITESPACE, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::INT) = {
          AutomatonStates::END_ERROR,   AutomatonStates::INT,         AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::WORD) = {
          AutomatonStates::WORD,      AutomatonStates::WORD,    AutomatonStates::KEYWORD, AutomatonStates::KEYWORD,
          AutomatonStates::KEYWORD,   AutomatonStates::KEYWORD, AutomatonStates::KEYWORD, AutomatonStates::KEYWORD,
          AutomatonStates::KEYWORD,   AutomatonStates::KEYWORD, AutomatonStates::KEYWORD, AutomatonStates::KEYWORD,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::KEYWORD) = {
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::OP_EQ) = {
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::OP_EQ_EQ,    AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::OP_EQ_EQ) = {
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_ERROR,   AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::OP_NE) = {
          AutomatonStates::END_ERROR, AutomatonStates::END_ERROR, AutomatonStates::END_ERROR,
          AutomatonStates::END_ERROR, AutomatonStates::OP_NE_EQ,  AutomatonStates::END_ERROR,
          AutomatonStates::END_ERROR, AutomatonStates::END_ERROR, AutomatonStates::END_ERROR,
          AutomatonStates::END_ERROR, AutomatonStates::END_ERROR, AutomatonStates::END_ERROR,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::OP_NE_EQ) = {
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_ERROR,   AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::OP_OPERAT) = {
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_ERROR,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_ERROR,   AutomatonStates::END_ERROR,
          AutomatonStates::END_ERROR,   AutomatonStates::END_ERROR,   AutomatonStates::END_ERROR,
          AutomatonStates::END_ERROR,   AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::S_SPLIT) = {
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::MINUS_OPERAT) = {
          AutomatonStates::END_SUCCESS, AutomatonStates::INT, AutomatonStates::END_ERROR,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_ERROR,   AutomatonStates::END_ERROR,
          AutomatonStates::END_ERROR,   AutomatonStates::END_ERROR,   AutomatonStates::END_ERROR,
          AutomatonStates::END_ERROR,   AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
      automatonMatrix.at(AutomatonStates::WS_WHITESPACE) = {
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,   AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,   AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::END_SUCCESS,   AutomatonStates::END_SUCCESS,
          AutomatonStates::END_SUCCESS, AutomatonStates::WS_WHITESPACE, AutomatonStates::END_SUCCESS,
          AutomatonStates::END_ERROR,
      };
   }

   ErrorOr<std::vector<Token>> tokenizeStream(std::istream& input) {
      auto outTokens = std::make_shared<std::vector<Token>>();  // Вектор выходных токенов
      std::string currentLine;   // Текущая считанная строка потока
      std::stringstream errors;  // Общий буфер всех ошибок
      bool hasErrors = false;    // Переменная-индикатор ошибок
      size_t lineNumber = 0;     // Номер текущей строки (для вывода ошибок)

      // Начинаем читать переданный поток построчно
      while (std::getline(input, currentLine)) {
         lineNumber++;

         // Добавляем в считанную строку символ переноса строки
         currentLine += "\n";

         size_t charNumber = 0;  // Номер текущего символа строки (для вывода ошибок и перемещения по строке)
         while (charNumber < currentLine.size() - 1) {
            // Инициализируем автомат
            AutomatonStates state = AutomatonStates::INITIAL;  // Текущее состояние машины
            std::stringstream buf;  // Буфер, который формируется в течение работы автомата
            auto token = Token::empty();
            char ch = currentLine.at(charNumber);
            state = automatonMatrix.at(state).at(getCharCategory(ch));

            // Запускаем автомат
            while (state != AutomatonStates::END_SUCCESS && state != AutomatonStates::END_ERROR) {
               switch (state) {
                  case AutomatonStates::INT: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));

                     if (state == AutomatonStates::END_SUCCESS) {
                        int tokenNum = constantsTable->add(buf.str());
                        token = Token(TableNumbers::CONSTANTS, tokenNum);
                     }

                     break;
                  }

                  case AutomatonStates::WORD: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));
                     break;
                  }

                  case AutomatonStates::KEYWORD: {
                     int tokenNum = keywordTable->find(buf.str());

                     if (tokenNum == -1) {
                        tokenNum = variablesTable->add(buf.str());
                        token = Token(TableNumbers::VARIABLES, tokenNum);
                     } else {
                        token = Token(TableNumbers::KEYWORDS, tokenNum);
                     }

                     state = AutomatonStates::END_SUCCESS;
                     break;
                  }

                  case AutomatonStates::OP_EQ: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));

                     if (state == AutomatonStates::END_SUCCESS) {
                        int tokenNum = operationsTable->find(buf.str());
                        if (tokenNum == -1) {
                           state = AutomatonStates::END_ERROR;
                        } else {
                           token = Token(TableNumbers::OPERATIONS, tokenNum);
                        }
                     }
                     break;
                  }

                  case AutomatonStates::OP_EQ_EQ: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));

                     if (state == AutomatonStates::END_SUCCESS) {
                        int tokenNum = operationsTable->find(buf.str());
                        if (tokenNum == -1) {
                           state = AutomatonStates::END_ERROR;
                        } else {
                           token = Token(TableNumbers::OPERATIONS, tokenNum);
                        }
                     }
                     break;
                  }

                  case AutomatonStates::OP_NE: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));
                     break;
                  }

                  case AutomatonStates::OP_NE_EQ: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));

                     if (state == AutomatonStates::END_SUCCESS) {
                        int tokenNum = operationsTable->find(buf.str());
                        if (tokenNum == -1) {
                           state = AutomatonStates::END_ERROR;
                        } else {
                           token = Token(TableNumbers::OPERATIONS, tokenNum);
                        }
                     }
                     break;
                  }

                  case AutomatonStates::OP_OPERAT: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));

                     if (state == AutomatonStates::END_SUCCESS) {
                        int tokenNum = operationsTable->find(buf.str());
                        if (tokenNum == -1) {
                           state = AutomatonStates::END_ERROR;
                        } else {
                           token = Token(TableNumbers::OPERATIONS, tokenNum);
                        }
                     }
                     break;
                  }

                  case AutomatonStates::MINUS_OPERAT: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));

                     if (state == AutomatonStates::END_SUCCESS) {
                        int tokenNum = operationsTable->find(buf.str());
                        if (tokenNum == -1) {
                           state = AutomatonStates::END_ERROR;
                        } else {
                           token = Token(TableNumbers::OPERATIONS, tokenNum);
                        }
                     }
                     break;
                  }

                  case AutomatonStates::S_SPLIT: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));

                     if (state == AutomatonStates::END_SUCCESS) {
                        int tokenNum = splittersTable->find(buf.str());
                        if (tokenNum == -1) {
                           state = AutomatonStates::END_ERROR;
                        } else {
                           token = Token(TableNumbers::SPLITTERS, tokenNum);
                        }
                     }
                     break;
                  }

                  case AutomatonStates::WS_WHITESPACE: {
                     buf << ch;
                     charNumber++;
                     ch = currentLine.at(charNumber);
                     state = automatonMatrix.at(state).at(getCharCategory(ch));
                     break;
                  }
               }
            }

            // Завершаем автомат
            if (state == AutomatonStates::END_ERROR) {
               hasErrors = true;
               buf.str("");  // Очищаем буфер перед записью туда ошибки
               buf << "Error: встречен недопустимый символ в позиции: (" << lineNumber << ", " << charNumber + 1
                   << ").";
               errors << buf.str() << "\n";
               // Считываем все символы до пробела или конца строки (скипаем ошибочную структуру - всё равно там уже
               // ошибка)
               while (currentLine.at(charNumber) != '\n' && currentLine.at(charNumber) != ' ') {
                  charNumber++;
               }
            } else if (state == AutomatonStates::END_SUCCESS) {
               if (!token.isEmpty) {
                  outTokens->push_back(token);
               }
            }
         }
      }

      if (hasErrors) {
         return ErrorOr<std::vector<Token>>::withError(errors.str());
      } else {
         return ErrorOr<std::vector<Token>>::withSuccess(outTokens);
      }
   }
};
