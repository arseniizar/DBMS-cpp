//
// Created by Altezza on 10.05.2024.
//

#ifndef DATABASEPROJECT_UT_PRINT_HPP
#define DATABASEPROJECT_UT_PRINT_HPP

// so in order to print the columns I selected or deleted or inserted I will
// print them in some kind of table using <iomanip> or using fmt println

void ut_print();
void ut_print(table t);
void ut_print(std::vector<column> cols);
void ut_print(query q);


#endif //DATABASEPROJECT_UT_PRINT_HPP
