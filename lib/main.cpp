#include <fstream>
#include <iostream>
#include <vector>

#include "scanner.h"
#include "tables/const_table.h"
#include "tables/variable_table.h"

using namespace std;

int main(int argc, char** argv) {
   setlocale(LC_ALL, "ru-RU.utf-8");

   auto keywordsTable = std::make_shared<ConstTable>();
   auto splittersTable = std::make_shared<ConstTable>();
   auto operationsTable = std::make_shared<ConstTable>();
   auto constantsTable = std::make_shared<VariableTable<ConstMetaData>>();
   auto variablesTable = std::make_shared<VariableTable<MetaData>>();

   keywordsTable->readFromFile("../../const_tables/keywords.txt");
   splittersTable->readFromFile("../../const_tables/splitters.txt");
   operationsTable->readFromFile("../../const_tables/operations.txt");

   auto scanner = Scanner(keywordsTable, splittersTable, operationsTable, constantsTable, variablesTable);

   ifstream file;
   if (argc == 2) {
      file = ifstream(argv[1]);
   }
   else {
      file = ifstream("../../test_file.txt");
   }

   if (file.is_open()) {
      auto scanResult = scanner.tokenizeStream(file);
      if (scanResult.successed()) {
         for (auto& token : *scanResult.data) {
            cout << token.toString() << " ";
         }
         cout << endl;
      } else {
         cout << scanResult.error << "\n";
      }
   } else {
      cout << "Can't open file!\n";
   }

   return 0;
}
