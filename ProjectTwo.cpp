#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <limits>

struct Course {
    std::string courseNumber;
    std::string courseTitle;
    std::vector<std::string> prerequisites;

    Course(std::string num = "", std::string title = "", std::vector<std::string> prereqs = {})
        : courseNumber(num), courseTitle(title), prerequisites(prereqs) {}
    bool operator<(const Course& other) const {
        return courseNumber < other.courseNumber;  // Operator for sorting courses alphanumerically
    }
};

class CourseHashTable {
private:
    static constexpr int TABLE_SIZE = 100;  // Adjusted size for hash table
    std::vector<Course> table[TABLE_SIZE];  // Array of vectors for chaining to handle collisions

    int hashFunction(const std::string& courseNumber) const {
        int sum = 0;
        for (char c : courseNumber) {
            sum += static_cast<int>(c);  // Sum character codes for hashing
        }
        return sum % TABLE_SIZE;  // Modulo to fit within table size
    }

public:
    void insert(const Course& course) {  // Load data into hash table
        int index = hashFunction(course.courseNumber);
        // Check for duplicates
        for (const Course& existing : table[index]) {
            if (existing.courseNumber == course.courseNumber) {
                return; // Skip if course already exists
            }
        }
        table[index].push_back(course);  // Add course to the appropriate bucket
    }

    const Course* find(const std::string& courseNumber) const { // Search for course info
        int index = hashFunction(courseNumber);
        for (const Course& course : table[index]) {
            if (course.courseNumber == courseNumber) {
                return &course;  // Return pointer to found course
            }
        }
        return nullptr;  // Return null if not found
    }

    std::vector<Course> getAllCourses() const {  // Get all courses for sorting and printing
        std::vector<Course> allCourses;
        for (int i = 0; i < TABLE_SIZE; i++) {
            for (const Course& course : table[i]) {
                allCourses.push_back(course);  // Collect all courses from buckets
            }
        }
        std::sort(allCourses.begin(), allCourses.end());  // Sort alphanumerically
        return allCourses;
    }
};

std::vector<std::string> split(const std::string& line, char delim = ',') {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(line + delim); // Add delim to catch the last token
    while (std::getline(ss, token, delim)) {
        if (!token.empty()) {
            tokens.push_back(token);  // Split line into tokens based on comma delimiter
        }
    }
    return tokens;
}

bool loadCourses(CourseHashTable& hashTable) {  // Read course data file
    const std::string filename = "CS 300 ABCU_Advising_Program_Input.csv";  // Implicitly prompted file name
    std::ifstream file(filename);  // Open file from working directory
    int loadedCount = 0;

    if (file.is_open()) {
        std::cout << "Reading from file: " << filename << std::endl;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                std::vector<std::string> tokens = split(line);
                if (tokens.size() < 2) {
                    std::cout << "Error: Skipping malformed line: " << line << "\n";  // Error handling
                    continue;
                }
                Course course;
                course.courseNumber = tokens[0];
                course.courseTitle = tokens[1];
                for (size_t i = 2; i < tokens.size(); ++i) {
                    course.prerequisites.push_back(tokens[i]);  // Store prerequisites
                }
                hashTable.insert(course);  // Insert into hash table
                loadedCount++;
                std::cout << "Loaded: " << course.courseNumber << " - " << course.courseTitle << "\n";
            }
        }
        file.close();
        std::cout << loadedCount << " courses loaded successfully.\n";  // successfully read
    }
    else {
        std::cout << "Error: Could not open file " << filename << ". Please make sure CS 300 ABCU_Advising_Program_Input.csv is in the working directory.\n";  // Error message
        return false; // Unsuccessful attempt
    }

    return loadedCount > 0;
}

void printCSCourses(const CourseHashTable& hashTable) {  // Print courses
    std::vector<Course> courses = hashTable.getAllCourses();
    bool found = false;
    for (const Course& course : courses) {
        if (course.courseNumber.find("CSCI") != std::string::npos ||
            course.courseNumber.find("MATH") != std::string::npos) {
            std::cout << course.courseNumber << ": " << course.courseTitle << std::endl;  // Print alphanumerically sorted list
            found = true;
        }
    }
    if (!found) {
        std::cout << "No Computer Science or Math courses found.\n";  // Error handling
    }
}

void printCourseInfo(const CourseHashTable& hashTable, const std::string& courseNumber) {  // Print course info
    const Course* course = hashTable.find(courseNumber);  // Search for course using hash table
    if (course) {
        std::cout << course->courseNumber << ": " << course->courseTitle << std::endl;  // Print course title
        if (course->prerequisites.empty()) {
            std::cout << "No prerequisites required.\n";  // Handle case with no prerequisites
        }
        else {
            std::cout << "Prerequisites: ";  // Start prerequisite list
            for (size_t i = 0; i < course->prerequisites.size(); ++i) {
                std::cout << course->prerequisites[i];  // Print each prerequisite course number
                if (i < course->prerequisites.size() - 1) {
                    std::cout << ", ";  // Add comma between prerequisites
                }
            }
            std::cout << std::endl;  // End prerequisite list
        }
    }
    else {
        std::cout << "Error: Course " << courseNumber << " not found.\n";  // Error handling for invalid course
    }
}

int getMenuChoice() {  // Create menu for user options
    int choice;
    std::cout << "\nCourse Management System\n";
    std::cout << "1. Load course data\n";
    std::cout << "2. Print Computer Science and Math courses\n";
    std::cout << "3. Print course information\n";
    std::cout << "9. Exit\n";
    std::cout << "Enter your choice: ";
    while (!(std::cin >> choice) || (choice != 1 && choice != 2 && choice != 3 && choice != 9)) {
        std::cout << "Invalid input. Please enter 1, 2, 3, or 9: ";  // Error message for invalid input
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Input validation
    }
    return choice;
}

int main() {
    CourseHashTable hashTable;
    bool dataLoaded = false;

    while (true) {
        int choice = getMenuChoice();
        if (choice == 1) {
            std::cout << "Attempting to load CS 300 ABCU_Advising_Program_Input.csv...\n";  // Implicit prompt for file
            dataLoaded = loadCourses(hashTable);  // Load data into structure
        }
        else if (choice == 2) {
            if (!dataLoaded) {
                std::cout << "Error: Please load course data first.\n";  // Error handling
            }
            else {
                printCSCourses(hashTable);
            }
        }
        else if (choice == 3) {
            if (!dataLoaded) {
                std::cout << "Error: Please load course data first.\n";  // Error handling
            }
            else {
                std::string courseNumber;
                std::cout << "Enter course number: ";  // Prompt for course number
                std::cin >> courseNumber;
                printCourseInfo(hashTable, courseNumber);
            }
        }
        else if (choice == 9) {
            std::cout << "Exiting program.\n";  // Exit option
            break;
        }
        // Clear input buffer after each choice to avoid issues
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return 0;
}