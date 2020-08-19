#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "LPSolveLibraries/lp_lib.h"

using namespace std;

void readGame_Matrix(vector<vector<double>> &, int &, int &, string);
void maxmin(vector<double> &, double &, vector<vector<double>>, int, int);
void printResults(vector<double>, double);

int main(int argc, char **argv)
{
  // Variables
  string game_matrix_file_name = argv[1];
  int number_of_first_player_strategies;
  int number_of_second_player_strategies;
  double game_value;
  vector<vector<double>> game_matrix;
  vector<double> first_player_optimal_mixed_strategy;

  // Methods
  readGame_Matrix(game_matrix, number_of_first_player_strategies, number_of_second_player_strategies, game_matrix_file_name);
  maxmin(first_player_optimal_mixed_strategy, game_value, game_matrix, number_of_first_player_strategies, number_of_second_player_strategies);
  printResults(first_player_optimal_mixed_strategy, game_value);

  return 0;
}

// Reading 2D game matrix from an input file
void readGame_Matrix(vector<vector<double>> & game_matrix, int & number_of_first_player_strategies, int & number_of_second_player_strategies, string game_matrix_file_name)
{
  ifstream game_matrix_file(game_matrix_file_name);
  while(game_matrix_file.peek() != EOF)
  {
    string line;
    vector<double> row;
    double temp;
    getline(game_matrix_file, line);
    istringstream iss(line);
    while(iss >> temp)
      row.push_back(temp);
    game_matrix.push_back(row);
  }
  number_of_first_player_strategies = game_matrix.size();
  number_of_second_player_strategies = game_matrix[0].size();
  return;
}

// Calculating the value of the zero-sum game using maxmin algorithm and LP solver
void maxmin(vector<double> & first_player_optimal_mixed_strategy, double & game_value, vector<vector<double>> game_matrix, int number_of_first_player_strategies, int number_of_second_player_strategies)
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
          row[j + 1] = game_matrix[j][i];
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
  game_value = get_objective(lp);
  get_variables(lp, row);
  for(int i = 0; i < number_of_LP_variables - 1; i++)
    first_player_optimal_mixed_strategy.push_back(row[i]);
  delete_lp(lp);
  return;
}

// Printing the results
void printResults(vector<double> first_player_optimal_mixed_strategy, double game_value)
{
  cout << "\n\n\n";
  cout << "The zero-sum game value is " << game_value << "." << endl;
  cout << "\n\n";
  cout << "Optimal mixed strategy for the first player:" << endl;
  for(int i = 0; i < first_player_optimal_mixed_strategy.size(); i++)
    cout << "p" + to_string(i + 1) + ": " << first_player_optimal_mixed_strategy[i] << endl;
  cout << "\n\n";
  return;
}
