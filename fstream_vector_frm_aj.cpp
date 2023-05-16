// 5/16/2023 from aj tans
//gamint ang fstream, tagasalin ang vector at final save ito sa fstream
//mabagal sa fstream kaya nag container muna ung vector. at final na pag save
//icopy ito ng buo sa fsteam
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <vector>

struct stformat {
  int password; // What is this for? Reserved for admin?
  int length;
};

struct stdata {
  int number; // Phone number?
  char date[100]; // What is the format?
  char name[100]; // Name of what?
  double amount;
};

bool get_data_header(std::ifstream& stream, stformat& output);

bool get_data_body(std::ifstream& stream, int n_entries, std::vector<stdata>& output);

bool update_data(const char db_file_path[], stformat header, const std::vector<stdata>& entry_list);

void display_entry(const stdata& entry, int index);

void get_index(int n_entries, int& output);

void get_entry(stdata& output);

bool get_string(std::string& output);

bool get_integer(int& output);

bool get_floating_point(double& output);

void clear_cin();

/* ========================================================================== */

int main() {
  const char db_file_path[] = "data.db";

  std::ifstream db_file_in;
  std::vector<stdata> entry_list;
  stformat header;

  db_file_in.open(db_file_path, std::ios::binary);

  if ( !(db_file_in.is_open()) ||
       !(get_data_header(db_file_in, header)) ||
       !(get_data_body(db_file_in, header.length, entry_list)) ) {
    std::cout << "ERROR: Failed to load data from \"" << db_file_path << "\".\n"
              << "1: Create new\n"
              << "2: Exit\n";
    int choice;

    while ( !(get_integer(choice)) || choice < 1 || choice > 2 ) {
      std::cout << "ERROR: Invalid input." << '\n';
    }

    if (choice == 1) {
      std::cout << "Enter a new password: ";
      while ( !(get_integer(header.password)) || header.password < 0 ) {
        std::cout << "ERROR: Invalid input." << '\n';
      }

      header.length = 0;
    }
    else /* if (choice == 2) */ {
      return EXIT_FAILURE;
    }
  }

  db_file_in.close();

  // After a successful read, `header.n_entries` should be equal to `entry_list.size()`.

  while (1) {
    std::cout << "======================\n"
              << "1: Display all entries\n"
              << "2: Display an entry\n"
              << "3: Create an entry\n"
              << "4: Update an entry\n"
              << "5: Delete an entry\n"
              << "6: Exit\n"
              << "======================\n";

    int choice;

    while ( !(get_integer(choice)) || choice < 1 || choice > 6 ) {
      std::cout << "ERROR: Invalid input." << '\n';
    }

    if (choice == 1) { // display all entries
      if (entry_list.empty()) {
        std::cout << "There are no entries in the database." << '\n';
        continue; // back to main menu
      }

      // for `std::vector`, `size()` returns the number of elements
      for (int i = 0, n = entry_list.size(); i < n; i += 1) {
        display_entry(entry_list[i], i);
      }
    }
    else if (choice == 2) { // display one entry
      if (entry_list.empty()) {
        std::cout << "There are no entries in the database." << '\n';
        continue; // back to main menu
      }

      int index;

      get_index(entry_list.size(), index);
      display_entry(entry_list[index], index);
    }
    else if (choice == 3) { // create an entry
      stdata entry;

      get_entry(entry);
      entry_list.push_back(entry); // add a new element to the vector

      if ( !(update_data(db_file_path, header, entry_list)) ) {
        std::cout << "Failed to update \"" << db_file_path << "\".\n";
        return EXIT_FAILURE;
      }

      std::cout << "Database updated.\n";
    }
    else if (choice == 4) { // update an entry
      if (entry_list.empty()) {
        std::cout << "There are no entries in the database." << '\n';
        continue; // back to main menu
      }

      int index;
      stdata entry;

      get_index(entry_list.size(), index);
      get_entry(entry);
      entry_list[index] = entry;

      if ( !(update_data(db_file_path, header, entry_list)) ) {
        std::cout << "Failed to update \"" << db_file_path << "\".\n";
        return EXIT_FAILURE;
      }

      std::cout << "Database updated.\n";
    }
    else if (choice == 5) { // delete an entry
      if (entry_list.empty()) {
        std::cout << "There are no entries in the database." << '\n';
        continue; // back to main menu
      }

      int index;

      get_index(entry_list.size(), index);

      // `entry_list.begin() + index` results to an iterator pointing to the
      // element to be removed.
      entry_list.erase(entry_list.begin() + index);

      if ( !(update_data(db_file_path, header, entry_list)) ) {
        std::cout << "Failed to update \"" << db_file_path << "\".\n";
        return EXIT_FAILURE;
      }

      std::cout << "Database updated.\n";
    }
    else /* if (choice == 6) */ { // exit
      break;
    }
  }

  return EXIT_SUCCESS;
}

/* ========================================================================== */

bool get_data_header(std::ifstream& stream, stformat& output) {
  stream.read((char*)&output, sizeof(stformat));
  return !(stream.fail());
}

bool get_data_body(std::ifstream& stream, int n_entries, std::vector<stdata>& output) {
  // Since the number of entries is known ahead of time, we reserve some
  // memory for the vector to reduce the number of reallocations.
  output.reserve(n_entries);

  stdata entry;
  int i;

  for (i = 0; i < n_entries; i += 1) {
    stream.read((char*)&entry, sizeof(stdata));

    if (stream.fail()) {
      break;
    }
    else {
      output.push_back(entry); // append a new element to the vector
    }
  }

  return i == n_entries;
}

bool update_data(const char db_file_path[], stformat header, const std::vector<stdata>& entry_list) {
  std::ofstream db_file_out;

  db_file_out.open(db_file_path, std::ios::binary);

  if ( !(db_file_out.is_open()) ) {
    return false;
  }

  header.length = entry_list.size();
  db_file_out.write((const char*)&header, sizeof(stformat));

  if (db_file_out.fail()) {
    return false;
  }

  for (int i = 0, n = entry_list.size(); i < n; i += 1) {
    db_file_out.write((const char*)&(entry_list[i]), sizeof(stdata));

    if (db_file_out.fail()) {
      return false;
    }
  }

  return true;
}

void display_entry(const stdata& entry, int index) {
  std::cout << "--------------------\n"
            << "Index:  " << index << "\n"
            << "Number: " << entry.number << "\n"
            << "Date:   " << entry.date << "\n"
            << "Name:   " << entry.name << "\n"
            << "Amount: " << entry.amount << "\n";
}

void get_index(int n_entries, int& output) {
  std::cout << "Index: ";

  while ( !(get_integer(output)) || output < 0 || output >= n_entries ) {
    std::cout << "ERROR: Invalid index.\n";
  }
}

void get_entry(stdata& output) {
  std::string s;

  std::cout << "Number: ";
  while ( !(get_integer(output.number)) ) {
    std::cout << "ERROR: Invalid input.\n";
  }

  std::cout << "Date: ";
  while ( !(get_string(s)) || s.size() >= sizeof(output.date) ) {
    std::cout << "ERROR: Invalid input.\n";
  }
  std::strcpy(output.date, s.c_str());

  std::cout << "Name: ";
  while ( !(get_string(s)) || s.size() >= sizeof(output.name) ) {
    std::cout << "ERROR: Invalid input.\n";
  }
  std::strcpy(output.name, s.c_str());

  std::cout << "Amount: ";
  while ( !(get_floating_point(output.amount)) ) {
    std::cout << "ERROR: Invalid input.\n";
  }
}

// To keep things easy to manage, all reads from `std::cin` are line-oriented.
// `std::getline()` consumes all characters, up to and including the line delimiter.
// This is why `get_integer()` and `get_floating_point()` uses this function.
bool get_string(std::string& output) {
  // If an error occurs, `output` is left empty.
  std::getline(std::cin, output);

  bool fail = std::cin.fail();
  std::cin.clear(); // clear error flags

  return !fail;
}

bool get_integer(int& output) {
  std::string s;
  std::istringstream iss;

  if ( !(get_string(s)) ) {
    return false;
  }

  iss.str(s); // assign a new string buffer to the input stream
  output = 0;
  // If an error occurs, `output` is not modified.
  iss >> output;

  return !(iss.fail());
}

bool get_floating_point(double& output) {
  std::string s;
  std::istringstream iss;

  if ( !(get_string(s)) ) {
    return false;
  }

  iss.str(s); // assign a new string buffer to the input stream
  output = 0;
  // If an error occurs, `output` is not modified.
  iss >> output;

  return !(iss.fail());
}
