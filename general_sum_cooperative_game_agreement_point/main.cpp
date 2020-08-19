#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "LPSolveLibraries/lp_lib.h"

using namespace std;

void readFirst_Player_Utility_Matrix(vector<vector<double>> &, int &, int &, string);
void readSecond_Player_Utility_Matrix(vector<vector<double>> &, int &, int &, string);
bool check(vector<vector<double>>, vector<vector<double>>);
void findAgreement_Point(pair<int, int> &, pair<double, double> &, double &, vector<vector<double>>, vector<vector<double>>);
void calculateZero_Sum_Game_Matrix(vector<vector<double>> &, vector<vector<double>>, vector<vector<double>>);
void maxmin(double &, double &, vector<vector<double>>, double, int, int);
void printResults(pair<int, int>, pair<double, double>, double, double, double);

int main(int argc, char **argv)
{
  // Variables
  string first_player_utility_matrix_file_name = argv[1];
  string second_player_utility_matrix_file_name = argv[2];
  int number_of_first_player_strategies;
  int number_of_second_player_strategies;
  pair<int, int> agreement_point_coordination;
  pair<double, double> agreement_point_value;
  double agreement_value;
  double first_player_share;
  double second_player_share;
  vector<vector<double>> first_player_utility_matrix;
  vector<vector<double>> second_player_utility_matrix;
  vector<vector<double>> zero_sum_game_matrix;

  // Methods
  readFirst_Player_Utility_Matrix(first_player_utility_matrix, number_of_first_player_strategies, number_of_second_player_strategies, first_player_utility_matrix_file_name);
  readSecond_Player_Utility_Matrix(second_player_utility_matrix, number_of_first_player_strategies, number_of_second_player_strategies, second_player_utility_matrix_file_name);
  if(!check(first_player_utility_matrix, second_player_utility_matrix))
  {
    cout << "Invalid Input Files!!!" << endl;
    return 0;
  }
  findAgreement_Point(agreement_point_coordination, agreement_point_value, agreement_value, first_player_utility_matrix, second_player_utility_matrix);
  calculateZero_Sum_Game_Matrix(zero_sum_game_matrix, first_player_utility_matrix, second_player_utility_matrix);
  maxmin(first_player_share, second_player_share, zero_sum_game_matrix, agreement_value, number_of_first_player_strategies, number_of_second_player_strategies);
  printResults(agreement_point_coordination, agreement_point_value, agreement_value, first_player_share, second_player_share);

  return 0;
}

// Reading 2D first player utility matrix from an input file
void readFirst_Player_Utility_Matrix(vector<vector<double>> & first_player_utility_matrix, int & number_of_first_player_strategies, int & number_of_second_player_strategies, string first_player_utility_matrix_file_name)
{
  ifstream first_player_utility_matrix_file(first_player_utility_matrix_file_name);
  while(first_player_utility_matrix_file.peek() != EOF)
  {
    string line;
    vector<double> row;
    double temp;
    getline(first_player_utility_matrix_file, line);
    istringstream iss(line);
    while(iss >> temp)
      row.push_back(temp);
    first_player_utility_matrix.push_back(row);
  }
  number_of_first_player_strategies = first_player_utility_matrix.size();
  number_of_second_player_strategies = first_player_utility_matrix[0].size();
  return;
}

// Reading 2D second player utility matrix from an input file
void readSecond_Player_Utility_Matrix(vector<vector<double>> & second_player_utility_matrix, int & number_of_first_player_strategies, int & number_of_second_player_strategies, string second_player_utility_matrix_file_name)
{
  ifstream second_player_utility_matrix_file(second_player_utility_matrix_file_name);
  while(second_player_utility_matrix_file.peek() != EOF)
  {
    string line;
    vector<double> row;
    double temp;
    getline(second_player_utility_matrix_file, line);
    istringstream iss(line);
    while(iss >> temp)
      row.push_back(temp);
    second_player_utility_matrix.push_back(row);
  }
  number_of_first_player_strategies = second_player_utility_matrix.size();
  number_of_second_player_strategies = second_player_utility_matrix[0].size();
  return;
}

// Checking the correctness of the input files
bool check(vector<vector<double>> first_player_utility_matrix, vector<vector<double>> second_player_utility_matrix)
{
  if(first_player_utility_matrix.size() == second_player_utility_matrix.size())
  {
    int size = first_player_utility_matrix.size();
    for(int i = 0; i < size; i++)
      if(first_player_utility_matrix[i].size() != second_player_utility_matrix[i].size())
        return false;
    return true;
  }
  return false;
}

// Finding agreement point
void findAgreement_Point(pair<int, int> & agreement_point_coordination, pair<double, double> & agreement_point_value, double & agreement_value, vector<vector<double>> first_player_utility_matrix, vector<vector<double>> second_player_utility_matrix)
{
  agreement_value = -1000000;
  for(int i = 0; i < first_player_utility_matrix.size(); i++)
    for(int j = 0; j < first_player_utility_matrix[0].size(); j++)
      if(first_player_utility_matrix[i][j] + second_player_utility_matrix[i][j] > agreement_value)
      {
        agreement_value = max(agreement_value, first_player_utility_matrix[i][j] + second_player_utility_matrix[i][j]);
        agreement_point_coordination.first = i;
        agreement_point_coordination.second = j;
        agreement_point_value.first = first_player_utility_matrix[agreement_point_coordination.first][agreement_point_coordination.second];
        agreement_point_value.second = second_player_utility_matrix[agreement_point_coordination.first][agreement_point_coordination.second];
      }
  return;
}

// Calculating zero-sum game matrix
void calculateZero_Sum_Game_Matrix(vector<vector<double>> & zero_sum_game_matrix, vector<vector<double>> first_player_utility_matrix, vector<vector<double>> second_player_utility_matrix)
{
  zero_sum_game_matrix.resize(first_player_utility_matrix.size(), vector<double> (first_player_utility_matrix[0].size()));
  for(int i = 0; i < first_player_utility_matrix.size(); i++)
    for(int j = 0; j < first_player_utility_matrix[0].size(); j++)
      zero_sum_game_matrix[i][j] = first_player_utility_matrix[i][j] - second_player_utility_matrix[i][j];
  return;
}

// Calculating the value of the zero-sum game using maxmin algorithm and LP solver
void maxmin(double & first_player_share, double & second_player_share, vector<vector<double>> zero_sum_game_matrix, double agreement_value, int number_of_first_player_strategies, int number_of_second_player_strategies)
{
  lprec *lp;
  int number_of_LP_variables = number_of_first_player_strategies + 1;
  int number_of_LP_constraints = number_of_second_player_strategies + 1;
  REAL row[number_of_LP_variables + 1];
  lp = make_lp(0, number_of_LP_variables);
  for(int i = 0; i < number_of_LP_variables; i++)
  {
    if(i < number_of_LP_variables - 1)
      set_bounds(lp, i + 1, 0.0, 1.0);
    else
      set_unbounded(lp, i + 1);
  }
  set_add_rowmode(lp, TRUE);
  for(int i = 0; i < number_of_LP_constraints; i++)
  {
    if(i < number_of_LP_constraints - 1)
    {
      for(int j = 0; j < number_of_LP_variables; j++)
      {
        if(j < number_of_LP_variables - 1)
          row[j + 1] = zero_sum_game_matrix[j][i];
        else
          row[j + 1] = -1;
      }
      add_constraint(lp, row, GE, 0.0);
    }
    else
    {
      for(int j = 0; j < number_of_LP_variables; j++)
      {
        if(j < number_of_LP_variables - 1)
          row[j + 1] = 1;
        else
          row[j + 1] = 0;
      }
      add_constraint(lp, row, EQ, 1.0);
    }
  }
  set_add_rowmode(lp, FALSE);
  for(int i = 0; i < number_of_LP_variables; i++)
  {
    if(i < number_of_LP_variables - 1)
      row[i + 1] = 0;
    else
      row[i + 1] = 1;
  }
  set_obj_fn(lp, row);
  set_maxim(lp);
  set_verbose(lp, IMPORTANT);
  solve(lp);
  first_player_share = (agreement_value + get_objective(lp)) / 2;
  second_player_share = (agreement_value - get_objective(lp)) / 2;
  delete_lp(lp);
  return;
}

// Printing the results
void printResults(pair<int, int> agreement_point_coordination, pair<double, double> agreement_point_value, double agreement_value, double first_player_share, double second_player_share)
{
  cout << "\n\n";
  cout << "The agreement point coordination is (" << agreement_point_coordination.first + 1 << "," << agreement_point_coordination.second + 1 << ")." << endl;
  cout << "\n";
  cout << "The agreement point value is (" << agreement_point_value.first << "," << agreement_point_value.second << ")." << endl;
  cout << "\n";
  cout << "The agreement value is " << agreement_value << "." << endl;
  cout << "\n";
  cout << "The first player share is " << first_player_share << "." << endl;
  cout << "\n";
  cout << "The second player share is " << second_player_share << "." << endl;
  cout << "\n\n";
  return;
}
