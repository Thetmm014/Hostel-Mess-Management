#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>

using namespace std;

// ---- Helper: get today's date as string ----
string today() {
    time_t t = time(0);
    tm* now = localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", now);
    return string(buf);
}

// ---- Helper: get current month-year ----
string currentMonth() {
    time_t t = time(0);
    tm* now = localtime(&t);
    char buf[10];
    strftime(buf, sizeof(buf), "%Y-%m", now);
    return string(buf);
}

// ---- Helper: trim whitespace ----
string trim(const string& s) {
    int start = s.find_first_not_of(" \t\r\n");
    int end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}


class Person {
public:
    string name;
    string password;

public:
    Person(string n, string p) : name(n), password(p) {}

    string getName() { return name; }

    bool checkPassword(string input) {
        return input == password;
    }
};


class ExtraItem {
public:
    string item_name;
    float price;

    ExtraItem(string n, float p) : item_name(n), price(p) {}
};


class Student : public Person {
public:
    string roll_no;
    string room_no;
    string join_date;
    bool is_active;

    Student(string roll, string n, string pass, string room, string jdate, bool active = true)
        : Person(n, pass), roll_no(roll), room_no(room), join_date(jdate), is_active(active) {}
};


class Manager : public Person {
public:
    Manager(string n, string pass) : Person(n, pass) {}
};


class MessSystem {
private:
    // file paths
    string students_file  = "students.csv";
    string attendance_file = "attendance.csv";
    string menu_file      = "menu.csv";
    string complaints_file = "complaints.csv";
    string feedback_file  = "feedback.csv";
    string bills_file     = "bills.csv";

    // meal prices per session
    map<string, float> meal_prices = {
        {"morning",   40.0},
        {"afternoon", 70.0},
        {"evening",   50.0}
    };

    // extras menu loaded from file
    vector<ExtraItem> extras_menu;

    // manager credentials (hardcoded for simplicity)
    Manager manager = Manager("Mess Manager", "admin123");

    // ---- Load extras menu from file ----
    void loadMenu() {
        extras_menu.clear();
        ifstream file(menu_file);
        if (!file.is_open()) return;

        string line;
        getline(file, line); // skip header
        while (getline(file, line)) {
            stringstream ss(line);
            string name, price_str;
            getline(ss, name, ',');
            getline(ss, price_str, ',');
            if (!name.empty())
                extras_menu.push_back(ExtraItem(trim(name), stof(trim(price_str))));
        }
        file.close();
    }

    // ---- Save extras menu to file ----
    void saveMenu() {
        ofstream file(menu_file);
        file << "item_name,price\n";
        for (auto& item : extras_menu)
            file << item.item_name << "," << item.price << "\n";
        file.close();
    }

    // ---- Initialize default menu if file doesn't exist ----
    void initMenu() {
        ifstream file(menu_file);
        if (!file.is_open()) {
            extras_menu = {
                ExtraItem("Egg Curry",   30.0),
                ExtraItem("Paneer",      50.0),
                ExtraItem("Extra Roti",   5.0),
                ExtraItem("Kheer",       20.0),
                ExtraItem("Cold Drink",  25.0),
                ExtraItem("Lassi",       20.0)
            };
            saveMenu();
        } else {
            file.close();
            loadMenu();
        }
    }

    // ---- Load all students from file ----
    vector<Student> loadStudents() {
        vector<Student> students;
        ifstream file(students_file);
        if (!file.is_open()) return students;

        string line;
        getline(file, line); // skip header
        while (getline(file, line)) {
            stringstream ss(line);
            string roll, name, pass, room, jdate, active_str;
            getline(ss, roll, ',');
            getline(ss, name, ',');
            getline(ss, pass, ',');
            getline(ss, room, ',');
            getline(ss, jdate, ',');
            getline(ss, active_str, ',');
            if (!roll.empty())
                students.push_back(Student(trim(roll), trim(name), trim(pass), trim(room), trim(jdate), trim(active_str) == "1"));
        }
        file.close();
        return students;
    }

    // ---- Save all students to file ----
    void saveStudents(vector<Student>& students) {
        ofstream file(students_file);
        file << "roll_no,name,password,room_no,join_date,is_active\n";
        for (auto& s : students)
            file << s.roll_no << "," << s.getName() << "," << s.password << "," << s.room_no << "," << s.join_date << "," << s.is_active << "\n";
        file.close();
    }

    // ---- Generate next roll number ----
    string generateRollNo(vector<Student>& students) {
        int max_num = 0;
        for (auto& s : students) {
            string num_part = s.roll_no.substr(1); // remove 'M' prefix
            int num = stoi(num_part);
            if (num > max_num) max_num = num;
        }
        string new_roll = "M" + to_string(max_num + 1);
        return new_roll;
    }

    // ---- Find student by roll number ----
    int findStudent(vector<Student>& students, string roll) {
        for (int i = 0; i < students.size(); i++)
            if (students[i].roll_no == roll) return i;
        return -1;
    }

    // ---- Record attendance + extras ----
    void recordAttendance(string roll, string session, float base_price, float extras_total, string extras_detail) {
        ofstream file(attendance_file, ios::app);
        file << today() << "," << roll << "," << session << "," << base_price << "," << extras_total << "," << extras_detail << "\n";
        file.close();
    }

    // ---- Display extras menu and take order ----
    float takeExtrasOrder(string& extras_detail) {
        float total = 0;
        extras_detail = "";

        cout << "\n  Extras Menu:\n";
        for (int i = 0; i < extras_menu.size(); i++)
            cout << "  [" << i + 1 << "] " << extras_menu[i].item_name << " - Rs." << extras_menu[i].price << "\n";
        cout << "  [0] No extras\n";

        cout << "\n  Enter item numbers one by one (0 to stop): ";
        int choice;
        while (cin >> choice && choice != 0) {
            if (choice >= 1 && choice <= extras_menu.size()) {
                total += extras_menu[choice - 1].price;
                if (!extras_detail.empty()) extras_detail += " | ";
                extras_detail += extras_menu[choice - 1].item_name;
                cout << "  Added: " << extras_menu[choice - 1].item_name << "\n";
            } else {
                cout << "  Invalid choice.\n";
            }
            cout << "  Next item (0 to stop): ";
        }
        return total;
    }

    // ---- Calculate monthly bill for a student ----
    float calculateBill(string roll, string month) {
        float total = 0;
        ifstream file(attendance_file);
        if (!file.is_open()) return 0;

        string line;
        getline(file, line); // skip header if any
        // check if first line is header
        if (line.find("date") != string::npos) {
            // it's a header, continue
        } else {
            // not a header, process it
            stringstream ss(line);
            string date, r, session, base_str, extras_str, detail;
            getline(ss, date, ',');
            getline(ss, r, ',');
            getline(ss, session, ',');
            getline(ss, base_str, ',');
            getline(ss, extras_str, ',');
            if (trim(r) == roll && date.substr(0, 7) == month)
                total += stof(trim(base_str)) + stof(trim(extras_str));
        }

        while (getline(file, line)) {
            stringstream ss(line);
            string date, r, session, base_str, extras_str, detail;
            getline(ss, date, ',');
            getline(ss, r, ',');
            getline(ss, session, ',');
            getline(ss, base_str, ',');
            getline(ss, extras_str, ',');
            if (trim(r) == roll && date.substr(0, 7) == month)
                total += stof(trim(base_str)) + stof(trim(extras_str));
        }
        file.close();
        return total;
    }

public:

    // ---- Start system ----
    void start() {
        // make sure attendance file has header if new
        ifstream check(attendance_file);
        if (!check.is_open()) {
            ofstream f(attendance_file);
            f << "date,roll_no,session,base_price,extras_total,extras_detail\n";
            f.close();
        }
        check.close();
        initMenu();
        mainMenu();
    }

    // ---- Main menu ----
    void mainMenu() {
        while (true) {
            cout << "\n========================================\n";
            cout << "     Hostel Mess Management System\n";
            cout << "========================================\n";
            cout << " [1] Manager Login\n";
            cout << " [2] Student Login\n";
            cout << " [3] Exit\n";
            cout << "----------------------------------------\n";
            cout << " Choice: ";

            int choice;
            cin >> choice;

            if (choice == 1) managerLogin();
            else if (choice == 2) studentLogin();
            else if (choice == 3) { cout << "\n Goodbye!\n\n"; break; }
            else cout << "\n Invalid choice.\n";
        }
    }

    // ---- Manager login ----
    void managerLogin() {
        cout << "\n  Manager Password: ";
        string pass;
        cin >> pass;

        if (!manager.checkPassword(pass)) {
            cout << "\n  Wrong password!\n";
            return;
        }
        managerMenu();
    }

    // ---- Manager menu ----
    void managerMenu() {
        while (true) {
            cout << "\n  ---- Manager Panel ----\n";
            cout << "  [1] Add New Student\n";
            cout << "  [2] Mark Attendance\n";
            cout << "  [3] View All Students\n";
            cout << "  [4] View Student Bill\n";
            cout << "  [5] Export Monthly Report\n";
            cout << "  [6] Manage Extras Menu\n";
            cout << "  [7] Update Meal Prices\n";
            cout << "  [8] View Complaints\n";
            cout << "  [9] Remove Student (Checkout)\n";
            cout << "  [0] Logout\n";
            cout << "  Choice: ";

            int choice;
            cin >> choice;

            if      (choice == 1) addStudent();
            else if (choice == 2) markAttendance();
            else if (choice == 3) viewAllStudents();
            else if (choice == 4) viewStudentBill();
            else if (choice == 5) exportMonthlyReport();
            else if (choice == 6) manageMenu();
            else if (choice == 7) updateMealPrices();
            else if (choice == 8) viewComplaints();
            else if (choice == 9) checkoutStudent();
            else if (choice == 0) break;
            else cout << "\n  Invalid choice.\n";
        }
    }

    // ---- Add new student ----
    void addStudent() {
        vector<Student> students = loadStudents();
        string roll = generateRollNo(students);

        cout << "\n  --- Add New Student ---\n";
        cout << "  Auto Roll No: " << roll << "\n";

        cin.ignore();
        cout << "  Student Name: ";
        string name; getline(cin, name);

        cout << "  Room Number: ";
        string room; getline(cin, room);

        cout << "  Set Password: ";
        string pass; getline(cin, pass);

        students.push_back(Student(roll, name, pass, room, today()));
        saveStudents(students);

        cout << "\n  Student added! Roll No: " << roll << "\n";
    }

    // ---- Mark attendance ----
    void markAttendance() {
        cout << "\n  --- Mark Attendance ---\n";
        cout << "  Session (morning/afternoon/evening): ";
        string session; cin >> session;

        if (meal_prices.find(session) == meal_prices.end()) {
            cout << "\n  Invalid session.\n";
            return;
        }

        float base_price = meal_prices[session];

        cout << "  Roll No: ";
        string roll; cin >> roll;

        vector<Student> students = loadStudents();
        int idx = findStudent(students, roll);
        if (idx == -1 || !students[idx].is_active) {
            cout << "\n  Student not found or inactive.\n";
            return;
        }

        cout << "\n  Student: " << students[idx].getName() << "\n";
        cout << "  Session: " << session << " | Base Price: Rs." << base_price << "\n";

        cout << "\n  Eating type:\n";
        cout << "  [1] Base meal + extras\n";
        cout << "  [2] Extras only (paying on spot)\n";
        cout << "  [3] Base meal only\n";
        cout << "  Choice: ";

        int type; cin >> type;

        float actual_base = 0;
        float extras_total = 0;
        string extras_detail = "none";

        if (type == 1) {
            actual_base = base_price;
            extras_total = takeExtrasOrder(extras_detail);
        } else if (type == 2) {
            actual_base = 0;
            extras_total = takeExtrasOrder(extras_detail);
        } else if (type == 3) {
            actual_base = base_price;
            extras_detail = "none";
        } else {
            cout << "\n  Invalid choice.\n";
            return;
        }

        recordAttendance(roll, session, actual_base, extras_total, extras_detail);
        cout << "\n  Attendance marked!\n";
        cout << "  Base: Rs." << actual_base << " | Extras: Rs." << extras_total << " | Total: Rs." << actual_base + extras_total << "\n";
    }

    // ---- View all students ----
    void viewAllStudents() {
        vector<Student> students = loadStudents();
        cout << "\n  --- All Students ---\n";
        cout << left << setw(8) << "Roll" << setw(20) << "Name" << setw(8) << "Room" << setw(12) << "Joined" << "Status\n";
        cout << string(58, '-') << "\n";
        for (auto& s : students)
            cout << left << setw(8) << s.roll_no << setw(20) << s.getName() << setw(8) << s.room_no << setw(12) << s.join_date << (s.is_active ? "Active" : "Left") << "\n";
    }

    // ---- View student bill ----
    void viewStudentBill() {
        cout << "\n  Roll No: ";
        string roll; cin >> roll;
        string month = currentMonth();
        float bill = calculateBill(roll, month);
        cout << "\n  Bill for " << roll << " (" << month << "): Rs." << bill << "\n";
    }

    // ---- Export monthly report ----
    void exportMonthlyReport() {
        string month = currentMonth();
        vector<Student> students = loadStudents();

        string filename = "report_" + month + ".csv";
        ofstream report(filename);
        report << "Roll No,Name,Month,Total Bill (Rs.)\n";

        cout << "\n  --- Monthly Report: " << month << " ---\n";
        cout << left << setw(8) << "Roll" << setw(20) << "Name" << "Bill\n";
        cout << string(40, '-') << "\n";

        for (auto& s : students) {
            float bill = calculateBill(s.roll_no, month);
            report << s.roll_no << "," << s.getName() << "," << month << "," << bill << "\n";
            cout << left << setw(8) << s.roll_no << setw(20) << s.getName() << "Rs." << bill << "\n";
        }

        report.close();
        cout << "\n  Report saved to: " << filename << "\n";
    }

    // ---- Manage extras menu ----
    void manageMenu() {
        while (true) {
            cout << "\n  --- Extras Menu ---\n";
            for (int i = 0; i < extras_menu.size(); i++)
                cout << "  [" << i + 1 << "] " << extras_menu[i].item_name << " - Rs." << extras_menu[i].price << "\n";

            cout << "\n  [A] Add item  [D] Delete item  [U] Update price  [B] Back\n";
            cout << "  Choice: ";
            char c; cin >> c;

            if (c == 'A' || c == 'a') {
                cin.ignore();
                cout << "  Item name: ";
                string name; getline(cin, name);
                cout << "  Price: ";
                float price; cin >> price;
                extras_menu.push_back(ExtraItem(name, price));
                saveMenu();
                cout << "  Item added!\n";
            } else if (c == 'D' || c == 'd') {
                cout << "  Item number to delete: ";
                int idx; cin >> idx;
                if (idx >= 1 && idx <= extras_menu.size()) {
                    extras_menu.erase(extras_menu.begin() + idx - 1);
                    saveMenu();
                    cout << "  Item deleted!\n";
                }
            } else if (c == 'U' || c == 'u') {
                cout << "  Item number to update: ";
                int idx; cin >> idx;
                if (idx >= 1 && idx <= extras_menu.size()) {
                    cout << "  New price: ";
                    float price; cin >> price;
                    extras_menu[idx - 1].price = price;
                    saveMenu();
                    cout << "  Price updated!\n";
                }
            } else if (c == 'B' || c == 'b') break;
        }
    }

    // ---- Update meal prices ----
    void updateMealPrices() {
        cout << "\n  --- Update Meal Prices ---\n";
        for (auto& p : meal_prices)
            cout << "  " << p.first << ": Rs." << p.second << "\n";

        cout << "\n  Session to update (morning/afternoon/evening): ";
        string session; cin >> session;

        if (meal_prices.find(session) == meal_prices.end()) {
            cout << "\n  Invalid session.\n";
            return;
        }

        cout << "  New price: ";
        float price; cin >> price;
        meal_prices[session] = price;
        cout << "\n  Price updated!\n";
    }

    // ---- View complaints ----
    void viewComplaints() {
        ifstream file(complaints_file);
        if (!file.is_open()) { cout << "\n  No complaints yet.\n"; return; }

        cout << "\n  --- Complaints ---\n";
        string line;
        getline(file, line); // skip header
        while (getline(file, line))
            cout << "  " << line << "\n";
        file.close();
    }

    // ---- Checkout student ----
    void checkoutStudent() {
        cout << "\n  Roll No to checkout: ";
        string roll; cin >> roll;

        vector<Student> students = loadStudents();
        int idx = findStudent(students, roll);

        if (idx == -1) { cout << "\n  Student not found.\n"; return; }

        // show final bill
        float bill = calculateBill(roll, currentMonth());
        cout << "\n  Final bill for " << students[idx].getName() << ": Rs." << bill << "\n";
        cout << "  Confirm checkout? (y/n): ";
        char c; cin >> c;

        if (c == 'y' || c == 'Y') {
            students[idx].is_active = false;
            saveStudents(students);
            cout << "\n  Student checked out. Record preserved.\n";
        }
    }

    // ---- Find roll number by room number ----
    void findRollByRoom() {
        cout << "\n  Enter your room number: ";
        string room; cin >> room;

        vector<Student> students = loadStudents();
        bool found = false;
        for (auto& s : students) {
            if (s.room_no == room && s.is_active) {
                cout << "\n  Student: " << s.getName() << " | Roll No: " << s.roll_no << "\n";
                found = true;
            }
        }
        if (!found)
            cout << "\n  No active student found for room " << room << "\n";
    }

    // ---- Student login ----
    void studentLogin() {
        cout << "\n  [1] Login with Roll Number\n";
        cout << "  [2] Forgot Roll Number? (use room number)\n";
        cout << "  Choice: ";
        int opt; cin >> opt;

        if (opt == 2) {
            findRollByRoom();
            return;
        }

        cout << "\n  Roll No: ";
        string roll; cin >> roll;

        vector<Student> students = loadStudents();
        int idx = findStudent(students, roll);

        if (idx == -1 || !students[idx].is_active) {
            cout << "\n  Student not found.\n";
            return;
        }

        cout << "  Password: ";
        string pass; cin >> pass;

        if (!students[idx].checkPassword(pass)) {
            cout << "\n  Wrong password!\n";
            return;
        }

        studentMenu(students[idx]);
    }

    // ---- Student menu ----
    void studentMenu(Student& student) {
        while (true) {
            cout << "\n  Welcome, " << student.getName() << " (" << student.roll_no << ")\n";
            cout << "  [1] View My Bill\n";
            cout << "  [2] View Today's Extras Menu\n";
            cout << "  [3] Register Complaint\n";
            cout << "  [4] Give Feedback\n";
            cout << "  [0] Logout\n";
            cout << "  Choice: ";

            int choice; cin >> choice;

            if (choice == 1) {
                float bill = calculateBill(student.roll_no, currentMonth());
                cout << "\n  Your bill for " << currentMonth() << ": Rs." << bill << "\n";
            } else if (choice == 2) {
                cout << "\n  --- Today's Extras Menu ---\n";
                for (auto& item : extras_menu)
                    cout << "  " << item.item_name << " - Rs." << item.price << "\n";
            } else if (choice == 3) {
                cin.ignore();
                cout << "\n  Your complaint: ";
                string complaint; getline(cin, complaint);
                ofstream file(complaints_file, ios::app);
                file << today() << "," << student.roll_no << "," << student.getName() << "," << complaint << "\n";
                file.close();
                cout << "\n  Complaint registered!\n";
            } else if (choice == 4) {
                cin.ignore();
                cout << "\n  Your feedback: ";
                string feedback; getline(cin, feedback);
                ofstream file(feedback_file, ios::app);
                file << today() << "," << student.roll_no << "," << student.getName() << "," << feedback << "\n";
                file.close();
                cout << "\n  Thanks for your feedback!\n";
            } else if (choice == 0) break;
            else cout << "\n  Invalid choice.\n";
        }
    }
};


int main() {
    MessSystem mess;
    mess.start();
    return 0;
}
