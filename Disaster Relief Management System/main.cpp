#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

struct district {
    string name = "";
    int population = 0;
    float damage_estimate = 0;
    float last_aid_received[10];
    int num_aids = 0;
    int aid_priority = 0;
};

/// Reading and Writing District Data File Functions
void load(string fname, district districts[], int &num) {
    ifstream file_input(fname);
    string line;
    if (file_input.is_open()) {

        // Read each line from the file
        while (getline(file_input, line)) {
            if (districts[num].name.empty()) {
                districts[num].name = line;
            }
            else if (districts[num].population == 0) {
                districts[num].population = stoi(line);
            }
            else if (districts[num].damage_estimate == 0) {
                districts[num].damage_estimate = stof(line);
            }
            else if (districts[num].num_aids == 0) {
                int count = 0;
                stringstream ss(line);
                int i = 0;
                while (ss >> districts[num].last_aid_received[i]) {
                    i++;
                    count++;
                }
                districts[num].num_aids = count;
                num++;
            }

            if (file_input.eof()) {
                break;
            }
        }
        file_input.close();
    }
    
    else {
        cout << "Error: Unable to open file " << fname << endl;
    }

    float last_aid_total;
    for (int i = 0; i < num; i++) {
        last_aid_total = 0;
        for (int j = 0; j < districts[i].num_aids; j++) {
            last_aid_total += districts[i].last_aid_received[j];
        }

        cout << "District: " << districts[i].name << endl;
        cout << "Population: " << districts[i].population << endl;
        cout << "Damage Estimate: " << fixed << setprecision(2) << districts[i].damage_estimate << endl;
        cout << "Total Aid Received: " << fixed << setprecision(2) << last_aid_total << endl;
        cout << "---" << endl;
    }
}

void save(string fname, district districts[], int num) {
    ofstream file_input(fname);
    if (file_input.is_open()) {
        for (int i = 0; i < num; i++) {
            file_input << districts[i].name << endl;
            file_input << districts[i].population << endl;
            file_input << districts[i].damage_estimate << endl;
            for (int j = 0; j < districts[i].num_aids; j++) {
                file_input << districts[i].last_aid_received[j] << " ";
            }
            file_input << endl;
        }
        file_input.close();
    }
    else {
        cout << "Error: Unable to open file " << fname << endl;
    }
}

/// District Management Functions
void update_district_priorities(district districts[], int num_districts) {
    float last_aid_total;
    for (int i = 0; i < num_districts; i++) {
        last_aid_total = 0;
        for (int j = 0; j < districts[i].num_aids; j++) {
            last_aid_total += districts[i].last_aid_received[j];
        }
        districts[i].aid_priority = districts[i].damage_estimate + ((last_aid_total) / 2) + districts[i].population * 10;
    }
    
    // Sort the districts in descending order based on aid priorities
    for (int i = 0; i < num_districts - 1; i++) {
        for (int j = 0; j < num_districts - i - 1; j++) {
            if (districts[j].aid_priority < districts[j + 1].aid_priority) {
                // Swap the districts
                district temp = districts[j];
                districts[j] = districts[j + 1];
                districts[j + 1] = temp;
            }
        }
    }
}

void change_district_damage_estimate(district districts[], int num_districts, string distname, float new_est) {
    int temp = 0;

    for (int i = 0; i < num_districts; i++) {
        if (new_est < 0) {
            cout << "Error: estimate cannot be negative!\n";
            return;
        }
        if (districts[i].name == distname) {
            districts[i].damage_estimate = new_est;
            temp = 1;
            break;
        }
    }

    if (temp == 0) {
        cout << "Error: district not found!\n";
        return;
    } 
}

bool distribute_aid(district districts[], int num_districts, string distname, float amount) {
    int temp = 0;
    if (amount < 0) {
        cout << "Error: aid cannot be negative!\n";
        return false;
    }

    for (int i = 0; i < num_districts; i++) {
        if (districts[i].name == distname) {
            for (int j = districts[i].num_aids; j > 0; j--) {
                districts[i].last_aid_received[j] = districts[i].last_aid_received[j - 1];
            }
            districts[i].last_aid_received[0] = amount;
            districts[i].num_aids++;
            //update damage_est
            districts[i].damage_estimate -= amount;
            temp = 1;
        }
    }
    

    if (temp == 0) {
        cout << "Error: district not found!\n";
        return false;
    }
    else {
        return true;
    }
    
}

void update_aid_distribution(district districts[], int num_districts, float new_aid) {
    float sum = 0;
    float aid_amount = 0;


    for (int i = 0; i < num_districts; i++) {
        sum += districts[i].aid_priority;
    }
    
    for (int i = 0; i < num_districts; i++) {
        aid_amount = (districts[i].aid_priority / sum) * new_aid;

        for (int j = districts[i].num_aids; j > 0; j--) {
            districts[i].last_aid_received[j] = districts[i].last_aid_received[j - 1];
        }
        districts[i].last_aid_received[0] = aid_amount;
        districts[i].damage_estimate -= aid_amount;
        cout << "District: " << districts[i].name << " received aid amount: " << "$" << aid_amount << endl;
        districts[i].num_aids++;
    }
}

void change_district_population(district districts[], int num_districts, string distname, int new_population) {
    int temp = 0;
    for (int i = 0; i < num_districts; i++) {
        if (districts[i].name == distname) {
            districts[i].population = new_population;
            temp = 1;
        }
    }
    float last_aid_total;
    for (int i = 0; i < num_districts; i++) {
        last_aid_total = 0;
        for (int j = 0; j < districts[i].num_aids; j++) {
            last_aid_total += districts[i].last_aid_received[j];
        }
        districts[i].aid_priority = districts[i].damage_estimate + ((last_aid_total) / 2) + districts[i].population * 10;
    }

    if (temp == 0) {
        cout << "Error: district not found!\n";
    }
}

/// Listing and Sorting District Functions
void list_all_districts(district districts[], int num) {
    float last_aid_total;
    for (int i = 0; i < num; i++) {
        last_aid_total = 0;
        for (int j = 0; j < districts[i].num_aids; j++) {
            last_aid_total += districts[i].last_aid_received[j];
        }
        cout << "District: " << districts[i].name << endl;
        cout << "Population: " << districts[i].population << endl;
        cout << "Damage Estimate: " << fixed << setprecision(2) << districts[i].damage_estimate << endl;
        cout << "Total Aid Received: " << fixed << setprecision(2) << last_aid_total << endl;
        cout << "---" << endl;
    }
}

void sort_districts_by_damage(district districts[], int num, bool ascending = true) {
    // Descending
    if (!ascending) {
        for (int i = 0; i < num - 1; i++) {
            for (int j = 0; j < num - i - 1; j++) {
                if (districts[j].damage_estimate < districts[j + 1].damage_estimate) {
                    // Swap the districts
                    district temp = districts[j];
                    districts[j] = districts[j + 1];
                    districts[j + 1] = temp;
                }
            }
        }
    }

    // Ascending
    else {
        for (int i = 0; i < num - 1; i++) {
            for (int j = 0; j < num - i - 1; j++) {
                if (districts[j].damage_estimate > districts[j + 1].damage_estimate) {
                    // Swap the districts
                    district temp = districts[j];
                    districts[j] = districts[j + 1];
                    districts[j + 1] = temp;
                }
            }
        }
    }
}

int main() {
    district districts[100];
    int num = 0;
    string fname_load, fname_save;
    /// Main: ///
    while (true) {
        int user_input;
        cout << endl;
        cout << "-----" << endl;
        cout << "Aid Distribution Management System" << endl;
        cout << "1. File Operations" << endl;
        cout << "2. Listing Options" << endl;
        cout << "3. District Management" << endl;
        cout << "0. Exit" << endl;
        cout << "-----" << endl;
        cout << "Enter your choice: ";
        cin >> user_input;

        /// Main Option 1 - File Operations
        if (user_input == 1) {
            while (true) {
                cin.ignore();
                cout << "1. Load Districts from File" << endl;
                cout << "2. Save Districts to File" << endl;
                cout << "Enter choice: ";
                cin >> user_input;
                if (user_input == 1) {
                    cin.ignore();
                    cout << "Enter filename to load: ";
                    cin >> fname_load;
                    load(fname_load, districts, num);
                    break;
                }
                else if (user_input == 2) {
                    cin.ignore();
                    cout << "Enter filename to save: ";
                    cin >> fname_save;
                    save(fname_save, districts, num);
                    break;
                }
            }
        }

        /// Main Option 2 - View and Sort District Information
        else if (user_input == 2) {
            while (true) {
                float min_damage, max_damage;
                bool ascending;
                cout << "1. List All Districts" << endl;
                cout << "2. List Districts by Damage" << endl;
                cout << "Enter choice: ";
                cin >> user_input;

                // 1. List All Districts
                if (user_input == 1) {
                    float last_aid_total;
                    for (int i = 0; i < num; i++) {
                        last_aid_total = 0;
                        for (int j = 0; j < districts[i].num_aids; j++) {
                            last_aid_total += districts[i].last_aid_received[j];
                        }
                        cout << "District: " << districts[i].name << endl;
                        cout << "Population: " << districts[i].population << endl;
                        cout << "Damage Estimate: " << fixed << setprecision(2) << districts[i].damage_estimate << endl;
                        cout << "Total Aid Received: " << fixed << setprecision(2) << last_aid_total << endl;
                        cout << "---" << endl;
                    }
                    break;
                }

                // 2. List Districts by Damage
                else if (user_input == 2) {
                    cout << "Enter min and max damage (enter -1 for no limit): ";
                    cin >> min_damage >> max_damage;

                    cout << "List in ascending order? (1 for yes, 0 for no): ";
                    cin >> ascending;

                    // Listing: No limit
                    if (min_damage == -1 && max_damage == -1) {
                        sort_districts_by_damage(districts, num, ascending);
                        list_all_districts(districts, num);
                        break;
                    }
                    
                    // Listing: With limit
                    else {
                        int temp_num = 0;
                        district temp[100];
                        if (max_damage == -1) {
                            max_damage = 1000000000000;
                        }
                        for (int i = 0; i < num; i++) {
                            if (districts[i].damage_estimate >= min_damage && districts[i].damage_estimate <= max_damage) {
                                temp[temp_num] = districts[i];
                                temp_num++;
                            }
                        }
                        sort_districts_by_damage(temp, temp_num, ascending);
                        list_all_districts(temp, temp_num);
                        break;
                    }   
                }
            }   
        }

        /// Main Option 3 - District Management
        else if (user_input == 3) {
            while (true) {
                cout << "1. Update District Priorities" << endl;
                cout << "2. Change District Damage Estimate" << endl;
                cout << "3. Distribute Aid to District" << endl;
                cout << "4. Update Aid Distribution" << endl;
                cout << "5. Change District Population" << endl;
                cout << "Enter choice: ";
                cin >> user_input;

                // 1. Update District Priorities
                if (user_input == 1) {
                    update_district_priorities(districts, num);
                    break;
                }

                // 2. Change District Damage Estimate
                else if (user_input == 2) {
                    cout << "Enter district name:\n";
                    string distname;
                    cin.ignore();
                    getline(cin, distname);

                    cout << "Enter new damage estimate:\n";
                    float new_est;
                    cin >> new_est;

                    change_district_damage_estimate(districts, num, distname, new_est);
                    break;
                }

                // 3. Distribute Aid to District
                else if (user_input == 3) {
                    cout << "Enter district name:\n";
                    string distname;
                    cin.ignore();
                    getline(cin, distname);

                    cout << "Enter aid amount:\n";
                    float amount;
                    cin >> amount;

                    distribute_aid(districts, num, distname, amount);
                    break;
                }

                // 4. Update Aid Distribution
                else if (user_input == 4) {
                    cout << "Enter new aid amount for distribution:\n";
                    float new_aid;
                    cin >> new_aid;

                    update_aid_distribution(districts, num, new_aid);
                    break;
                }

                // 5. Change District Population
                else if (user_input == 5) {
                    cout << "Enter district name:\n";
                    string distname;
                    cin.ignore();
                    getline(cin, distname);

                    cout << "Enter new population:\n";
                    int new_population;
                    cin >> new_population;

                    change_district_population(districts, num, distname, new_population);
                    break;
                }
            }
        }

        /// Main Option 0 - Exit
        else if (user_input == 0) {
            cout << "Exiting the program.";
            return 0;
        }
    }
    return 0;
}