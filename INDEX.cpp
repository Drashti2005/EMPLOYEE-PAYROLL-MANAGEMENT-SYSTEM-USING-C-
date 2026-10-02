#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <limits>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

class Input {
public:
    static int getInt(const string& message) {
        int value; 
        while (true) {
            cout << message;
            if (cin >> value) {
                cin.ignore( numeric_limits<streamsize>::max(),'\n' );                
                return value;
            }
            cout << RED << "Invalid input! Enter a number.\n" << RESET;
            cin.clear();
            cin.ignore( numeric_limits<streamsize>::max(),'\n' );
        }
    }

    static string getString(const string& message) {
        string value;
        cout << message;
        getline(cin, value);
        return value;
    }
};

class Employee {
private:
    int employeeId;
    string name;
    string department;
    string designation;
    string phone;
    string address;
    string joiningDate;
    int salary;

public:
    Employee(): employeeId(0), salary(0) {}

    Employee(int id, const string& empName, const string& dept, const string& desig, const string& ph, const string& addr, const string& joinDate, int sal) : employeeId(id), name(empName), department(dept), designation(desig), phone(ph), address(addr), joiningDate(joinDate), salary(sal) {}

    int getId() const {
        return employeeId;
    }

    string getName() const {
        return name;
    }

    string getDepartment() const {
        return department;
    }

    string getDesignation() const {
        return designation;
    }

    string getPhone() const {
        return phone;
    }

    string getAddress() const {
        return address;
    }

    string getJoiningDate() const {
        return joiningDate;
    }

    int getSalary() const {
        return salary;
    }

    void setName(const string& value) {
        name = value;
    }

    void setDepartment(const string& value) {
        department = value;
    }

    void setDesignation(const string& value) {
        designation = value;
    }

    void setPhone(const string& value) {
        phone = value;
    }

    void setAddress(const string& value) {
        address = value;
    }

    void setJoiningDate(const string& value) {
        joiningDate = value;
    }

    void setSalary(int value) {
        if (value < 0)
            throw invalid_argument("Salary cannot be negative.");

        salary = value;
    }

    string toCSV() const {
        return to_string(employeeId) + "," + name + "," + department + "," + designation + "," + phone + "," + address + "," + joiningDate + "," + to_string(salary); 
    }

    static Employee fromCSV(const string& line) {

        stringstream ss(line);

        string id;
        string name;
        string dept;
        string desig;
        string phone;
        string address;
        string joiningDate;
        string salary;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, dept, ',');
        getline(ss, desig, ',');
        getline(ss, phone, ',');
        getline(ss, address, ',');
        getline(ss, joiningDate, ',');
        getline(ss, salary, ',');

        return Employee(stoi(id), name, dept, desig, phone, address, joiningDate, stoi(salary));
    }
};

class EmployeeDatabase {
private:
    string fileName;

public:
    explicit EmployeeDatabase(const string& file = "Data.csv") : fileName(file) {}
    void save(const vector<Employee>& employees) const {
        ofstream file(fileName);
        if (!file)
            throw runtime_error("Unable to open database file.");

        for (const Employee& emp : employees) {
            file << emp.toCSV() << '\n';
        }
    }

    vector<Employee> load() const {
        vector<Employee> employees;

        ifstream file(fileName);
        if (!file)
            return employees;

        string line;

        while (getline(file, line)) {
            if (!line.empty()) {
                try {
                    employees.push_back(Employee::fromCSV(line));
                }
                catch (...) {
                    cerr << "Warning: Invalid employee record skipped.\n";
                }
            }
        }
        return employees;
    }
};

class EmployeeManager {

private:
    vector<Employee> employees;
    EmployeeDatabase database;

public:
    EmployeeManager() : database("Data.csv") {
        loadEmployees();
    }

    void loadEmployees() {
        employees = database.load();
    }

    void saveEmployees() {
        database.save(employees);
    }
    
    void addEmployee(const Employee& employee) {

        if (findById(employee.getId()) != nullptr) {
            throw runtime_error("Employee ID already exists.");
        }
        employees.push_back(employee);
        saveEmployees();
        cout << GREEN << "Employee added successfully!\n" << RESET;
    }

    Employee* findById(int id) {
        for (Employee& employee : employees) {
            if (employee.getId() == id)
                return &employee;
        }
        return nullptr;
    }

    const Employee* findById(int id) const {
        for (const Employee& employee : employees) {
            if (employee.getId() == id)
                return &employee;
        }
        return nullptr;
    }

    bool deleteEmployee(int id) {
        auto it = remove_if( employees.begin(), employees.end(), [id](const Employee& employee) {
                return employee.getId() == id;
            }
        );

        if (it == employees.end())
            return false;
        employees.erase(it, employees.end());
        saveEmployees();
        return true;
    }

    void displayEmployee(int id) const {
        const Employee* employee = findById(id);
        if (employee == nullptr) {
            cout << RED << "Employee not found!\n" << RESET;
            return;
        }

        cout << CYAN << "\n-----------------------------------------------------------------------------------------------------------\n" << RESET;
        cout << YELLOW << setw(8)  << "ID" << setw(15) << "Name" << setw(15) << "Department" << setw(20) << "Designation" << setw(12) << "Phone" << setw(15) << "Address" << setw(15) << "Joining Date" << setw(10) << "Salary" << RESET << '\n';
        cout << CYAN << "-----------------------------------------------------------------------------------------------------------\n" << RESET;

        string deptColor = RESET;

        if (employee->getDepartment() == "IT")
            deptColor = GREEN;

        else if (employee->getDepartment() == "HR")
            deptColor = MAGENTA;

        else if (employee->getDepartment() == "Finance")
            deptColor = BLUE;

        cout << setw(8) << employee->getId() << setw(15) << employee->getName() << deptColor << setw(15) << employee->getDepartment() << RESET << setw(20) << employee->getDesignation() << setw(12) << employee->getPhone() << setw(15) << employee->getAddress() << setw(15) << employee->getJoiningDate();

        if (employee->getSalary() > 50000) {
            cout << RED << setw(10) << employee->getSalary() << RESET;
        }
        else {
            cout << setw(10) << employee->getSalary();
        }
        cout << '\n';
        cout << CYAN << "-----------------------------------------------------------------------------------------------------------\n" << RESET;
    }

    void updateEmployee(int id) {
        Employee* employee = findById(id);

        if (employee == nullptr) {
            cout << RED << "Employee not found!\n" << RESET;
            return;
        }

        cout << "\nUpdating Employee " << id << "...\n";

        while (true) {
            cout << "\n" << "1. Name\n" << "2. Department\n" << "3. Designation\n" << "4. Phone\n" << "5. Address\n" << "6. Joining Date\n" << "7. Salary\n" << "8. Exit\n";

            int choice = Input::getInt("Enter choice: ");

            switch (choice) {
                case 1: {
                    string value = Input::getString( "Enter new Name: " );
                    employee->setName(value);
                    break;
                }

                case 2: {
                    string value = Input::getString( "Enter new Department: " );                    
                    employee->setDepartment(value);
                    break;
                }

                case 3: {
                    string value = Input::getString( "Enter new Designation: " );
                    employee->setDesignation(value);
                    break;
                }

                case 4: {
                    string value = Input::getString( "Enter new Phone: " );
                    employee->setPhone(value);
                    break;
                }

                case 5: {
                    string value = Input::getString( "Enter new Address: " );
                    employee->setAddress(value);
                    break;
                }

                case 6: {
                    string value = Input::getString( "Enter new Joining Date: " );
                    employee->setJoiningDate(value);
                    break;
                }

                case 7: {
                    int salary = Input::getInt( "Enter new Salary: ");
                    employee->setSalary(salary);
                    break;
                }

                case 8:
                    saveEmployees();
                    cout << GREEN << "Update completed.\n" << RESET;
                    return;

                default:
                    cout << RED << "Invalid choice!\n" << RESET;
                    continue;
            }
            saveEmployees();
            cout << GREEN << "Field updated successfully!\n" << RESET;
        }
    }

    const vector<Employee>& getEmployees() const {
        return employees;
    }
};

class Payroll {

private:
    int allowances;
    int deductions;
    int overtime;

public:

    Payroll() : allowances(0), deductions(0), overtime(0) {}

    Payroll(int allow, int deduct, int over) : allowances(allow),  deductions(deduct),  overtime(over) {}

    int calculateNetSalary(const Employee& employee) const {
        return employee.getSalary() + allowances + overtime - deductions;
    }

    void inputPayrollDetails() {
        allowances = Input::getInt( "Enter Allowances: ");
        deductions = Input::getInt( "Enter Deductions: ");
        overtime = Input::getInt( "Enter Overtime Pay: ");
    }

    void displaySummary(const Employee& employee) const {
        int netSalary = calculateNetSalary(employee);

        cout << CYAN << "\n------------------- Payroll Summary -------------------\n" << RESET;
        cout << "Employee ID: " << employee.getId() << '\n';
        cout << "Name: " << employee.getName() << '\n';
        cout << "Basic Salary: " << employee.getSalary() << '\n';
        cout << "Allowances: " << allowances << '\n';
        cout << "Overtime: " << overtime << '\n';
        cout << "Deductions: " << deductions << '\n';
        cout << YELLOW << "Net Salary: " << netSalary << RESET << '\n';
        cout << CYAN << "-------------------------------------------------------\n" << RESET;
    }

    int getNetSalary(const Employee& employee) const {
        return calculateNetSalary(employee);
    }
};

class Payslip {

public:

    static void generate( const Employee& employee, const Payroll& payroll) {
        int netSalary = payroll.getNetSalary(employee);

        string fileName = "Payslip_" + to_string(employee.getId()) + ".txt";
        ofstream file(fileName);

        if (!file) {
            throw runtime_error( "Unable to create payslip." );
        }

        file << "------------------- Employee Payslip -------------------\n";
        file << "Employee ID: " << employee.getId() << '\n';
        file << "Name: " << employee.getName() << '\n';
        file << "Department: " << employee.getDepartment() << '\n';
        file << "Designation: " << employee.getDesignation() << '\n';
        file << "Basic Salary: " << employee.getSalary() << '\n';
        file << "Net Salary: " << netSalary << '\n';
        file << "--------------------------------------------------------\n";
        file.close();
        cout << GREEN << "Payslip generated successfully!\n" << RESET;
    }
};

class PayrollReport {

public:

    static void generate( const vector<Employee>& employees) {
        cout << CYAN << "\n================================= Monthly Payroll Report =================================\n" << RESET;
        cout << YELLOW << setw(8)  << "ID" << setw(15) << "Name" << setw(15) << "Department" << setw(20) << "Designation" << setw(10) << "Basic" << setw(12) << "Net Salary" << RESET << '\n';
        cout << CYAN << "==========================================================================================\n" << RESET;
        long long totalSalary = 0;
        for (const Employee& employee : employees) {
            int netSalary = employee.getSalary();
            totalSalary += netSalary;

            string deptColor = RESET;

            if (employee.getDepartment() == "IT")
                deptColor = GREEN;

            else if (employee.getDepartment() == "HR")
                deptColor = MAGENTA;

            else if (employee.getDepartment() == "Finance")
                deptColor = BLUE;

            cout << setw(8) << employee.getId() << setw(15) << employee.getName() << deptColor << setw(15) << employee.getDepartment() << RESET << setw(20) << employee.getDesignation() << setw(10) << employee.getSalary();

            if (netSalary > 50000) {
                cout << RED << setw(12) << netSalary << RESET;
            }
            else {
                cout << setw(12) << netSalary;
            }
            cout << '\n';
        }

        cout << CYAN << "==========================================================================================\n" << RESET;
        cout << YELLOW << setw(58) << "TOTAL" << setw(22) << totalSalary << RESET << '\n';
        cout << CYAN << "==========================================================================================\n" << RESET;
    }
};

class User {

protected:
    string username;

public:

    explicit User(const string& user) : username(user) {}
    virtual ~User() = default;
    virtual void showMenu() const = 0;
    string getUsername() const {
        return username;
    }
};

class Admin : public User {

private:
    EmployeeManager& manager;

public:

    Admin(const string& user, EmployeeManager& employeeManager) : User(user), manager(employeeManager) {}

    void showMenu() const override {
        cout << "\n" << CYAN << "========== ADMIN MENU ==========" << RESET << "\n";
        cout << "1. Add New Employee\n";
        cout << "2. Display Employee\n";
        cout << "3. Update Employee\n";
        cout << "4. Delete Employee\n";
        cout << "5. Calculate Salary\n";
        cout << "6. Monthly Payroll Report\n";
        cout << "7. Exit\n";
    }

    void run() {
        while (true) {
            showMenu();
            int choice = Input::getInt("Enter choice: ");

            try {
                switch (choice) {
                    case 1:
                        addEmployee();
                        break;

                    case 2:
                        displayEmployee();
                        break;

                    case 3:
                        updateEmployee();
                        break;

                    case 4:
                        deleteEmployee();
                        break;

                    case 5:
                        calculateSalary();
                        break;

                    case 6:
                        PayrollReport::generate( manager.getEmployees() );                    
                        break;

                    case 7:
                        cout << "Exiting Admin Menu...\n";
                        return;

                    default:
                        cout << RED << "Invalid choice!\n" << RESET;
                }
            }
            catch (const exception& e) {
                cout << RED << "Error: " << e.what() << RESET << '\n';
            }
        }
    }

private:

    void addEmployee() {
        cout << "\nEnter Employee Details:\n";
        int id = Input::getInt("ID: ");
        string name = Input::getString("Name: ");
        string dept = Input::getString("Department: ");
        string designation = Input::getString("Designation: ");
        string phone = Input::getString("Phone: ");
        string address = Input::getString("Address: ");
        string joiningDate = Input::getString("Joining Date: ");
        int salary = Input::getInt("Salary: ");

        Employee employee( id, name, dept, designation, phone, address, joiningDate, salary );
        manager.addEmployee(employee);
    }

    void displayEmployee() {
        int id = Input::getInt("Enter Employee ID: ");
        manager.displayEmployee(id);
    }

    void updateEmployee() {
        int id = Input::getInt( "Enter Employee ID to update: ");
        manager.updateEmployee(id);
    }

    void deleteEmployee() {
        int id = Input::getInt( "Enter Employee ID to delete: " );

        if (manager.deleteEmployee(id)) {
            cout << GREEN << "Employee deleted successfully!\n" << RESET;
        }
        else {
            cout << RED << "Employee not found!\n" << RESET;
        }
    }

    void calculateSalary() {

        int id = Input::getInt( "Enter Employee ID: " );

        Employee* employee = manager.findById(id);

        if (employee == nullptr) {
            cout << RED << "Employee not found!\n" << RESET;
            return;
        }

        Payroll payroll;
        payroll.inputPayrollDetails();
        payroll.displaySummary(*employee);
        Payslip::generate( *employee, payroll);
    }
};

class EmployeeUser : public User {

private:
    EmployeeManager& manager;

public:

    EmployeeUser( const string& user, EmployeeManager& employeeManager ) : User(user),  manager(employeeManager) {}

    void showMenu() const override {
        cout << "\n" << CYAN << "======== EMPLOYEE MENU ========" << RESET << "\n";
        cout << "1. View My Details\n";
        cout << "2. Exit\n";
    }

    void run(int employeeId) {
        while (true) {
            showMenu();
            int choice = Input::getInt("Enter choice: ");

            switch (choice) {
                case 1:
                    manager.displayEmployee( employeeId);
                    break;

                case 2:
                    cout << "Returning to Main Menu...\n";
                    return;

                default:
                    cout << RED << "Invalid choice!\n" << RESET;
            }
        }
    }
};

class Authentication {

private:
    static constexpr const char* ADMIN_PASSWORD = "PassCode@26";

public:

    static bool adminLogin() {
        const int MAX_ATTEMPTS = 3;
        for (int attempt = 1; attempt <= MAX_ATTEMPTS; ++attempt) {
            string password = Input::getString( "Enter Admin Password (Attempt " + to_string(attempt) + "/3): " );

            if (password == ADMIN_PASSWORD) {
                cout << GREEN << "Login successful!\n" << RESET;
                return true;
            }
            cout << RED << "Incorrect password!\n" << RESET;
        }
        return false;
    }
};

class EmployeeManagementSystem {
private:
    EmployeeManager manager;

public:
    void run() {
        cout << CYAN << "\n============================================\n" << "       EMPLOYEE MANAGEMENT SYSTEM\n" << "============================================\n" << RESET;
        while (true) {
            showMainMenu();
            int choice = Input::getInt("Enter choice: ");

            switch (choice) {
                case 1:
                    adminLogin();
                    break;

                case 2:
                    employeeLogin();
                    break;

                case 3:
                    cout << GREEN << "Exiting program...\n" << RESET;
                    return;

                default:
                    cout << RED << "Invalid choice!\n" << RESET;
            }
        }
    }

private:
    void showMainMenu() const {

        cout << "\n" << YELLOW << "========== MAIN MENU ==========" << RESET << "\n";
        cout << "1. Admin\n";
        cout << "2. Employee\n";
        cout << "3. Exit\n";
    }

    void adminLogin() {
        if (!Authentication::adminLogin()) {
            cout << RED << "Access denied after 3 failed attempts.\n" << RESET;
            return;
        }

        Admin admin( "Administrator", manager );
        admin.run();
    }

    void employeeLogin() {
        int employeeId = Input::getInt( "Enter your Employee ID: " );        

        if (manager.findById(employeeId) == nullptr) {
            cout << RED << "Employee not found!\n" << RESET;
            return;
        }

        EmployeeUser employeeUser( "Employee",manager );
        employeeUser.run(employeeId);
    }
};

int main() {

    try {
        EmployeeManagementSystem system;
        system.run();
    }
    catch (const exception& e) {

        cerr << RED << "Fatal Error: " << e.what() << RESET << '\n';
        return 1;
    }
    return 0;
}