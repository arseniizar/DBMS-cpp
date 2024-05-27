//
// Created by Altezza on 10.05.2024.
//

#ifndef DATABASEPROJECT_UT_PRINT_HPP
#define DATABASEPROJECT_UT_PRINT_HPP

// so in order to print the columns I selected or deleted or inserted I will
// print them in some kind of Table using <iomanip> or using fmt println

void ut_print();
void ut_print(Table t);
void ut_print(std::vector<Column> cols);
void ut_print(Query q);


#endif //DATABASEPROJECT_UT_PRINT_HPP
